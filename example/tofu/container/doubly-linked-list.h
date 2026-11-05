#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"

template<typename Derived, typename T>
class DoublyLinkedListLowLevel : protected ListCommon<Derived, T>
{
private:
	using Common = ListCommon<Derived, T>;

public:
	struct Entry
	{
		T *next;
		T *previous;
	};

private:
	T *head = nullptr;
	T *tail = nullptr;
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

	constexpr void push_front(T &value)
	{
		++total_entries;

		Entry &entry = Derived::GetEntryFromHeldType(value);

		if (head == nullptr)
			tail = &value;
		else
			Derived::GetEntryFromHeldType(*head).previous = &value;

		entry.next = head;
		entry.previous = nullptr;

		head = &value;
	}

	constexpr void push_back(T &value)
	{
		++total_entries;

		Entry &entry = Derived::GetEntryFromHeldType(value);

		if (tail == nullptr)
			head = &value;
		else
			Derived::GetEntryFromHeldType(*tail).next = &value;

		entry.next = nullptr;
		entry.previous = tail;

		tail = &value;
	}

	constexpr T* erase(T &value)
	{
		--total_entries;

		Entry &entry = Derived::GetEntryFromHeldType(value);

		T* const next = entry.next;

		if (entry.previous == nullptr)
			head = entry.next;
		else
			Derived::GetEntryFromHeldType(*entry.previous).next = entry.next;

		if (entry.next == nullptr)
			tail = entry.previous;
		else
			Derived::GetEntryFromHeldType(*entry.next).previous = entry.previous;

		return next;
	}

	constexpr T& pop_front()
	{
		auto &entry = front();
		erase(entry);
		return entry;
	}

	constexpr T& pop_back()
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

template<typename T>
class DoublyLinkedList;

template<typename T>
using DoublyLinkedListBase = DoublyLinkedListLowLevel<DoublyLinkedList<T>, T>;

template<typename T>
class DoublyLinkedList : public DoublyLinkedListBase<T>
{
public:
	using Base = DoublyLinkedListBase<T>;

	static constexpr Base::Entry& GetEntryFromHeldType(T &held)
	{
		return held;
	}

	static constexpr T& DereferenceHeldType(T &held)
	{
		return held;
	}
};

#endif /* DOUBLY_LINKED_LIST_H */
