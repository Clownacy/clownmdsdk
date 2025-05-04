#ifndef OBJECTS_BULLET_H
#define OBJECTS_BULLET_H

#include "base.h"

namespace Objects
{
	class Bullet : public Base
	{
	protected:
		long x_velocity;
		
	public:
		Bullet(const Coordinate::World &position, const long x_velocity)
			: Base(position)
			, x_velocity(x_velocity)
		{}
		void Update();
	};
}

#endif // OBJECTS_BULLET_H
