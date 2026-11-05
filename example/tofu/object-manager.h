#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <variant>

#include "container/pool.h"
#include "objects/bullet.h"
#include "objects/player.h"

class ObjectManager
{
private:
	Pool<std::variant<Objects::Bullet, Objects::Player>, 0x10> pool;

public:
	template<typename T, typename... Args>
	T* EmplaceFront(Args &&...args)
	{
		if (pool.full())
			return nullptr;

		auto it = pool.allocate_front(std::in_place_type<T>, std::forward<Args>(args)...);
		return std::get_if<T>(&*it);
	}

	template<typename T, typename... Args>
	T* EmplaceBack(Args &&...args)
	{
		if (pool.full())
			return nullptr;

		auto it = pool.allocate_back(std::in_place_type<T>, std::forward<Args>(args)...);
		return std::get_if<T>(&*it);
	}

	void Update();
};

extern ObjectManager object_manager;

#endif // OBJECT_MANAGER_H
