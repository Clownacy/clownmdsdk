#ifndef OBJECTS_PLAYER_H
#define OBJECTS_PLAYER_H

#include "base.h"

namespace Objects
{
	class Player : public Base
	{
	public:
		bool facing_left, on_ground;
		long y_velocity;

		using Base::Base;
		void Update();
	};
}

#endif // OBJECTS_PLAYER_H
