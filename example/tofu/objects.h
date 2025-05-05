#ifndef OBJECTS_H
#define OBJECTS_H

#include <array>
#include <variant>

#include "objects/bullet.h"
#include "objects/player.h"
#include "pool.h"

namespace Objects
{
	class PoolEntry : public std::variant<std::monostate, Bullet, Player>, public Pool<PoolEntry>::Entry {};

	extern std::array<PoolEntry, 0x10> pool_buffer;
	extern Pool<PoolEntry> pool;

	template<typename T, typename... Args>
	T* EmplaceFront(Args &&...args)
	{
		if (pool.full())
			return nullptr;

		return &pool.allocate_front().emplace<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	T* EmplaceBack(Args &&...args)
	{
		if (pool.full())
			return nullptr;

		return &pool.allocate_back().emplace<T>(std::forward<Args>(args)...);
	}

	void Update();
}

#endif // OBJECTS_H
