#ifndef OBJECTS_BULLET_H
#define OBJECTS_BULLET_H

#include "base.h"

namespace Objects
{
	class Bullet : public Base
	{
	protected:
		long x_velocity;
		unsigned int life = 60 * 1;
		
	public:
		Bullet(const Coordinate::World &position, const long x_velocity)
			: Base(position)
			, x_velocity(x_velocity)
		{}
		bool Update();
	};
}

#endif // OBJECTS_BULLET_H
