#include "doubly-linked-list.h"

#include <array>
#include <iterator>
#include <span>

// Test suite

class TestEntry : public DoublyLinkedList<TestEntry>::Entry
{
public:
	int value;

	constexpr TestEntry(const int value)
		: value(value)
	{}
};

static constexpr auto MakeList(const std::span<TestEntry> &entries)
{
	DoublyLinkedList<TestEntry> list;
	for (auto &entry : entries)
		list.push_back(entry);
	return list;
};

// Iterators
static_assert(std::bidirectional_iterator<DoublyLinkedList<TestEntry>::iterator>);
static_assert(std::bidirectional_iterator<DoublyLinkedList<TestEntry>::const_iterator>);
static_assert(std::bidirectional_iterator<DoublyLinkedList<TestEntry>::reverse_iterator>);
static_assert(std::bidirectional_iterator<DoublyLinkedList<TestEntry>::const_reverse_iterator>);

// push_front
// push_back
// pop_front
// pop_back
static_assert(
	[]() constexpr
	{
		auto entries = std::to_array<TestEntry>({0, 1, 2, 3});

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
		auto entries = std::to_array<TestEntry>({0, 1, 2, 3});
		// There is some kind of god-awful bug in GCC right now that causes the move constructor
		// to not work here, so avoid it by using the move assignment operator instead.
		DoublyLinkedList<TestEntry> list; list = MakeList(entries);

		std::size_t count = 0;
		auto iterator = entries.begin();

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
		auto entries = std::to_array<TestEntry>({0, 1, 2, 3});
		DoublyLinkedList<TestEntry> list; list = MakeList(entries);

		std::size_t count = 0;
		auto iterator = entries.rbegin();

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
		auto entries = std::to_array<TestEntry>({0, 1, 2, 3});
		DoublyLinkedList<TestEntry> list; list = MakeList(entries);
		return list.front().value == 0 && list.back().value == 3;
	}()
);

// empty
static_assert(
	[]() constexpr
	{
		auto entries = std::to_array<TestEntry>({0, 1, 2, 3});
		DoublyLinkedList<TestEntry> list; list = MakeList(entries);
		return !list.empty();
	}()
);
static_assert(
	[]() constexpr
	{
		const DoublyLinkedList<TestEntry> list;
		return list.empty();
	}()
);

// clear
static_assert(
	[]() constexpr
	{
		auto entries = std::to_array<TestEntry>({0, 1, 2, 3});
		DoublyLinkedList<TestEntry> list; list = MakeList(entries);
		list.clear();
		return list.empty();
	}()
);

// Assorted iterator validity tests.
static_assert(
	[]() constexpr
	{
		std::array<TestEntry, 0> entries;
		DoublyLinkedList<TestEntry> list; list = MakeList(entries);
		return list.empty();
	}()
);

static_assert(
	[]() constexpr
	{
		auto entries = std::to_array<TestEntry>({0});
		DoublyLinkedList<TestEntry> list; list = MakeList(entries);
		return !list.empty();
	}()
);

static_assert(
	[]() constexpr
	{
		auto entries = std::to_array<TestEntry>({0});
		DoublyLinkedList<TestEntry> list_a;
		for (auto &entry : entries)
			list_a.push_back(entry);
		[[maybe_unused]] DoublyLinkedList<TestEntry> list_b = std::move(list_a);
		return list_a.begin() == list_a.end();
	}()
);

static_assert(
	[]() constexpr
	{
		auto entries = std::to_array<TestEntry>({0});
		DoublyLinkedList<TestEntry> list_b;
		{
			const std::span<TestEntry> entries_span = entries;
			DoublyLinkedList<TestEntry> list_a;
			for (auto &entry : entries_span)
				list_a.push_back(entry);
			list_b = std::move(list_a);
		}
		auto it = list_b.begin();
		return ++it == list_b.end();
	}()
);

static_assert(
	[]() constexpr
	{
		auto entries = std::to_array<TestEntry>({0});
		DoublyLinkedList<TestEntry> list_b;
		list_b = MakeList(entries);
		auto it = list_b.begin();
		return ++it == list_b.end();
	}()
);

static_assert(
	[]() constexpr
	{
		auto entries = std::to_array<TestEntry>({0});
		DoublyLinkedList<TestEntry> list_b; list_b = MakeList(entries);
		auto it = list_b.begin();
		return ++it == list_b.end();
	}()
);

static_assert(
	[]() constexpr
	{
		auto entries = std::to_array<TestEntry>({0});
		DoublyLinkedList<TestEntry> list; list = MakeList(entries);
		return (++list.rbegin()).base() == list.begin();
	}()
);

static_assert(
	[]() constexpr
	{
		auto entries = std::to_array<TestEntry>({0});
		DoublyLinkedList<TestEntry> list; list = MakeList(entries);
		return (++list.rend()).base() == list.end();
	}()
);

static_assert(
	[]() constexpr
	{
		std::array<TestEntry, 0> entries;
		DoublyLinkedList<TestEntry> list; list = MakeList(entries);
		auto it = list.begin();
		return ++it == list.end();
	}()
);

static_assert(
	[]() constexpr
	{
		std::array<TestEntry, 0> entries;
		DoublyLinkedList<TestEntry> list; list = MakeList(entries);
		return list.begin() == list.end();
	}()
);

static_assert(
	[]() constexpr
	{
		std::array<TestEntry, 0> entries;
		DoublyLinkedList<TestEntry> list; list = MakeList(entries);
		return list.rbegin() == list.rend();
	}()
);

static_assert(
	[]() constexpr
	{
		auto entries = std::to_array<TestEntry>({0});
		DoublyLinkedList<TestEntry> list; list = MakeList(entries);
		auto it = list.begin();
		return ++it == list.end();
	}()
);

static_assert(
	[]() constexpr
	{
		auto entries = std::to_array<TestEntry>({0});
		DoublyLinkedList<TestEntry> list; list = MakeList(entries);
		auto it = list.rbegin();
		return ++it == list.rend();
	}()
);

static_assert(
	[]() constexpr
	{
		auto entries = std::to_array<TestEntry>({0, 1});
		DoublyLinkedList<TestEntry> list; list = MakeList(entries);
		list.pop_front();
		return list.size() == entries.size() - 1;
	}()
);
