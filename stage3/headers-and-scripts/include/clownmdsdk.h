/*
Copyright (c) 2024-2025 Clownacy

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef CLOWNSDK_MD_HEADER_GUARD
#define CLOWNSDK_MD_HEADER_GUARD

#ifdef __cplusplus
#define __VISIBILITY extern "C"
#else
#define __VISIBILITY
#endif

#define __ENTRY_POINT __VISIBILITY  __attribute__((section(".text.entry"))) [[noreturn]]
#define __INTERRUPT_HANDLER __VISIBILITY __attribute__((interrupt))

__ENTRY_POINT void _EntryPoint(void);

// M68000 exception handlers.
__INTERRUPT_HANDLER void _BusErrorHandler(void);
__INTERRUPT_HANDLER void _AddressErrorHandler(void);
__INTERRUPT_HANDLER void _IllegalInstructionHandler(void);
__INTERRUPT_HANDLER void _DivisionByZeroHandler(void);
__INTERRUPT_HANDLER void _CHKHandler(void);
__INTERRUPT_HANDLER void _TRAPVHandler(void);
__INTERRUPT_HANDLER void _PrivilegeViolationHandler(void);
__INTERRUPT_HANDLER void _TraceHandler(void);
__INTERRUPT_HANDLER void _UnimplementedInstructionLineAHandler(void);
__INTERRUPT_HANDLER void _UnimplementedInstructionLineFHandler(void);
__INTERRUPT_HANDLER void _UnassignedHandler(void);
__INTERRUPT_HANDLER void _UninitialisedInterruptHandler(void);
__INTERRUPT_HANDLER void _SpuriousInterruptHandler(void);
__INTERRUPT_HANDLER void _Level1InterruptHandler(void);
__INTERRUPT_HANDLER void _Level2InterruptHandler(void);
__INTERRUPT_HANDLER void _Level3InterruptHandler(void);
__INTERRUPT_HANDLER void _Level4InterruptHandler(void);
__INTERRUPT_HANDLER void _Level5InterruptHandler(void);
__INTERRUPT_HANDLER void _Level6InterruptHandler(void);
__INTERRUPT_HANDLER void _Level7InterruptHandler(void);
__INTERRUPT_HANDLER void _TRAP0Handler(void);
__INTERRUPT_HANDLER void _TRAP1Handler(void);
__INTERRUPT_HANDLER void _TRAP2Handler(void);
__INTERRUPT_HANDLER void _TRAP3Handler(void);
__INTERRUPT_HANDLER void _TRAP4Handler(void);
__INTERRUPT_HANDLER void _TRAP5Handler(void);
__INTERRUPT_HANDLER void _TRAP6Handler(void);
__INTERRUPT_HANDLER void _TRAP7Handler(void);
__INTERRUPT_HANDLER void _TRAP8Handler(void);
__INTERRUPT_HANDLER void _TRAP9Handler(void);
__INTERRUPT_HANDLER void _TRAP10Handler(void);
__INTERRUPT_HANDLER void _TRAP11Handler(void);
__INTERRUPT_HANDLER void _TRAP12Handler(void);
__INTERRUPT_HANDLER void _TRAP13Handler(void);
__INTERRUPT_HANDLER void _TRAP14Handler(void);
__INTERRUPT_HANDLER void _TRAP15Handler(void);

// Runs once at startup, before anything else.
__VISIBILITY __attribute__((section(".text.entry"))) void _SP_Init(void);

// Run indefinitely; should not return. Handles the bulk of operations.
__VISIBILITY [[noreturn]] void _SP_Main(void); /* TODO: This can return. */

// Runs once per frame, either 50 or 60 times a second for PAL or NTSC respectively.
__VISIBILITY void _SP_VerticalInterrupt(void);

// Runs in response to the user interrupt, which only occurs when manually triggered by the programmer.
__VISIBILITY void _SP_User(void);

#if defined(__cplusplus) && __cplusplus >= 202302L

#include <atomic>
#include <bit>
#include <cassert>
#include <cstdint>
#include <limits>
#include <optional>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>

#include <clownlzss/decompressors/kosinski.h>

#define __BIND_ADDRESS(ADDRESS, NAME, ...) static auto &NAME = *reinterpret_cast<__VA_ARGS__*>(ADDRESS)

// TODO: Move this to its own translation unit? Optimisation can be handled by LTO.
namespace ClownMDSDK
{
	namespace MainCPU
	{
		namespace Unsafe
		{
			__BIND_ADDRESS(0xA10001, version_register, volatile unsigned char);

			static constexpr unsigned int total_io_ports = 3;
			__BIND_ADDRESS(0xA10002, io_data, std::array<volatile unsigned short, total_io_ports>);
			__BIND_ADDRESS(0xA10008, io_ctrl, std::array<volatile unsigned short, total_io_ports>);

			inline bool IsPAL()
			{
				return (version_register & 1 << 6) != 0;
			}

			inline bool IsMegaCDConnected()
			{
				return (version_register & 1 << 5) == 0;
			}
		}

		static constexpr unsigned long master_clock = 53693175;

		namespace M68k
		{
			static constexpr unsigned long clock = master_clock / 7;

			inline unsigned int GetStatusRegister()
			{
				unsigned short sr;
				asm(
					"move.w	%%sr,%0"
					: "=dQUm" (sr)
				);
				return sr;
			}

			inline unsigned int GetInterruptMask(const unsigned int sr)
			{
				return sr >> 8 & 7;
			}

			inline unsigned int GetInterruptMask()
			{
				return GetInterruptMask(GetStatusRegister());
			}

			inline unsigned int SetInterruptMask(const unsigned int level)
			{
				const auto sr = GetStatusRegister();

				asm(
					"move.w	%0,%%sr"
					:
					: "id" ((sr & ~0x0700) | level << 8)
				);

				return GetInterruptMask(sr);
			}

			inline unsigned int DisableInterrupts()
			{
				return SetInterruptMask(7);
			}
		}

		namespace FM
		{
			namespace Unsafe
			{
				__BIND_ADDRESS(0xA04000, ports, std::array<volatile unsigned char, 4>);
				static auto &A0 = ports[0];
				static auto &D0 = ports[1];
				static auto &A1 = ports[2];
				static auto &D1 = ports[3];
			}

			static constexpr unsigned int sample_rate = M68k::clock / (6 * 6 * 4);
		}

		namespace Z80
		{
			namespace Unsafe
			{
				__BIND_ADDRESS(0xA00000, ram, std::array<volatile unsigned char, 0x2000>);
				__BIND_ADDRESS(0xA11100, bus_request, volatile unsigned short);
				__BIND_ADDRESS(0xA11200, reset, volatile unsigned short);

				inline void AssertReset()
				{
					asm volatile(
						"move.w	%1,%0\n"
						: "=Qm" (Unsafe::reset)
						: "dai" (0)
						: "cc"
					);
				}

				inline void DeassertReset()
				{
					asm volatile(
						"move.w	%1,%0\n"
						: "=Qm" (Unsafe::reset)
						: "dai" (0x100)
						: "cc"
					);
				}

				inline void RequestBus()
				{
					asm volatile(
						"move.w	%1,%0\n"
						: "=Qm" (Unsafe::bus_request)
						: "dai" (0x100)
						: "cc"
					);
				}

				inline void WaitUntilBusObtained()
				{
					asm volatile(
						"0:\n"
						"	btst	%1,%0\n"
						"	bne.s	0b\n"
						: "=Qm" (Unsafe::bus_request)
						: "di" (0)
						: "cc"
					);
				}

				inline void ReleaseBus()
				{
					asm volatile(
						"move.w	%1,%0\n"
						: "=Qm" (Unsafe::bus_request)
						: "dai" (0)
						: "cc"
					);
				}

				inline void WaitUntilBusReleased()
				{
					asm volatile(
						"0:\n"
						"	btst	%1,%0\n"
						"	beq.s	0b\n"
						: "=Qm" (Unsafe::bus_request)
						: "di" (0)
						: "cc"
					);
				}
			}

			static constexpr unsigned long clock = master_clock / 15;

			inline void Reset()
			{
				Unsafe::AssertReset();
				asm(
						"nop\n"
					"	nop\n"
					"	nop\n"
					"	nop\n"
				);
				Unsafe::DeassertReset();
			}
		}

		namespace PSG
		{
			static constexpr unsigned long sample_rate = Z80::clock / 16;

			inline void Write(const unsigned char value)
			{
				asm(
					"move.b	%0,(0xC00011).l" // Must be an absolute long address so that the code is slow enough for writes to never be missed.
					:
					: "idQUm" (value)
					: "cc"
				);
			}
		}

		namespace VDP
		{
			struct Register00
			{
				unsigned int id : 8 = 0x80;
				bool bit7 : 1 = false;
				bool bit6 : 1 = false;
				bool blank_leftmode_8_pixels : 1;
				bool enable_horizontal_interrupt : 1;
				bool bit3 : 1 = false;
				bool bit2 : 1 = true;
				bool lock_hv_counter : 1;
				bool bit0 : 1 = false;
			};

			struct Register01
			{
				unsigned int id : 8 = 0x81;
				bool bit7 : 1 = false;
				bool enable_display : 1;
				bool enable_vertical_interrupt : 1;
				bool enable_dma_transfer : 1;
				bool enable_v30_cell_mode : 1;
				bool enable_mega_drive_mode : 1;
				bool bit1 : 1 = false;
				bool bit0 : 1 = false;
			};

			struct Register02
			{
				unsigned int id : 8 = 0x82;
				bool bit7 : 1 = false;
				unsigned int plane_a_location : 4;
				bool bit2 : 1 = false;
				bool bit1 : 1 = false;
				bool bit0 : 1 = false;
			};

			struct Register03
			{
				unsigned int id : 8 = 0x83;
				bool bit7 : 1 = false;
				bool bit6 : 1 = false;
				unsigned int window_plane_location : 5;
				bool bit0 : 1 = false;
			};

			struct Register04
			{
				unsigned int id : 8 = 0x84;
				bool bit7 : 1 = false;
				bool bit6 : 1 = false;
				bool bit5 : 1 = false;
				bool bit4 : 1 = false;
				unsigned int plane_b_location : 4;
			};

			struct Register05
			{
				unsigned int id : 8 = 0x85;
				unsigned int sprite_table_location : 8;
			};

			struct Register07
			{
				unsigned int id : 8 = 0x87;
				bool bit7 : 1 = false;
				bool bit6 : 1 = false;
				unsigned int palette_line : 2;
				unsigned int palette_index : 4;
			};

			struct Register0A
			{
				unsigned int id : 8 = 0x8A;
				unsigned int horiziontal_interrupt_interval : 8;
			};

			struct Register0B
			{
				unsigned int id : 8 = 0x8B;
				bool bit7 : 1 = false;
				bool bit6 : 1 = false;
				bool bit5 : 1 = false;
				bool bit4 : 1 = false;
				bool enable_external_interrupt_level_2 : 1;
				unsigned int vertical_scroll_mode : 1;
				unsigned int horizontal_scroll_mode : 2;
			};

			struct Register0C
			{
				unsigned int id : 8 = 0x8C;
				bool enable_h40_cell_mode_1 : 1;
				bool bit6 : 1 = false;
				bool bit5 : 1 = false;
				bool bit4 : 1 = false;
				bool enable_shadow_highlight_mode : 1;
				unsigned int interlace_mode : 2;
				bool enable_h40_cell_mode_2 : 1;
			};

			struct Register0D
			{
				unsigned int id : 8 = 0x8D;
				bool bit7 : 1 = false;
				unsigned int horizontal_scroll_location : 7;
			};

			struct Register0F
			{
				unsigned int id : 8 = 0x8F;
				unsigned int increment : 8;
			};

			struct Register10
			{
				unsigned int id : 8 = 0x90;
				bool bit7 : 1 = false;
				bool bit6 : 1 = false;
				unsigned int vertical_size : 2;
				bool bit3 : 1 = false;
				bool bit2 : 1 = false;
				unsigned int horizontal_size : 2;
			};

			struct Register11
			{
				unsigned int id : 8 = 0x91;
				bool window_align_right : 1;
				bool bit6 : 1 = false;
				bool bit5 : 1 = false;
				unsigned int window_width : 5;
			};

			struct Register12
			{
				unsigned int id : 8 = 0x92;
				bool window_align_bottom : 1;
				bool bit6 : 1 = false;
				bool bit5 : 1 = false;
				unsigned int window_height : 5;
			};

			struct Register13
			{
				unsigned int id : 8 = 0x93;
				unsigned int dma_length_low : 8;
			};

			struct Register14
			{
				unsigned int id : 8 = 0x94;
				unsigned int dma_length_high : 8;
			};

			struct Register15
			{
				unsigned int id : 8 = 0x95;
				unsigned int dma_source_low : 8;
			};

			struct Register16
			{
				unsigned int id : 8 = 0x96;
				unsigned int dma_source_middle : 8;
			};

			struct Register17
			{
				unsigned int id : 8 = 0x97;
				unsigned int dma_mode : 1; // TODO: Enum?
				unsigned int dma_source_high : 7;
			};

			template<typename T>
			concept Register = requires(T reg)
			{
				reg.id;
				std::bit_cast<unsigned short>(reg);
			};

			namespace CRAM
			{
				class Colour
				{
					private:
						unsigned short colour;

					public:
						Colour(const unsigned short colour) : colour(colour) {}
						Colour(const unsigned int red, const unsigned int green, const unsigned int blue) : colour(blue << 9 | green << 5 | red << 1) {}
						unsigned short GetRaw() const {return colour;}

						static constexpr unsigned int Size() {return sizeof(colour);}
				};

				static_assert(std::is_standard_layout_v<Colour>); // Make sure that this is just a wrapped 'unsigned short'.
			}

			namespace VRAM
			{
				struct TileMetadata
				{
					bool priority : 1;
					unsigned int palette_line : 2;
					bool y_flip : 1;
					bool x_flip : 1;
					unsigned int tile_index : 11;
				};

				static_assert(std::is_standard_layout_v<TileMetadata> && sizeof(TileMetadata) == 2); // Make sure that this has no hidden data.
			}

			template<typename T>
			struct ValueWrapper
			{
				T value;

				constexpr ValueWrapper(const T value) : value(value) {}
			};

			struct DataValueWord : public ValueWrapper<unsigned short>
			{
				using ValueWrapper::ValueWrapper;

				constexpr DataValueWord(const CRAM::Colour colour) : ValueWrapper(colour.GetRaw()) {}
				constexpr DataValueWord(const VRAM::TileMetadata tile_metadata) : ValueWrapper(std::bit_cast<unsigned short>(tile_metadata)) {}
			};

			struct DataValueLongword : public ValueWrapper<unsigned long>
			{
				using ValueWrapper::ValueWrapper;

				constexpr DataValueLongword(const CRAM::Colour colour1, const CRAM::Colour colour2) : ValueWrapper(static_cast<unsigned long>(colour1.GetRaw()) << 16 | colour2.GetRaw()) {}
			};

			struct ControlValueWord : public ValueWrapper<unsigned short>
			{
				using ValueWrapper::ValueWrapper;

				template<Register T>
				constexpr ControlValueWord(const T &reg) : ValueWrapper(std::bit_cast<unsigned short>(reg)) {}
			};

			struct ControlValueLongword : public ValueWrapper<unsigned long>
			{
				using ValueWrapper::ValueWrapper;

				template<Register T1, Register T2>
				constexpr ControlValueLongword(const T1 &reg1, const T2 &reg2) : ValueWrapper(static_cast<unsigned long>(std::bit_cast<unsigned short>(reg1)) << 16 | std::bit_cast<unsigned short>(reg2)) {}
			};

			enum class RAM : unsigned int
			{
				VRAM  = 0x21,
				CRAM  = 0x2B,
				VSRAM = 0x25
			};

			enum class Access : unsigned int
			{
				READ  = 0x0C,
				WRITE = 0x07,
				DMA   = 0x27
			};

			__BIND_ADDRESS(0xC00000, data_port_word, volatile unsigned short);
			__BIND_ADDRESS(0xC00000, data_port_longword, volatile unsigned long);
			__BIND_ADDRESS(0xC00004, control_port_word, volatile unsigned short);
			__BIND_ADDRESS(0xC00004, control_port_longword, volatile unsigned long);
			__BIND_ADDRESS(0xC00004 + 1, control_port_low_byte, volatile unsigned char);

			inline void Write(const DataValueWord value)
			{
				asm volatile(
					"move.w	%1,%0"
					: "=Qm" (data_port_word)
					: "daim" (value) // TODO: Other holders?
					: "cc"
				);
			}

			inline void Write(const DataValueLongword value)
			{
				asm volatile(
					"move.l	%1,%0"
					: "=Qm" (data_port_word) // I would use 'data_port_longword', but using 'data_port_word' makes the compiler share pointers with WriteDataPortWord.
					: "daim" (value) // TODO: Other holders?
					: "cc"
				);
			}

			inline void Write(const ControlValueWord value)
			{
				asm volatile(
					"move.w	%1,%0"
					: "=Qm" (VDP::control_port_word)
					: "daim" (value) // TODO: Other holders?
					: "cc"
				);
			}

			inline void Write(const ControlValueLongword value)
			{
				asm volatile(
					"move.l	%1,%0"
					: "=Qm" (VDP::control_port_word) // I would use 'control_port_longword', but using 'control_port_word' makes the compiler share pointers with WriteControlPortWord.
					: "daim" (value) // TODO: Other holders?
					: "cc"
				);
			}

			template<typename... Ts>
			inline void Write(const std::tuple<Ts...> &data)
			{
				std::apply([](auto&&... args){((Write(args)), ...);}, data);
			}

			template<typename T1, typename T2, typename... Ts>
			inline void Write(const T1 &data1, const T2 &data2, const Ts&... other_data)
			{
				Write(std::make_tuple(data1, data2, other_data...));
			}

			inline constexpr ControlValueLongword MakeCommand(const RAM ram, const Access access, const unsigned int address)
			{
				const unsigned int cd = std::to_underlying(ram) & std::to_underlying(access);
				return static_cast<unsigned long>(cd & 3) << 30 | static_cast<unsigned long>(address & 0x3FFF) << 16 | (cd & 0x3C) << 2 | (address & 0xC000) >> 14;
			}

			inline void SendCommand(const RAM ram, const Access access, const unsigned int address)
			{
				Write(MakeCommand(ram, access, address));
			}

			template<typename T, unsigned int bit_index_power>
			inline constexpr auto RepeatBits(const T value)
			{
				T accumulator = value;
				for (unsigned int i = 0; i < std::bit_ceil(static_cast<unsigned int>(std::numeric_limits<T>::digits)) - bit_index_power; ++i)
					accumulator |= accumulator << (1 << (bit_index_power + i));
				return accumulator;
			}

			// Specialisations, because GCC's optimiser is stupid.
			template<>
			inline auto RepeatBits<unsigned short, 3>(const unsigned short value)
			{
				return value << 8 | value;
			}

			template<>
			inline auto RepeatBits<unsigned long, 4>(const unsigned long value)
			{
				return value << 16 | value;
			}

			inline void SetAddressIncrement(const unsigned int increment)
			{
				Write(Register0F{.increment = increment});
			}

			inline constexpr ControlValueLongword MakeDMALengthCommand(const unsigned int length)
			{
				return {Register13{.dma_length_low = length & 0xFF}, Register14{.dma_length_high = length >> 8}};
			}

			inline void SetDMALength(const unsigned int length)
			{
				Write(MakeDMALengthCommand(length));
			}

			namespace Unsafe
			{
				// WARNING: Make sure to request the Z80 bus before sending these commands to the VDP,
				// otherwise the VDP or Z80 may read garbage data.
				inline constexpr auto MakeDMACopyCommands(const RAM ram, const unsigned int address, const void* const data, const unsigned int length)
				{
					// TODO: 128KiB boundary.
					const auto source_address = std::bit_cast<std::uintptr_t>(data);

					return std::make_tuple(
						MakeDMALengthCommand(length),
						Register15{.dma_source_low = source_address >> (8 * 0 + 1)},
						Register16{.dma_source_middle = source_address >> (8 * 1 + 1)},
						Register17{.dma_mode = 0, .dma_source_high = source_address >> (8 * 2 + 1)},
						MakeCommand(ram, Access::DMA, address));
				}

				// WARNING: Make sure to request the Z80 bus before calling this function,
				// otherwise the VDP or Z80 may read garbage data.
				inline void CopyWordsWithDMA(const RAM ram, const unsigned int address, const void* const data, const unsigned int length)
				{
					// TODO: Use 'always_inline', like FillBytesWithDMA.
					// TODO: 128KiB boundary.
					const auto source_address = std::bit_cast<std::uintptr_t>(data);

					SetDMALength(length);
					Write(Register15{.dma_source_low = source_address >> (8 * 0 + 1)});
					Write(Register16{.dma_source_middle = source_address >> (8 * 1 + 1)});
					Write(Register17{.dma_mode = 0, .dma_source_high = source_address >> (8 * 2 + 1)});

					// According to Sega's 'GENESIS SOFTWARE MANUAL', the write
					// that triggers the DMA transfer must be from memory.
					asm volatile(
							"move.l	%1,-(%%sp)\n"
						"	move.l	(%%sp)+,%0"
						: "=Qm" (control_port_word)
						: "daim" (MakeCommand(ram, Access::DMA, address)) // TODO: Other holders?
						: "cc"
					);
				}
			}

			inline void CopyWordsWithoutDMA(const RAM ram, const unsigned int address, const void* const data, const unsigned int length)
			{
				SendCommand(ram, Access::WRITE, address);

				const unsigned long *data_pointer = static_cast<const unsigned long*>(data);

				for (unsigned int i = 0; i < length / 2; ++i)
				{
					Write(DataValueLongword(*data_pointer));
					++data_pointer;
				}

				if (length % 2 != 0)
					Write(DataValueWord(*reinterpret_cast<const unsigned short*>(data_pointer)));
			}

			inline void FillWordsWithoutDMA(const RAM ram, const unsigned int address, const unsigned int length, const unsigned int value)
			{
				SendCommand(ram, Access::WRITE, address);

				const auto longword_value = DataValueLongword(RepeatBits<unsigned long, 4>(value));
				const auto word_value = DataValueWord(value);

				for (unsigned int i = 0; i < length / 2; ++i)
					Write(longword_value);

				if (length % 2 != 0)
					Write(word_value);
			}

			inline void WaitUntilDMAIsComplete()
			{
				asm(
					"0:\n"
					"	btst	%1,%0\n"
					"	bne.s	0b\n"
					:
					: "Qm" (control_port_low_byte), "di" (1)
					: "cc"
				);
			}

			namespace VRAM
			{
				struct Sprite
				{
					unsigned int : 6;
					unsigned int y : 10;
					unsigned int : 4;
					unsigned int width : 2;
					unsigned int height : 2;
					unsigned int : 1;
					unsigned int link : 7;
					TileMetadata tile_metadata;
					unsigned int : 7;
					unsigned int x : 9;
				};

				static_assert(std::is_standard_layout_v<Sprite> && sizeof(Sprite) == 8); // Make sure that this has no hidden data.

				static constexpr unsigned int TILE_WIDTH = 8;
				static constexpr unsigned int TILE_HEIGHT_NORMAL = 8;
				static constexpr unsigned int TILE_HEIGHT_INTERLACE_MODE_2 = TILE_HEIGHT_NORMAL * 2;
				static constexpr unsigned int TILE_SIZE_IN_BYTES_NORMAL = TILE_WIDTH * TILE_HEIGHT_NORMAL / 2;
				static constexpr unsigned int TILE_SIZE_IN_BYTES_INTERLACE_MODE_2 = TILE_WIDTH * TILE_HEIGHT_INTERLACE_MODE_2 / 2;

				inline constexpr auto MakeDMAFillCommands(const unsigned int address, const unsigned int length, const unsigned int value)
				{
					return std::make_tuple(MakeDMALengthCommand(length - 1), Register17{.dma_mode = 1, .dma_source_high = 0}, MakeCommand(RAM::VRAM, Access::DMA, address), DataValueWord(RepeatBits<unsigned short, 3>(value)));
				}

				__attribute__((always_inline)) inline void FillBytesWithDMA(const unsigned int address, const unsigned int length, const unsigned int value)
				{
					// TODO: Don't use the tuple stuff: see CopyWordsWithDMA.
					Write(MakeDMAFillCommands(address, length, value));
				}

				inline void FillWordsWithoutDMA(const unsigned int address, const unsigned int length, const unsigned int value)
				{
					FillWordsWithoutDMA(RAM::VRAM, address, length, value);
				}

				inline void SetPlaneALocation(const unsigned int vram_address)
				{
					constexpr unsigned int divisor = 0x2000;
					assert(vram_address % divisor == 0);
					Write(Register02{.plane_a_location = vram_address / divisor});
				}

				inline void SetWindowPlaneLocation(const unsigned int vram_address)
				{
					constexpr unsigned int divisor = 0x800;
					assert(vram_address % divisor == 0);
					Write(Register03{.window_plane_location = vram_address / divisor});
				}

				inline void SetPlaneBLocation(const unsigned int vram_address)
				{
					constexpr unsigned int divisor = 0x2000;
					assert(vram_address % divisor == 0);
					Write(Register04{.plane_b_location = vram_address / divisor});
				}

				inline void SetSpriteTableLocation(const unsigned int vram_address)
				{
					constexpr unsigned int divisor = 0x200;
					assert(vram_address % divisor == 0);
					Write(Register05{.sprite_table_location = vram_address / divisor});
				}

				inline void SetHorizontalScrollLocation(const unsigned int vram_address)
				{
					constexpr unsigned int divisor = 0x400;
					assert(vram_address % divisor == 0);
					Write(Register0D{.horizontal_scroll_location = vram_address / divisor});
				}
			}

			namespace CRAM
			{
				constexpr unsigned int PALETTE_LINE_LENGTH = 16;
				constexpr unsigned int TOTAL_PALETTE_LINES = 4;
				constexpr unsigned int total_words = TOTAL_PALETTE_LINES * PALETTE_LINE_LENGTH;

				inline unsigned int PaletteLineAndIndexToOffset(const unsigned int palette_line, const unsigned int colour_index)
				{
					return (palette_line * PALETTE_LINE_LENGTH + colour_index) * Colour::Size();
				}

				inline void Set(const unsigned int palette_line, const unsigned int colour_index, const Colour colour)
				{
					Write(
						MakeCommand(RAM::CRAM, Access::WRITE, PaletteLineAndIndexToOffset(palette_line, colour_index)),
						DataValueWord(colour.GetRaw())
					);
				}

				inline void FillWordsWithoutDMA(const unsigned int address, const unsigned int length, const unsigned int value)
				{
					FillWordsWithoutDMA(RAM::CRAM, address, length, value);
				}

				inline void FillWordsWithoutDMA(const unsigned int address, const unsigned int length, const Colour colour)
				{
					FillWordsWithoutDMA(RAM::CRAM, address, length, colour.GetRaw());
				}

				inline void Fill(const Colour colour)
				{
					FillWordsWithoutDMA(0, total_words, colour);
				}
			}

			namespace VSRAM
			{
				constexpr unsigned int total_words = 40;

				inline void FillWordsWithoutDMA(const unsigned int address, const unsigned int length, const unsigned int value)
				{
					FillWordsWithoutDMA(RAM::VSRAM, address, length, value);
				}
			}
		}

		namespace Z80
		{
			class Bus
			{
			private:
				// Ideally, the compiler will resolve this variable
				// at compile-time and optimise it away completely.
				bool released = false;

			public:
				Bus(const bool wait_for_bus = true)
				{
					Unsafe::RequestBus();

					if (wait_for_bus)
						Unsafe::WaitUntilBusObtained();
				}

				Bus(const Bus &other) = delete;
				Bus& operator=(const Bus &other) = delete;

				~Bus()
				{
					Release();
				}

				void Release()
				{
					if (!released)
					{
						released = true;
						Unsafe::ReleaseBus();
					}
				}

				auto& RAM()
				{
					return Unsafe::ram;
				}

				auto& IOData()
				{
					return ClownMDSDK::MainCPU::Unsafe::io_data;
				}

				auto& IOData(const std::size_t index)
				{
					assert(index < total_io_ports);
					return IOData()[index];
				}

				auto& IOCtrl()
				{
					return ClownMDSDK::MainCPU::Unsafe::io_ctrl;
				}

				auto& IOCtrl(const std::size_t index)
				{
					assert(index < total_io_ports);
					return IOCtrl()[index];
				}

				bool IsConsolePAL()
				{
					return ClownMDSDK::MainCPU::Unsafe::IsPAL();
				}

				bool IsMegaCDConnected()
				{
					return ClownMDSDK::MainCPU::Unsafe::IsMegaCDConnected();
				}

				void WriteFMI(const unsigned char address, const unsigned char value)
				{
					asm(
						"0:\n"
						"	tst.b	(%0)\n"     // 8(2/0)
						"	bmi.s	0b\n"       // 10(2/0) | 8(1/0)
						"	move.b	%1,(%0)\n"  // 8(1/1)
						"	move.b	%2,1(%0)\n" // 12(2/1)
						"	nop\n"              // 4(1/0)
						"	nop\n"              // 4(1/0)
						"	nop\n"              // 4(1/0)
						:
						: "a" (FM::Unsafe::ports), "idQUm" (address), "idQUm" (value)
						: "cc"
					);
				}

				void WriteFMII(const unsigned char address, const unsigned char value)
				{
					asm(
						"0:\n"
						"	tst.b	(%0)\n"     // 8(2/0)
						"	bmi.s	0b\n"       // 10(2/0) | 8(1/0)
						"	move.b	%1,2(%0)\n" // 12(2/1)
						"	move.b	%2,3(%0)\n" // 12(2/1)
						"	nop\n"              // 4(1/0)
						"	nop\n"              // 4(1/0)
						"	nop\n"              // 4(1/0)
						:
						: "a" (FM::Unsafe::ports), "idQUm" (address), "idQUm" (value)
						: "cc"
					);
				}

				void CopyWordsToVDPWithDMA(const VDP::RAM ram, const unsigned int address, const void* const data, const unsigned int length)
				{
					VDP::Unsafe::CopyWordsWithDMA(ram, address, data, length);
				}

				struct ControlPad3Button
				{
					bool start : 1;
					bool a : 1;
					bool c : 1;
					bool b : 1;
					bool right : 1;
					bool left : 1;
					bool down : 1;
					bool up : 1;
				};

				void InitialiseIOPortAsControlPad3Button(const unsigned int port_index)
				{
					IOCtrl(port_index) = 0x40;
					IOData(port_index) = 0x40;
				}

				ControlPad3Button ReadIOPortAsControlPad3Button(const unsigned int port_index)
				{
					auto &data_port = IOData(port_index);

					const auto SetAndRead = [&data_port](const unsigned int value)
					{
						data_port = value;
						// It is necessary to wait for the data to latch.
						asm("nop");
						asm("nop");
						return data_port;
					};

					// Invert the values so that 'true' means 'held'.
					unsigned char output = 0xFF;

					// Read A and start.
					output ^= SetAndRead(0x00) << 2 & 0xC0;

					// Read up, down, left, right, C, and B.
					output ^= SetAndRead(0x40) << 0 & 0x3F;

					return std::bit_cast<ControlPad3Button>(output);
				}
			};

			class BusInterruptSafe : public Bus
			{
			private:
				const unsigned int interrupt_mask;

			public:
				BusInterruptSafe(const bool wait_for_bus = true)
					: interrupt_mask(M68k::DisableInterrupts())
					, Bus(wait_for_bus)
				{}

				~BusInterruptSafe()
				{
					M68k::SetInterruptMask(interrupt_mask);
				}
			};
		}

		namespace MegaCD
		{
			struct SubCPU
			{
				bool interrupt_level_2_mask : 1;
				bool bit14 : 1 = false;
				bool bit13 : 1 = false;
				bool bit12 : 1 = false;
				bool bit11 : 1 = false;
				bool bit10 : 1 = false;
				bool bit9 : 1 = false;
				bool raise_interrupt_level_2 : 1;
				bool bit7 : 1 = false;
				bool bit6 : 1 = false;
				bool bit5 : 1 = false;
				bool bit4 : 1 = false;
				bool bit3 : 1 = false;
				bool bit2 : 1 = false;
				bool bus_request : 1;
				bool NOT_reset : 1; // Active low.
			};

			__BIND_ADDRESS(0xA12000, subcpu, volatile SubCPU);

			struct MemoryMode
			{
				unsigned int write_protect : 8;
				unsigned int prg_ram_bank : 2;
				bool bit5 : 1 = false;
				bool bit4 : 1 = false;
				bool bit3 : 1 = false;
				bool word_ram_1m_mode : 1;
				bool dmna : 1;
				bool ret : 1;
			};

			__BIND_ADDRESS(0xA12002, memory_mode, volatile MemoryMode);

			static inline void ResetGateArray()
			{
				asm(
						"move.w	#0xFF00,(0xA12002).l\n"
					"	move.b	#3,(0xA12001).l\n"
					"	move.b	#2,(0xA12001).l\n"
					"	move.b	#0,(0xA12001).l\n"
				);
			}

			namespace CDC
			{
				enum class DeviceDestination : unsigned int
				{
					MAIN_CPU = 2,
					SUB_CPU  = 3,
					PCM_RAM  = 4,
					PRG_RAM  = 5,
					WORD_RAM = 7,
				};

				struct Mode
				{
					bool end_of_data_transfer : 1;
					bool data_set_ready : 1;
					bool bit13 : 1 = false;
					bool bit12 : 1 = false;
					bool bit11 : 1 = false;
					DeviceDestination device_destination : 3;
					bool bit7 : 1 = false;
					bool bit6 : 1 = false;
					bool bit5 : 1 = false;
					bool bit4 : 1 = false;
					bool bit3 : 1 = false;
					bool bit2 : 1 = false;
					bool bit1 : 1 = false;
					bool bit0 : 1 = false;
				};

				__BIND_ADDRESS(0xA12004, mode, volatile Mode);
				__BIND_ADDRESS(0xA12008, host_data, volatile unsigned short);
			}

			__BIND_ADDRESS(0xA12006, horizontal_interrupt_vector, volatile unsigned short);
			__BIND_ADDRESS(0xA1200C, stop_watch, volatile unsigned short);
			__BIND_ADDRESS(0xA1200E, communication_flag, std::atomic<unsigned short>);
			__BIND_ADDRESS(0xA1200E, communication_flag_ours, std::atomic<unsigned char>);
			__BIND_ADDRESS(0xA1200F, communication_flag_theirs, std::atomic<unsigned char>);
			__BIND_ADDRESS(0xA12010, communication_command, std::array<std::atomic<unsigned short>, 8>);
			__BIND_ADDRESS(0xA12020, communication_status, std::array<std::atomic<unsigned short>, 8>);

			struct JumpTable
			{
				struct Entry
				{
					unsigned short jmp_opcode;
					void (*address)();
				};

				Entry reset;
				Entry level_6;
				Entry level_4;
				Entry level_2;
				std::array<Entry, 0x10> trap;
				Entry chk;
				Entry address_error;
				Entry divide_by_zero;
				Entry trapv;
				Entry illegal_instruction_1;
				Entry illegal_instruction_2;
				Entry supervisor_error;
				Entry trace;
			};

			__BIND_ADDRESS(0xFFFFFD00, jump_table, volatile JumpTable);

			namespace CDBoot
			{
				template<typename T>
				__BIND_ADDRESS(0, boot_rom, const std::array<T, 128 * 1024UL / sizeof(T)>);

				template<typename T>
				__BIND_ADDRESS(0x20000, prg_ram_window, std::array<T, 128 * 1024UL / sizeof(T)>);

				template<typename T>
				__BIND_ADDRESS(0x200000, word_ram_2m, std::array<T, 256 * 1024UL / sizeof(T)>);
				template<typename T>
				__BIND_ADDRESS(0x200000, word_ram_1m, std::array<T, 128 * 1024UL / sizeof(T)>);

				static inline void GiveWordRAMToSubCPU()
				{
					// Ensure that the compiler understands that Word-RAM is "read" by this, so it needs to flush memory.
					asm(
						""
						:
						: "m" (word_ram_2m<unsigned char>)
					);

					memory_mode.dmna = true;
				}
			}

			namespace CartridgeBoot
			{
				template<typename T>
				__BIND_ADDRESS(0x400000, boot_rom, const std::array<T, 128 * 1024UL / sizeof(T)>);

				template<typename T>
				__BIND_ADDRESS(0x420000, prg_ram_window, std::array<T, 128 * 1024UL / sizeof(T)>);

				template<typename T>
				__BIND_ADDRESS(0x600000, word_ram_2m, std::array<T, 256 * 1024UL / sizeof(T)>);
				template<typename T>
				__BIND_ADDRESS(0x600000, word_ram_1m, std::array<T, 128 * 1024UL / sizeof(T)>);

				static inline void GiveWordRAMToSubCPU()
				{
					// Ensure that the compiler understands that Word-RAM is "read" by this, so it needs to flush memory.
					asm(
						""
						:
						: "m" (word_ram_2m<unsigned char>)
					);

					memory_mode.dmna = true;
				}

				template<typename T>
				static inline bool InitialiseSubCPU(const std::span<const T> &subcpu_payload)
				{
					{
						Z80::Bus z80_bus;
						if (!z80_bus.IsMegaCDConnected())
							return false;
						z80_bus.Release();
					}

					// Find the SUB-CPU BIOS payload.
					const auto sub_cpu_bios_payload_offset = []() -> unsigned long
					{
						if (boot_rom<unsigned short>[0x1586E / 2] == ('E' << 8 | 'G')) // "SEGA"
							return 0x15800; // Western BIOS
						if (boot_rom<unsigned short>[0x1606E / 2] == ('E' << 8 | 'G')) // "SEGA"
							return 0x16000; // Regular BIOS
						if (boot_rom<unsigned short>[0x1606E / 2] == ('O' << 8 | 'N')) // "WONDER"
							return 0x16000; // WonderMega/X'Eye BIOS
						if (boot_rom<unsigned short>[0x1AD6E / 2] == ('E' << 8 | 'G')) // "SEGA"
							return 0x1AD00; // LaserActive BIOS
						return 0;
					}();

					if (sub_cpu_bios_payload_offset == 0)
						return false;

					// Reset the hardware.
					ResetGateArray();

					// ResetGateArray sets 'write_protect' to '0xFF', so let's reset it here.
					memory_mode.write_protect = 0;

					// Stop and reset SUB-CPU.
					subcpu.bus_request = true;
					subcpu.NOT_reset = false;
					while (!subcpu.bus_request);

					// Clear PRG-RAM.
					for (unsigned int i = 0; i < 4; ++i)
					{
						memory_mode.prg_ram_bank = i;
						std::fill(std::begin(prg_ram_window<unsigned long>), std::end(prg_ram_window<unsigned long>), 0);
					}
					memory_mode.prg_ram_bank = 0;

					// Decompress SUB-CPU BIOS payload to PRG-RAM.
					ClownLZSS::KosinskiDecompress(&boot_rom<unsigned char>[sub_cpu_bios_payload_offset], &prg_ram_window<unsigned char>[0]);

					// Upload our SUB-CPU payload to PRG-RAM.
					std::copy(std::cbegin(subcpu_payload), std::cend(subcpu_payload), &prg_ram_window<unsigned char>[0x6000]);

					// Send WORD-RAM to SUB-CPU.
					GiveWordRAMToSubCPU();

					// Restart the SUB-CPU.
					subcpu.bus_request = false;
					subcpu.NOT_reset = true;
					while (!subcpu.NOT_reset);

					return true;
				}
			}
		}
	}

	namespace SubCPU
	{
		template<typename T>
		__BIND_ADDRESS(0x80000, word_ram_2m, std::array<T, 256 * 1024UL / sizeof(T)>);
		template<typename T>
		__BIND_ADDRESS(0xC0000, word_ram_1m, std::array<T, 128 * 1024UL / sizeof(T)>);

		struct Status
		{
			bool bit15 : 1 = false;
			bool bit14 : 1 = false;
			bool bit13 : 1 = false;
			bool bit12 : 1 = false;
			bool bit11 : 1 = false;
			bool bit10 : 1 = false;
			bool led_green : 1;
			bool led_red : 1;
			unsigned int version : 4;
			bool bit3 : 1 = false;
			bool bit2 : 1 = false;
			bool bit1 : 1 = false;
			bool NOT_reset : 1; // Active low.
		};

		__BIND_ADDRESS(0xFFFF8000, status, volatile Status);

		struct MemoryMode
		{
			unsigned int write_protect : 8;
			bool bit7 : 1 = false;
			bool bit6 : 1 = false;
			bool bit5 : 1 = false;
			unsigned int priority_mode : 2; // TODO: Priority mode enum.
			bool word_ram_1m_mode : 1;
			bool dmna : 1;
			bool ret : 1;
		};

		__BIND_ADDRESS(0xFFFF8002, memory_mode, volatile MemoryMode);

		static inline void GiveWordRAMToMainCPU()
		{
			// Ensure that the compiler understands that Word-RAM is "read" by this, so it needs to flush memory.
			asm(
				""
				:
				: "m" (word_ram_2m<unsigned char>)
			);

			memory_mode.ret = true;
		}

		namespace CDC
		{
			using DeviceDestination = MainCPU::MegaCD::CDC::DeviceDestination;

			struct Mode
			{
				bool end_of_data_transfer : 1;
				bool data_set_ready : 1;
				bool upper_byte_read : 1;
				bool bit12 : 1 = false;
				bool bit11 : 1 = false;
				DeviceDestination device_destination : 3; // TODO: Destination enum.
				bool bit7 : 1 = false;
				bool bit6 : 1 = false;
				bool bit5 : 1 = false;
				bool bit4 : 1 = false;
				unsigned int register_address : 4;
			};

			__BIND_ADDRESS(0xFFFF8004, mode, volatile Mode);
			__BIND_ADDRESS(0xFFFF8006, register_data, volatile unsigned short);
			__BIND_ADDRESS(0xFFFF8008, host_data, volatile unsigned short);
			__BIND_ADDRESS(0xFFFF800A, dma_address, volatile unsigned short);

			static inline void SetDMAAddress(const std::uintptr_t address)
			{
				assert((address % 8) != 0);
				dma_address = address / 8;
			}
		}

		__BIND_ADDRESS(0xFFFF800C, stop_watch, volatile unsigned short);
		__BIND_ADDRESS(0xFFFF800E, communication_flag, std::atomic<unsigned short>);
		__BIND_ADDRESS(0xFFFF800E, communication_flag_theirs, std::atomic<unsigned char>);
		__BIND_ADDRESS(0xFFFF800F, communication_flag_ours, std::atomic<unsigned char>);
		__BIND_ADDRESS(0xFFFF8010, communication_command, std::array<std::atomic<unsigned short>, 8>);
		__BIND_ADDRESS(0xFFFF8020, communication_status, std::array<std::atomic<unsigned short>, 8>);
		__BIND_ADDRESS(0xFFFF8030, timer_interrupt_level_3, volatile unsigned short);

		struct InterruptMaskControl
		{
			bool bit15 : 1 = false;
			bool bit14 : 1 = false;
			bool bit13 : 1 = false;
			bool bit12 : 1 = false;
			bool bit11 : 1 = false;
			bool bit10 : 1 = false;
			bool bit9 : 1 = false;
			bool bit8 : 1 = false;
			bool bit7 : 1 = false;
			bool level_6_subcode : 1;
			bool level_5_cdc : 1;
			bool level_4_cdd : 1;
			bool level_3_timer : 1;
			bool level_2_md : 1;
			bool level_1_graphics : 1;
			bool bit0 : 1 = false;
		};

		__BIND_ADDRESS(0xFFFF8032, interrupt_mask_control, volatile InterruptMaskControl);

		struct CDFader
		{
			bool end_of_data_transfer : 1;
			unsigned int volume_data : 11;
			unsigned int deemphasis_flag : 2; // TODO: Enum.
			bool bit1 : 1 = false;
			bool bit0 : 1 = false;
		};

		__BIND_ADDRESS(0xFFFF8034, cd_fader, volatile CDFader);

		struct CDDControl
		{
			bool bit15 : 1 = false;
			bool bit14 : 1 = false;
			bool bit13 : 1 = false;
			bool bit12 : 1 = false;
			bool bit11 : 1 = false;
			bool bit10 : 1 = false;
			bool bit9 : 1 = false;
			bool data_or_music : 1;
			bool bit7 : 1 = false;
			bool bit6 : 1 = false;
			bool bit5 : 1 = false;
			bool bit4 : 1 = false;
			bool bit3 : 1 = false;
			bool host_clock : 1; // By setting this, commands are automatically fed to the CDD.
			bool data_receiving_status : 1;
			bool data_transmission_status : 1;
		};

		__BIND_ADDRESS(0xFFFF8036, cdd_control, volatile CDDControl);

		__BIND_ADDRESS(0xFFFF8038, cdd_receiving_status, std::array<volatile unsigned char, 10>);
		__BIND_ADDRESS(0xFFFF8042, cdd_transmission_command, std::array<volatile unsigned char, 10>);

		struct FontColour
		{
			bool bit15 : 1 = false;
			bool bit14 : 1 = false;
			bool bit13 : 1 = false;
			bool bit12 : 1 = false;
			bool bit11 : 1 = false;
			bool bit10 : 1 = false;
			bool bit9 : 1 = false;
			bool bit8 : 1 = false;
			unsigned int source_colour_data_0 : 4;
			unsigned int source_colour_data_1 : 4;
		};

		__BIND_ADDRESS(0xFFFF804C, font_colour, volatile FontColour);

		__BIND_ADDRESS(0xFFFF804E, font_bit, volatile unsigned short);
		__BIND_ADDRESS(0xFFFF8050, font_data, std::array<volatile unsigned short, 4>);

		struct StampDataSize
		{
			bool graphics_operation_in_progress : 1;
			bool bit14 : 1 = false;
			bool bit13 : 1 = false;
			bool bit12 : 1 = false;
			bool bit11 : 1 = false;
			bool bit10 : 1 = false;
			bool bit9 : 1 = false;
			bool bit8 : 1 = false;
			bool bit7 : 1 = false;
			bool bit6 : 1 = false;
			bool bit5 : 1 = false;
			bool bit4 : 1 = false;
			bool bit3 : 1 = false;
			unsigned int stamp_map_size : 1; // TODO: Enum.
			unsigned int stamp_size : 1; // TODO: Enum.
			bool repeat : 1;
		};

		__BIND_ADDRESS(0xFFFF8058, stamp_data_size, volatile StampDataSize);

		// TODO: This has various restrictions based on the current mode. Maybe make a function for doing all settings at once with asserts?
		__BIND_ADDRESS(0xFFFF805A, stamp_map_base_address, volatile unsigned short);
		__BIND_ADDRESS(0xFFFF805C, image_buffer_vertical_cell_size, volatile unsigned short);
		__BIND_ADDRESS(0xFFFF805E, image_buffer_start_address, volatile unsigned short);

		struct ImageBufferOffset
		{
			bool bit15 : 1 = false;
			bool bit14 : 1 = false;
			bool bit13 : 1 = false;
			bool bit12 : 1 = false;
			bool bit11 : 1 = false;
			bool bit10 : 1 = false;
			bool bit9 : 1 = false;
			bool bit8 : 1 = false;
			bool bit7 : 1 = false;
			bool bit6 : 1 = false;
			unsigned int y : 3;
			unsigned int x : 3;
		};

		__BIND_ADDRESS(0xFFFF8060, image_buffer_offset, volatile ImageBufferOffset);
		__BIND_ADDRESS(0xFFFF8062, image_buffer_horizontal_dot_size, volatile unsigned short);
		__BIND_ADDRESS(0xFFFF8064, image_buffer_vertical_dot_size, volatile unsigned short);
		__BIND_ADDRESS(0xFFFF8066, trace_vector_base_address, volatile unsigned short);

		struct SubcodeAddress
		{
			bool bit15 : 1 = false;
			bool bit14 : 1 = false;
			bool bit13 : 1 = false;
			bool bit12 : 1 = false;
			bool bit11 : 1 = false;
			bool bit10 : 1 = false;
			bool bit9 : 1 = false;
			bool bit8 : 1 = false;
			bool subcode_address_overrun : 1;
			unsigned int subcode_top_address : 6;
			bool bit0 : 1 = false;
		};

		__BIND_ADDRESS(0xFFFF8068, subcode_address, volatile SubcodeAddress);

		__BIND_ADDRESS(0xFFFF8100, subcode_buffer, std::array<volatile unsigned short, 0x40>);
		__BIND_ADDRESS(0xFFFF8180, subcode_buffer_image, std::array<volatile unsigned short, 0x40>);

		namespace PCM
		{
			__BIND_ADDRESS(0xFFFF2000, ram_window, std::array<std::atomic<unsigned short>, 0x1000>);
		}

		namespace BIOS
		{
			// TODO: Document everything here.

			#define CLOWNMDSDK_READ_REGISTER(REGISTER, TYPE, OUTPUT) \
			[]() -> TYPE \
			{ \
				register TYPE reg asm(REGISTER); \
				asm("" : OUTPUT (reg)); \
				return reg; \
			}()

			#define CLOWNMDSDK_READ_DATA_REGISTER(REGISTER, TYPE) CLOWNMDSDK_READ_REGISTER(REGISTER, TYPE, "=d")
			#define CLOWNMDSDK_READ_ADDRESS_REGISTER(REGISTER, TYPE) CLOWNMDSDK_READ_REGISTER(REGISTER, TYPE, "=a")

			#define CLOWNMDSDK_WRITE_DATA_REGISTER(REGISTER, VALUE) \
			do \
			{ \
				register const auto reg asm(REGISTER) = VALUE; \
				asm("" : : "d" (reg)); \
			} while(0)

			#define CLOWNMDSDK_WRITE_INPUT_ADDRESS_REGISTER(REGISTER, VALUE) \
			do \
			{ \
				register const auto reg asm(REGISTER) = &VALUE; \
				asm("" : : "a" (reg), "m" (*reg)); \
			} while(0)

			#define CLOWNMDSDK_WRITE_OUTPUT_ADDRESS_REGISTER(REGISTER, VALUE) \
			do \
			{ \
				register const auto reg asm(REGISTER) = VALUE; \
				asm("" : : "a" (reg)); \
			} while(0)

			static inline bool Call(const unsigned short code)
			{
				register auto d0 asm("d0") = code;
				asm volatile(
					"jsr	(0x5F22).w"
					: "+d" (d0)
					: 
					: "cc", "d1", "a0", "a1" // According to 'Mega-CD BIOS Manual', these are the only registers that get clobbered.
				);

				// If the result is unused, this code will be stripped-out by the compiler.
				bool result;
				asm("scc.b	%0" : "=dm" (result) : "d" (d0));
				return result;
			}

			namespace Drive
			{
				struct InitialiseParameters
				{
					unsigned char first_track, last_track;
				};

				static inline void Initialise(const InitialiseParameters &parameters)
				{
					CLOWNMDSDK_WRITE_INPUT_ADDRESS_REGISTER("a0", parameters);
					Call(0x10);
				}

				static inline void Open()
				{
					Call(0xA);
				}
			}

			namespace Music
			{
				static inline void Stop()
				{
					Call(2);
				}

				static inline void Play(const unsigned short &track_number)
				{
					CLOWNMDSDK_WRITE_INPUT_ADDRESS_REGISTER("a0", track_number);
					Call(0x11);
				}

				static inline void PlayOnce(const unsigned short &track_number)
				{
					CLOWNMDSDK_WRITE_INPUT_ADDRESS_REGISTER("a0", track_number);
					Call(0x12);
				}

				static inline void PlayRepeat(const unsigned short &track_number)
				{
					CLOWNMDSDK_WRITE_INPUT_ADDRESS_REGISTER("a0", track_number);
					Call(0x13);
				}

				static inline void PlayTime(const unsigned long &time)
				{
					CLOWNMDSDK_WRITE_INPUT_ADDRESS_REGISTER("a0", time);
					Call(0x14);
				}

				static inline void Seek(const unsigned long &track_number)
				{
					CLOWNMDSDK_WRITE_INPUT_ADDRESS_REGISTER("a0", track_number);
					Call(0x15);
				}

				static inline void SeekOnce(const unsigned long &track_number)
				{
					CLOWNMDSDK_WRITE_INPUT_ADDRESS_REGISTER("a0", track_number);
					Call(0x19);
				}

				static inline void SeekTime(const unsigned long &time)
				{
					CLOWNMDSDK_WRITE_INPUT_ADDRESS_REGISTER("a0", time);
					Call(0x16);
				}

				static inline void PauseOn()
				{
					Call(3);
				}

				static inline void PauseOff()
				{
					Call(4);
				}

				static inline void ScanFastForward()
				{
					Call(5);
				}

				static inline void ScanFastReverse()
				{
					Call(6);
				}

				static inline void ScanOff()
				{
					Call(7);
				}
			}

			namespace CDROM
			{
				static inline void Read(const unsigned long &sector_number)
				{
					CLOWNMDSDK_WRITE_INPUT_ADDRESS_REGISTER("a0", sector_number);
					Call(0x17);
				}

				struct ReadNParameters
				{
					unsigned long first_sector_number, total_sectors;
				};

				static inline void ReadN(const ReadNParameters &parameters)
				{
					CLOWNMDSDK_WRITE_INPUT_ADDRESS_REGISTER("a0", parameters);
					Call(0x20);
				}

				struct ReadEParameters
				{
					unsigned long first_sector_number, last_sector_number;
				};

				static inline void ReadE(const ReadEParameters &parameters)
				{
					CLOWNMDSDK_WRITE_INPUT_ADDRESS_REGISTER("a0", parameters);
					Call(0x21);
				}

				static inline void Seek(const unsigned long &sector_number)
				{
					CLOWNMDSDK_WRITE_INPUT_ADDRESS_REGISTER("a0", sector_number);
					Call(0x18);
				}

				static inline void PauseOn()
				{
					Call(8);
				}

				static inline void PauseOff()
				{
					Call(9);
				}
			}

			namespace Misc
			{
				static inline bool WasCommandExecuted()
				{
					return Call(0x80);
				}

				struct StatusTable
				{
					unsigned short bios_status;
					unsigned short led;
					std::array<unsigned char, 20> cdd_status;
					unsigned long volume;
					unsigned long header;
				};

				static inline const StatusTable& Status()
				{
					Call(0x81);
					return *CLOWNMDSDK_READ_ADDRESS_REGISTER("a0", const StatusTable*);
				}

				struct TableOfContentsEntry
				{
					unsigned char minutes, seconds, frames;
					bool is_rom_track;
				};

				static inline TableOfContentsEntry ReadTableOfContents(const unsigned short track_number)
				{
					CLOWNMDSDK_WRITE_DATA_REGISTER("d1", track_number);
					Call(0x83);
					return {CLOWNMDSDK_READ_DATA_REGISTER("d0", unsigned long), CLOWNMDSDK_READ_DATA_REGISTER("d1", unsigned char)};
				}

				static inline void WriteTableOfContents(const unsigned long* const track_number)
				{
					// This hack is necessary so that GCC knows that we use the array that 'track_number' points to.
					// https://stackoverflow.com/questions/56432259/how-can-i-indicate-that-the-memory-pointed-to-by-an-inline-asm-argument-may-be
					CLOWNMDSDK_WRITE_INPUT_ADDRESS_REGISTER("a0", *reinterpret_cast<const unsigned long(*)[]>(track_number));
					Call(0x82);
				}

				static inline void Pause(const unsigned short delay)
				{
					CLOWNMDSDK_WRITE_DATA_REGISTER("d1", delay);
					Call(0x84);
				}
			}

			namespace Fader
			{
				static inline void Set(const unsigned short volume)
				{
					CLOWNMDSDK_WRITE_DATA_REGISTER("d1", volume);
					Call(0x85);
				}

				static inline void Change(const unsigned long volume_and_rate)
				{
					CLOWNMDSDK_WRITE_DATA_REGISTER("d1", volume_and_rate);
					Call(0x86);
				}
			}

			namespace CDC
			{
				static inline void Start()
				{
					Call(0x87);
				}

				static inline void Stop()
				{
					Call(0x89);
				}

				static inline bool SectorsAvailableForReading()
				{
					return Call(0x8A);
				}

				// Returns a BCD timecode.
				// TODO: Make a BCD class?
				static inline std::optional<unsigned long> Read()
				{
					if (!Call(0x8B))
						return std::nullopt;

					return CLOWNMDSDK_READ_DATA_REGISTER("d0", unsigned long);
				}

				static inline bool Transfer(void *&data_buffer, void *&header_buffer)
				{
					CLOWNMDSDK_WRITE_OUTPUT_ADDRESS_REGISTER("a0", data_buffer);
					CLOWNMDSDK_WRITE_OUTPUT_ADDRESS_REGISTER("a1", header_buffer);
					const bool success = Call(0x8C);
					data_buffer = CLOWNMDSDK_READ_ADDRESS_REGISTER("a0", void*);
					header_buffer = CLOWNMDSDK_READ_ADDRESS_REGISTER("a1", void*);
					return success;
				}

				static inline bool Acknowledge()
				{
					return Call(0x8D);
				}

				static inline bool SetMode(const unsigned short mode) // TODO: Enum for the mode.
				{
					CLOWNMDSDK_WRITE_DATA_REGISTER("d1", mode);
					return Call(0x96);
				}
			}

			// TODO: Subcodes, LEDs, and BuRAM.
		}
	}
}

#endif /* defined(__cplusplus) && __cplusplus >= 202302L */

#endif /* CLOWNSDK_MD_HEADER_GUARD */
