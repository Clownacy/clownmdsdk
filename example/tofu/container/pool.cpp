#include "pool.h"

#include <array>

// Test suite

class TestEntry
{
public:
	int value;

	constexpr TestEntry(const int value)
		: value(value)
	{}
};

template<std::size_t S>
static constexpr auto MakeList(const auto &entries)
{
	Pool<TestEntry, S> list;
	for (auto &entry : entries)
		list.allocate_front(entry);
	return list;
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

		Pool<TestEntry, 10> list;

		list.allocate_front(entries[0]);
		list.allocate_back(entries[1]);
		list.deallocate(list.begin());
		list.allocate_front(entries[2]);
		list.allocate_back(entries[3]);
		list.deallocate(list.rbegin());

		if (list.front().value != 2)
			return false;

		if (list.back().value != 1)
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
		std::array<int, 4> entries = {0, 1, 2, 3};
		const auto list = MakeList<10>(entries);

		const auto &const_list = list;
		std::size_t count = 0;
		auto iterator = entries.rbegin();

		for (const auto &value : const_list)
		{
			if (value.value != *iterator++)
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
		std::array<int, 4> entries = {0, 1, 2, 3};
		const auto list = MakeList<10>(entries);

		const auto &const_list = list;
		std::size_t count = 0;
		auto iterator = entries.begin();

		for (auto it = const_list.rbegin(); it != const_list.rend(); ++it)
		{
			const auto &value = *it;

			if (value.value != *iterator++)
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
		std::array<int, 4> entries = {0, 1, 2, 3};
		const auto list = MakeList<10>(entries);
		return list.front().value == 3 && list.back().value == 0;
	}()
);

// empty
// full
static_assert(
	[]() constexpr
	{
		std::array<int, 4> entries = {0, 1, 2, 3};
		const auto list = MakeList<10>(entries);
		return !list.empty() && !list.full();
	}()
);
static_assert(
	[]() constexpr
	{
		std::array<int, 4> entries = {0, 1, 2, 3};
		const auto list = MakeList<4>(entries);
		return !list.empty() && list.full();
	}()
);
static_assert(
	[]() constexpr
	{
		const Pool<TestEntry, 10> list;
		return list.empty() && !list.full();
	}()
);
static_assert(
	[]() constexpr
	{
		const Pool<TestEntry, 0> list;
		return list.empty() && list.full();
	}()
);
