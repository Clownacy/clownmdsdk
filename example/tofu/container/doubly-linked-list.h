#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"

template<typename HeldType, typename DerivedEntry>
class DoublyLinkedListLowLevel : protected ListCommon<DerivedEntry, HeldType>
{
private:
	using Common = ListCommon<DerivedEntry, HeldType>;

public:
	struct Entry
	{
		HeldType *next;
		HeldType *previous;
	};

private:
	HeldType *head = nullptr;
	HeldType *tail = nullptr;
	std::size_t total_entries = 0;

public:
	template<typename Self>
	constexpr auto& front(this Self &&self)
	{
		_assertm(self.head != nullptr, "Tried to access first element of empty list.");
		return *self.head;
	}

	template<typename Self>
	constexpr auto& back(this Self &&self)
	{
		_assertm(self.tail != nullptr, "Tried to access last element of empty list.");
		return *self.tail;
	}

	constexpr Common::Iterator begin()
	{
		return typename Common::Iterator(head);
	}

	constexpr Common::Iterator end()
	{
		return typename Common::Iterator();
	}

	constexpr void push_front(HeldType &entry)
	{
		++total_entries;

		if (head == nullptr)
			tail = &entry;
		else
			DerivedEntry::GetEntryFromHeldType(*head).previous = &entry;

		DerivedEntry::GetEntryFromHeldType(entry).next = head;
		DerivedEntry::GetEntryFromHeldType(entry).previous = nullptr;

		head = &entry;
	}

	constexpr void push_back(HeldType &entry)
	{
		++total_entries;

		if (tail == nullptr)
			head = &entry;
		else
			DerivedEntry::GetEntryFromHeldType(*tail).next = &entry;

		DerivedEntry::GetEntryFromHeldType(entry).next = nullptr;
		DerivedEntry::GetEntryFromHeldType(entry).previous = tail;

		tail = &entry;
	}

	constexpr HeldType* erase(HeldType &entry)
	{
		--total_entries;

		HeldType* const next = DerivedEntry::GetEntryFromHeldType(entry).next;

		if (DerivedEntry::GetEntryFromHeldType(entry).previous == nullptr)
			head = DerivedEntry::GetEntryFromHeldType(entry).next;
		else
			DerivedEntry::GetEntryFromHeldType(*DerivedEntry::GetEntryFromHeldType(entry).previous).next = DerivedEntry::GetEntryFromHeldType(entry).next;

		if (DerivedEntry::GetEntryFromHeldType(entry).next == nullptr)
			tail = DerivedEntry::GetEntryFromHeldType(entry).previous;
		else
			DerivedEntry::GetEntryFromHeldType(*DerivedEntry::GetEntryFromHeldType(entry).next).previous = DerivedEntry::GetEntryFromHeldType(entry).previous;

		return next;
	}

	constexpr HeldType& pop_front()
	{
		auto &entry = front();
		erase(entry);
		return entry;
	}

	constexpr HeldType& pop_back()
	{
		auto &entry = back();
		erase(entry);
		return entry;
	}

	constexpr std::size_t size() const
	{
		return total_entries;
	}

	constexpr bool empty() const
	{
		return head == nullptr;
	}
};

template<typename HeldType>
class DoublyLinkedList;

template<typename HeldType>
using DoublyLinkedListBase = DoublyLinkedListLowLevel<HeldType, DoublyLinkedList<HeldType>>;

template<typename HeldType>
class DoublyLinkedList : public DoublyLinkedListBase<HeldType>
{
public:
	using Base = DoublyLinkedListBase<HeldType>;

	static constexpr Base::Entry& GetEntryFromHeldType(HeldType &held)
	{
		return held;
	}

	static constexpr HeldType& DereferenceHeldType(HeldType &held)
	{
		return held;
	}
};

#endif /* DOUBLY_LINKED_LIST_H */
