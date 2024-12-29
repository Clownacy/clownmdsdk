#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cstddef>

#include <clownmdsdk.h>

template<std::size_t total_joypads>
class JoypadManager
{
	public:
		struct Joypad
		{
			ClownMDSDK::MainCPU::Z80::Bus::Joypad3Button held;
			ClownMDSDK::MainCPU::Z80::Bus::Joypad3Button pressed;
		};

	private:
		std::array<Joypad, total_joypads> joypads;

	public:
		JoypadManager()
		{
			ClownMDSDK::MainCPU::Z80::Bus z80_bus;

			for (unsigned int i = 0; i < joypads.size(); ++i)
				z80_bus.InitialiseIOPortAsJoypad3Button(i);
		}

		void Update()
		{
			const auto joypad_data = []()
			{
				std::array<ClownMDSDK::MainCPU::Z80::Bus::Joypad3Button, total_joypads> data;

				ClownMDSDK::MainCPU::Z80::Bus z80_bus;

				for (unsigned int i = 0; i < data.size(); ++i)
					data[i] = z80_bus.ReadIOPortAsJoypad3Button(i);

				return data;
			}();

			for (unsigned int i = 0; i < joypads.size(); ++i)
			{
				const unsigned char new_held = std::bit_cast<unsigned char>(joypad_data[i]);
				const unsigned char old_held = std::bit_cast<unsigned char>(joypads[i].held);
				const unsigned char new_pressed = new_held & ~old_held;

				joypads[i].held = std::bit_cast<ClownMDSDK::MainCPU::Z80::Bus::Joypad3Button>(new_held);
				joypads[i].pressed = std::bit_cast<ClownMDSDK::MainCPU::Z80::Bus::Joypad3Button>(new_pressed);
			}
		}

		const auto& GetJoypads()
		{
			return joypads;
		}
};

#endif // CONTROLLER_H
