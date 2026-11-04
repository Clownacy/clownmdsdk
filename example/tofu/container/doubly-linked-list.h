#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"

template<typename HeldType, typename Derived>
class DoublyLinkedListLowLevel : protected ListCommon<Derived, HeldType>
{
private:
	using Common = ListCommon<Derived, HeldType>;

public:
	class Entry
	{
	public:
		HeldType *next;
		HeldType *previous;

		// TODO: Get rid of this and just use nullptr `directly`.
		static constexpr auto GetNull()
		{
			return static_cast<HeldType*>(nullptr);
		}
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
			head->previous = &entry;

		entry.next = head;
		entry.previous = nullptr;

		head = &entry;
	}

	constexpr void push_back(HeldType &entry)
	{
		++total_entries;

		if (tail == nullptr)
			head = &entry;
		else
			tail->next = &entry;

		entry.next = nullptr;
		entry.previous = tail;

		tail = &entry;
	}

	constexpr Entry* erase(HeldType &entry)
	{
		--total_entries;

		HeldType* const next = entry.next;

		if (entry.previous == nullptr)
			head = entry.next;
		else
			entry.previous->next = entry.next;

		if (entry.next == nullptr)
			tail = entry.previous;
		else
			entry.next->previous = entry.previous;

		return next;
	}

	constexpr Entry& pop_front()
	{
		auto &entry = front();
		erase(entry);
		return entry;
	}

	constexpr Entry& pop_back()
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
