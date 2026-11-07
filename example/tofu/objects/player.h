#ifndef OBJECTS_PLAYER_H
#define OBJECTS_PLAYER_H

#include "base.h"

namespace Objects
{
	class Player : public Base
	{
	private:
		void UpdateCamera();

	public:
		bool facing_left, on_ground;
		long y_velocity;

		Player(const Coordinate::World &position)
			: Base(position)
		{
			UpdateCamera();
		}

		bool Update();
	};
}

#endif // OBJECTS_PLAYER_H
