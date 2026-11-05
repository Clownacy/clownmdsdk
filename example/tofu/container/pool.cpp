#include "pool.h"

#include <array>
#include <initializer_list>

// Test suite

template<std::size_t S>
static constexpr auto ToPool(const std::initializer_list<int> &entries)
{
	Pool<int, S> pool;
	for (auto &entry : entries)
		pool.allocate_front(entry);
	return pool;
};

// allocate_front
// allocate_back
// deallocate
// begin
// rbegin
static_assert(
	[]() constexpr
	{
		std::array<int, 4> entries = {0, 1, 2, 3};

		Pool<int, 10> pool;

		pool.allocate_front(entries[0]);
		pool.allocate_back(entries[1]);
		pool.deallocate(pool.begin());
		pool.allocate_front(entries[2]);
		pool.allocate_back(entries[3]);
		pool.deallocate(pool.rbegin());

		if (pool.front() != 2)
			return false;

		if (pool.back() != 1)
			return false;

		return true;
	}()
);

// begin
// end
// iterator
// size
// max_size
static_assert(
	[]() constexpr
	{
		const auto entries = {0, 1, 2, 3};
		const auto pool = ToPool<10>(entries);

		const auto &const_list = pool;
		std::size_t count = 0;
		auto iterator = std::rbegin(entries);

		for (const auto &value : const_list)
		{
			if (value != *iterator++)
				return false;

			++count;
		}

		return count == entries.size() && count == const_list.size() && const_list.max_size() == 10;
	}()
);

// rbegin
// rend
// iterator
// size
// max_size
static_assert(
	[]() constexpr
	{
		const auto entries = {0, 1, 2, 3};
		const auto pool = ToPool<10>(entries);

		const auto &const_list = pool;
		std::size_t count = 0;
		auto iterator = entries.begin();

		for (auto it = const_list.rbegin(); it != const_list.rend(); ++it)
		{
			const auto &value = *it;

			if (value != *iterator++)
				return false;

			++count;
		}

		return count == entries.size() && count == const_list.size() && const_list.max_size() == 10;
	}()
);

// front
// back
static_assert(
	[]() constexpr
	{
		const auto pool = ToPool<10>({0, 1, 2, 3});
		return pool.front() == 3 && pool.back() == 0;
	}()
);

// empty
// full
static_assert(
	[]() constexpr
	{
		const auto pool = ToPool<10>({0, 1, 2, 3});
		return !pool.empty() && !pool.full();
	}()
);
static_assert(
	[]() constexpr
	{
		const auto pool = ToPool<4>({0, 1, 2, 3});
		return !pool.empty() && pool.full();
	}()
);
static_assert(
	[]() constexpr
	{
		const Pool<int, 10> pool;
		return pool.empty() && !pool.full();
	}()
);
static_assert(
	[]() constexpr
	{
		const Pool<int, 0> pool;
		return pool.empty() && pool.full();
	}()
);
