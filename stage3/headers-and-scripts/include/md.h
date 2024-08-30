// Copyright (c) 2024 Clownacy

// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted.

// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#ifndef CLOWNSDK_MD_HEADER_GUARD
#define CLOWNSDK_MD_HEADER_GUARD

#include <bit>
#include <cassert>
#include <cstdint>
#include <limits>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>

// TODO: Is there any point in supporting C here?
#ifdef __cplusplus
#define VISIBILITY extern "C"
#else
#define VISIBILITY
#endif

#define ENTRY_POINT VISIBILITY [[noreturn]]
#define INTERRUPT_HANDLER VISIBILITY __attribute__((interrupt))

ENTRY_POINT void EntryPoint();
INTERRUPT_HANDLER void BusErrorHandler();
INTERRUPT_HANDLER void AddressErrorHandler();
INTERRUPT_HANDLER void IllegalInstructionHandler();
INTERRUPT_HANDLER void DivisionByZeroHandler();
INTERRUPT_HANDLER void CHKHandler();
INTERRUPT_HANDLER void TRAPVHandler();
INTERRUPT_HANDLER void PrivilegeViolationHandler();
INTERRUPT_HANDLER void TraceHandler();
INTERRUPT_HANDLER void UnimplementedInstructionLineAHandler();
INTERRUPT_HANDLER void UnimplementedInstructionLineFHandler();
INTERRUPT_HANDLER void UnassignedHandler();
INTERRUPT_HANDLER void UninitialisedInterruptHandler();
INTERRUPT_HANDLER void SpuriousInterruptHandler();
INTERRUPT_HANDLER void Level1InterruptHandler();
INTERRUPT_HANDLER void Level2InterruptHandler();
INTERRUPT_HANDLER void Level3InterruptHandler();
INTERRUPT_HANDLER void Level4InterruptHandler();
INTERRUPT_HANDLER void Level5InterruptHandler();
INTERRUPT_HANDLER void Level6InterruptHandler();
INTERRUPT_HANDLER void Level7InterruptHandler();
INTERRUPT_HANDLER void TRAP0Handler();
INTERRUPT_HANDLER void TRAP1Handler();
INTERRUPT_HANDLER void TRAP2Handler();
INTERRUPT_HANDLER void TRAP3Handler();
INTERRUPT_HANDLER void TRAP4Handler();
INTERRUPT_HANDLER void TRAP5Handler();
INTERRUPT_HANDLER void TRAP6Handler();
INTERRUPT_HANDLER void TRAP7Handler();
INTERRUPT_HANDLER void TRAP8Handler();
INTERRUPT_HANDLER void TRAP9Handler();
INTERRUPT_HANDLER void TRAP10Handler();
INTERRUPT_HANDLER void TRAP11Handler();
INTERRUPT_HANDLER void TRAP12Handler();
INTERRUPT_HANDLER void TRAP13Handler();
INTERRUPT_HANDLER void TRAP14Handler();
INTERRUPT_HANDLER void TRAP15Handler();

// TODO: Move this to its own translation unit? Optimisation can be handled by LTO.
namespace MD
{
	namespace Unsafe
	{
		static volatile unsigned char &version_register = *reinterpret_cast<volatile unsigned char*>(0xA10001);

		static volatile unsigned short* const io_data = reinterpret_cast<volatile unsigned short*>(0xA10002);
		static volatile unsigned short* const io_ctrl = reinterpret_cast<volatile unsigned short*>(0xA10008);

		inline bool IsPAL()
		{
			return (version_register & 1 << 6) != 0;
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
			static volatile unsigned char* const ports = reinterpret_cast<volatile unsigned char*>(0xA04000);
			static volatile unsigned char &A0 = ports[0];
			static volatile unsigned char &D0 = ports[1];
			static volatile unsigned char &A1 = ports[2];
			static volatile unsigned char &D1 = ports[3];
		}

		static constexpr unsigned int sample_rate = M68k::clock / (6 * 6 * 4);
	}

	namespace Z80
	{
		namespace Unsafe
		{
			static volatile unsigned char* const ram = reinterpret_cast<volatile unsigned char*>(0xA00000);
			static volatile unsigned short &bus_request = *reinterpret_cast<volatile unsigned short*>(0xA11100);
			static volatile unsigned short &reset = *reinterpret_cast<volatile unsigned short*>(0xA11200);

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

		class Bus
		{
		private:
			const unsigned int interrupt_mask;

		public:
			Bus(const bool wait_for_bus = true) : interrupt_mask(M68k::DisableInterrupts())
			{
				Unsafe::RequestBus();

				if (wait_for_bus)
					Unsafe::WaitUntilBusObtained();
			}

			~Bus()
			{
				Unsafe::ReleaseBus();

				M68k::SetInterruptMask(interrupt_mask);
			}

			const std::span<volatile unsigned char, 0x2000> ram = std::span<volatile unsigned char, 0x2000>(Unsafe::ram, 0x2000);
			volatile unsigned short* const io_data = MD::Unsafe::io_data;
			volatile unsigned short* const io_ctrl = MD::Unsafe::io_ctrl;

			bool IsConsolePAL()
			{
				return MD::Unsafe::IsPAL();
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
		};
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
			bool bit2 : 1 = true;
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

		template<typename T>
		struct ValueWrapper
		{
			T value;

			constexpr ValueWrapper(const T value) : value(value) {}
		};

		struct DataValueWord : public ValueWrapper<unsigned short> {using ValueWrapper::ValueWrapper;};
		struct DataValueLongword : public ValueWrapper<unsigned long> {using ValueWrapper::ValueWrapper;};

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

		enum class CommandRAM : unsigned int
		{
			VRAM  = 0x21,
			CRAM  = 0x2B,
			VSRAM = 0x25
		};

		enum class CommandAccess : unsigned int
		{
			READ  = 0x0C,
			WRITE = 0x07,
			DMA   = 0x27
		};

		static volatile unsigned short &data_port_word = *reinterpret_cast<volatile unsigned short*>(0xC00000);
		static volatile unsigned long &data_port_longword = *reinterpret_cast<volatile unsigned long*>(0xC00000);
		static volatile unsigned short &control_port_word = *reinterpret_cast<volatile unsigned short*>(0xC00004);
		static volatile unsigned long &control_port_longword = *reinterpret_cast<volatile unsigned long*>(0xC00004);

		inline void Write(const DataValueWord value)
		{
			asm volatile(
				"move.w	%1,%0"
				: "=Qm" (MD::VDP::data_port_word)
				: "daim" (value) // TODO: Other holders?
				: "cc"
			);
		}

		inline void Write(const DataValueLongword value)
		{
			asm volatile(
				"move.l	%1,%0"
				: "=Qm" (MD::VDP::data_port_word) // I would use 'data_port_longword', but using 'data_port_word' makes the compiler share pointers with WriteDataPortWord.
				: "daim" (value) // TODO: Other holders?
				: "cc"
			);
		}

		inline void Write(const ControlValueWord value)
		{
			asm volatile(
				"move.w	%1,%0"
				: "=Qm" (MD::VDP::control_port_word)
				: "daim" (value) // TODO: Other holders?
				: "cc"
			);
		}

		inline void Write(const ControlValueLongword value)
		{
			asm volatile(
				"move.l	%1,%0"
				: "=Qm" (MD::VDP::control_port_word) // I would use 'control_port_longword', but using 'control_port_word' makes the compiler share pointers with WriteControlPortWord.
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

		inline constexpr ControlValueLongword MakeCommand(const CommandRAM ram, const CommandAccess access, const unsigned int address)
		{
			const unsigned int cd = std::to_underlying(ram) & std::to_underlying(access);
			return static_cast<unsigned long>(cd & 3) << 30 | static_cast<unsigned long>(address & 0x3FFF) << 16 | (cd & 0x3C) << 2 | (address & 0xC000) >> 14;
		}

		inline void SendCommand(const CommandRAM ram, const CommandAccess access, const unsigned int address)
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

		inline constexpr auto MakeDMACopyCommands(const CommandRAM ram, const unsigned int address, const void* const data, const unsigned int length)
		{
			// TODO: 128KiB boundary.
			const auto source_address = std::bit_cast<std::intptr_t>(data);

			return std::make_tuple(
				MakeDMALengthCommand(length),
				Register15{.dma_source_low = source_address >> (8 * 0 + 1)},
				Register16{.dma_source_middle = source_address >> (8 * 1 + 1)},
				Register17{.dma_mode = 0, .dma_source_high = source_address >> (8 * 2 + 1)},
				MakeCommand(ram, CommandAccess::DMA, address));
		}

		inline void CopyWordsWithDMA(const CommandRAM ram, const unsigned int address, const void* const data, const unsigned int length)
		{
			// TODO: Use 'always_inline', like FillBytesWithDMA.
			// TODO: 128KiB boundary.
			const auto source_address = std::bit_cast<std::intptr_t>(data);

			SetDMALength(length);
			Write(Register15{.dma_source_low = source_address >> (8 * 0 + 1)});
			Write(Register16{.dma_source_middle = source_address >> (8 * 1 + 1)});
			Write(Register17{.dma_mode = 0, .dma_source_high = source_address >> (8 * 2 + 1)});

			// According to Sega's 'GENESIS SOFTWARE MANUAL', the write
			// that triggers the DMA transfer must be from memory.
			asm volatile(
					"move.l	%1,-(%%sp)\n"
				"	move.l	(%%sp)+,%0"
				: "=Qm" (MD::VDP::control_port_word)
				: "daim" (MakeCommand(ram, CommandAccess::DMA, address)) // TODO: Other holders?
				: "cc"
			);
		}

		inline void CopyWordsWithoutDMA(const CommandRAM ram, const unsigned int address, const void* const data, const unsigned int length)
		{
			SendCommand(ram, CommandAccess::WRITE, address);

			const unsigned long *data_pointer = static_cast<const unsigned long*>(data);

			for (unsigned int i = 0; i < length / 2; ++i)
			{
				Write(DataValueLongword(*reinterpret_cast<const unsigned long*>(data_pointer)));
				++data_pointer;
			}

			if (length % 2 != 0)
				Write(DataValueWord(*reinterpret_cast<const unsigned short*>(data_pointer)));
		}

		inline void FillWordsWithoutDMA(const CommandRAM ram, const unsigned int address, const unsigned int length, const unsigned int value)
		{
			SendCommand(ram, CommandAccess::WRITE, address);

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
				: "Qm" (data_port_word), "di" (1)
				: "cc"
			);
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
			static constexpr unsigned int TILE_HEIGHT = 8;
			static constexpr unsigned int TILE_SIZE_IN_BYTES = TILE_WIDTH * TILE_HEIGHT / 2;

			inline constexpr auto MakeDMAFillCommands(const unsigned int address, const unsigned int length, const unsigned int value)
			{
				return std::make_tuple(MakeDMALengthCommand(length - 1), Register17{.dma_mode = 1, .dma_source_high = 0}, MakeCommand(CommandRAM::VRAM, CommandAccess::DMA, address), DataValueWord(RepeatBits<unsigned short, 3>(value)));
			}

			__attribute__((always_inline)) inline void FillBytesWithDMA(const unsigned int address, const unsigned int length, const unsigned int value)
			{
				// TODO: Don't use the tuple stuff: see CopyWordsWithDMA.
				Write(MakeDMAFillCommands(address, length, value));
			}

			inline void FillWordsWithoutDMA(const unsigned int address, const unsigned int length, const unsigned int value)
			{
				FillWordsWithoutDMA(CommandRAM::VRAM, address, length, value);
			}

			inline void SetPlaneALocation(const unsigned int vram_address)
			{
				constexpr unsigned int divisor = 0x2000;
				assert(vram_address % divisor == 0);
				Write(Register02{.plane_a_location = vram_address / divisor});
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

			inline unsigned int PaletteLineAndIndexToOffset(const unsigned int palette_line, const unsigned int colour_index)
			{
				return (palette_line * PALETTE_LINE_LENGTH + colour_index) * Colour::Size();
			}

			inline void Set(const unsigned int palette_line, const unsigned int colour_index, const Colour colour)
			{
				Write(
					MakeCommand(CommandRAM::CRAM, CommandAccess::WRITE, PaletteLineAndIndexToOffset(palette_line, colour_index)),
					DataValueWord(colour.GetRaw())
				);
			}

			inline void FillWordsWithoutDMA(const unsigned int address, const unsigned int length, const unsigned int value)
			{
				FillWordsWithoutDMA(CommandRAM::CRAM, address, length, value);
			}

			inline void FillWordsWithoutDMA(const unsigned int address, const unsigned int length, const Colour colour)
			{
				FillWordsWithoutDMA(CommandRAM::CRAM, address, length, colour.GetRaw());
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
				FillWordsWithoutDMA(CommandRAM::VSRAM, address, length, value);
			}
		}
	}
}

#endif /* CLOWNSDK_MD_HEADER_GUARD */
