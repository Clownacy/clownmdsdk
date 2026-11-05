#include "singly-linked-list.h"

#include <array>

// Test suite

class TestEntry : public SinglyLinkedList<TestEntry>::Entry
{
public:
	int value;

	constexpr TestEntry(const int value)
		: value(value)
	{}
};

static constexpr auto MakeList = [](auto &entries) constexpr
{
	SinglyLinkedList<TestEntry> list;
	for (auto &entry : entries)
		list.push_front(entry);
	return list;
};

// push_front
// pop_front
// begin
// end
static_assert(
	[]() constexpr
	{
		std::array<TestEntry, 4> entries = {0, 1, 2, 3};

		SinglyLinkedList<TestEntry> list;

		list.push_front(entries[0]);
		list.push_front(entries[1]);
		list.pop_front();
		list.push_front(entries[2]);
		list.push_front(entries[3]);
		list.pop_front();

		if (list.front().value != 2)
			return false;

		list.pop_front();

		if (list.front().value != 0)
			return false;

		list.pop_front();

		return list.begin() == list.end();
	}()
);

// iterator
static_assert(
	[]() constexpr
	{
		std::array<TestEntry, 4> entries = {0, 1, 2, 3};
		const auto list = MakeList(entries);

		std::size_t count = 0;
		auto iterator = entries.rbegin();

		for (const auto &value : list)
		{
			if (value.value != (iterator++)->value)
				return false;

			++count;
		}

		return count == entries.size();
	}()
);

// front
static_assert(
	[]() constexpr
	{
		std::array<TestEntry, 4> entries = {0, 1, 2, 3};
		const auto list = MakeList(entries);
		return list.front().value == 3;
	}()
);

// empty
static_assert(
	[]() constexpr
	{
		std::array<TestEntry, 4> entries = {0, 1, 2, 3};
		const auto list = MakeList(entries);
		return !list.empty();
	}()
);
static_assert(
	[]() constexpr
	{
		std::array<TestEntry, 0> entries = {};
		const auto list = MakeList(entries);
		return list.empty();
	}()
);
