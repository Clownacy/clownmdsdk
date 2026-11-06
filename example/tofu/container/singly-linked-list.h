#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"

template<typename T>
struct SinglyLinkedListEntry
{
	T *next;
};

// Low-level interface; user decides how `T` contains `SinglyLinkedListEntry<T>` by specifying own `Iterator`.
template<typename T, typename Iterator, typename ConstIterator>
class SinglyLinkedListLowLevel
{
public:
	using Entry = SinglyLinkedListEntry<T>;

	using iterator = Iterator;
	using const_iterator = ConstIterator;

protected:
	T *head = nullptr;

public:
	constexpr auto& front(this auto &&self)
	{
		_assertm(self.head != nullptr, "Tried to access first element of empty list.");
		return *self.head;
	}

	constexpr iterator begin()
	{
		return iterator(head);
	}

	constexpr iterator end()
	{
		return iterator();
	}

	constexpr const_iterator begin() const
	{
		return const_iterator(head);
	}

	constexpr const_iterator end() const
	{
		return const_iterator();
	}

	constexpr const_iterator cbegin() const
	{
		return begin();
	}

	constexpr const_iterator cend() const
	{
		return end();
	}

	constexpr void push_front(this auto &&self, T &entry)
	{
		self.GetEntryFromHeldType(entry).next = self.head;
		self.head = &entry;
	}

	constexpr T& pop_front(this auto &&self)
	{
		T &entry = self.front();
		self.head = self.GetEntryFromHeldType(entry).next;
		return entry;
	}

	constexpr bool empty() const
	{
		return head == nullptr;
	}

	constexpr void clear(this auto &&self)
	{
		while (!self.empty())
			self.pop_front();
	}
};

// High-level interface; `T` must derive from `SinglyLinkedListEntry<T>`.
template<typename Base>
class SinglyLinkedListIteratorBase : public Base
{
public:
	using Base::Base;

	constexpr auto& Dereference()
	{
		return *this->GetHeldPointer();
	}
};

template<typename Base>
class SinglyLinkedListIterator : public SinglyLinkedListIteratorBase<Base>
{
public:
	constexpr auto* Advance()
	{
		return this->GetHeldPointer()->next;
	}
};

template<typename T>
class SinglyLinkedList : public SinglyLinkedListLowLevel<T, SinglyLinkedListIterator<ListIterator<T, T>>, SinglyLinkedListIterator<ListConstIterator<T, T>>>
{
public:
	static constexpr SinglyLinkedListEntry<T>& GetEntryFromHeldType(T &held)
	{
		return held;
	}
};

#endif /* SINGLY_LINKED_LIST_H */
