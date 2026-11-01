#ifndef OBJECTS_H
#define OBJECTS_H

#include <array>
#include <variant>

#include "objects/bullet.h"
#include "objects/player.h"
#include "pool.h"

namespace Objects
{
	using Pool = ::Pool<std::variant<Bullet, Player>, 0x10>;

	extern Pool pool;

	template<typename T, typename... Args>
	T* EmplaceFront(Args &&...args)
	{
		if (pool.full())
			return nullptr;

		auto &entry = pool.allocate_front(std::in_place_type<T>, std::forward<Args>(args)...);
		return std::get_if<T>(&entry);
	}

	template<typename T, typename... Args>
	T* EmplaceBack(Args &&...args)
	{
		if (pool.full())
			return nullptr;

		auto &entry = pool.allocate_back(std::in_place_type<T>, std::forward<Args>(args)...);
		return std::get_if<T>(&entry);
	}

	void Update();
}

#endif // OBJECTS_H
