#ifndef OBJECTS_H
#define OBJECTS_H

#include <array>
#include <variant>

#include <clownmdsdk.h>

#include "pool.h"
#include "objects/player.h"

using namespace ClownMDSDK::MainCPU;

namespace Objects
{
	class PoolEntry : public std::variant<Player>, public Pool<PoolEntry>::Entry {};

	extern std::array<PoolEntry, 0x10> pool_buffer;
	extern Pool<PoolEntry> pool;

	template<typename T>
	T& AllocateFront()
	{
		return pool.allocate_front().emplace<T>();
	}

	template<typename T>
	T& AllocateBack()
	{
		return pool.allocate_back().emplace<T>();
	}

	void Update();
}

#endif // OBJECTS_H
