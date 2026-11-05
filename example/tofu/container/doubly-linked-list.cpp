#include "doubly-linked-list.h"

#include <array>

// Test suite

class TestEntry : public DoublyLinkedList<TestEntry>::Entry
{
public:
	int value;

	constexpr TestEntry(const int value)
		: value(value)
	{}
};

static constexpr auto MakeList = [](auto &entries) constexpr
{
	DoublyLinkedList<TestEntry> list;
	for (auto &entry : entries)
		list.push_front(entry);
	return list;
};

// push_front
// push_back
// pop_front
// pop_back
static_assert(
	[]() constexpr
	{
		std::array<TestEntry, 4> entries = {0, 1, 2, 3};

		DoublyLinkedList<TestEntry> list;

		list.push_front(entries[0]);
		list.push_back(entries[1]);
		list.pop_front();
		list.push_front(entries[2]);
		list.push_back(entries[3]);
		list.pop_back();

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

// rbegin
// rend
// iterator
static_assert(
	[]() constexpr
	{
		std::array<TestEntry, 4> entries = {0, 1, 2, 3};
		const auto list = MakeList(entries);

		std::size_t count = 0;
		auto iterator = entries.begin();

		for (auto it = list.rbegin(); it != list.rend(); ++it)
		{
			const auto &value = *it;

			if (value.value != (iterator++)->value)
				return false;

			++count;
		}

		return count == entries.size();
	}()
);

// front
// back
static_assert(
	[]() constexpr
	{
		std::array<TestEntry, 4> entries = {0, 1, 2, 3};
		const auto list = MakeList(entries);
		return list.front().value == 3 && list.back().value == 0;
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
