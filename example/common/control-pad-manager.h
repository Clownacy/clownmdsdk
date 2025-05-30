#ifndef CONTROL_PAD_MANAGER_H
#define CONTROL_PAD_MANAGER_H

#include <cstddef>

#include <clownmdsdk.h>

template<std::size_t total_control_pads>
class ControlPadManager
{
	public:
		struct ControlPad
		{
			ClownMDSDK::MainCPU::Z80::Bus::ControlPad3Button held;
			ClownMDSDK::MainCPU::Z80::Bus::ControlPad3Button pressed;
		};

	private:
		std::array<ControlPad, total_control_pads> control_pads;

	public:
		ControlPadManager()
		{
			ClownMDSDK::MainCPU::Z80::Bus::Lock(
				[&](auto &z80_bus)
				{
					for (unsigned int i = 0; i < std::size(control_pads); ++i)
						z80_bus.InitialiseIOPortAsControlPad3Button(i);
				}
			);
		}

		void Update()
		{
			const auto control_pad_data = []()
			{
				std::array<ClownMDSDK::MainCPU::Z80::Bus::ControlPad3Button, total_control_pads> data;

				ClownMDSDK::MainCPU::Z80::Bus::Lock(
					[&](auto &z80_bus)
					{
						for (unsigned int i = 0; i < std::size(data); ++i)
							data[i] = z80_bus.ReadIOPortAsControlPad3Button(i);
					}
				);

				return data;
			}();

			for (unsigned int i = 0; i < std::size(control_pads); ++i)
			{
				const unsigned char new_held = std::bit_cast<unsigned char>(control_pad_data[i]);
				const unsigned char old_held = std::bit_cast<unsigned char>(control_pads[i].held);
				const unsigned char new_pressed = new_held & ~old_held;

				control_pads[i].held = std::bit_cast<ClownMDSDK::MainCPU::Z80::Bus::ControlPad3Button>(new_held);
				control_pads[i].pressed = std::bit_cast<ClownMDSDK::MainCPU::Z80::Bus::ControlPad3Button>(new_pressed);
			}
		}

		const auto& GetControlPads()
		{
			return control_pads;
		}

		const auto& GetControlPad(const std::size_t index)
		{
			return control_pads[index];
		}
};

#endif // CONTROL_PAD_MANAGER_H
