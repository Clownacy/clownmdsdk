#ifndef HV_TEST_H
#define HV_TEST_H

#include <array>
#include <atomic>

#include "mode.h"

class HVTest
{
private:
	struct Values
	{
		unsigned char v_counter;
		bool h_blank;
	};

	unsigned int state;
	std::atomic<bool> do_sample = false;
	unsigned int line = 0;
	std::array<Values, 0x200> values;

public:
	HVTest(unsigned int state);
	~HVTest();
	ModeID Update();
	void HorizontalInterrupt();
	void VerticalInterrupt();
};

#endif // HV_TEST_H
