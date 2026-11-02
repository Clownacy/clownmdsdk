#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <array>
#include <variant>

#include "container/pool.h"
#include "objects/bullet.h"
#include "objects/player.h"

class ObjectManager
{
private:
	using Pool = ::Pool<std::variant<Objects::Bullet, Objects::Player>, 0x10>;

	Pool pool;

public:
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
};

extern ObjectManager object_manager;

#endif // OBJECT_MANAGER_H
