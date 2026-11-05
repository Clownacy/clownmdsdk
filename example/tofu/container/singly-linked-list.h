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

private:
	T *head = nullptr;

public:
	constexpr auto& front(this auto &&self)
	{
		_assertm(self.head != nullptr, "Tried to access first element of empty list.");
		return *self.head;
	}

	constexpr Iterator begin()
	{
		return Iterator(head);
	}

	constexpr Iterator end()
	{
		return Iterator();
	}

	constexpr ConstIterator begin() const
	{
		return ConstIterator(head);
	}

	constexpr ConstIterator end() const
	{
		return ConstIterator();
	}

	constexpr ConstIterator cbegin() const
	{
		return begin();
	}

	constexpr ConstIterator cend() const
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
template<typename T>
class SinglyLinkedListIterator : public ListIterator<T, T>
{
public:
	constexpr T& Dereference()
	{
		return *this->GetHeldPointer();
	}

	constexpr T* Advance()
	{
		return this->GetHeldPointer()->next;
	}
};

template<typename T>
class SinglyLinkedList : public SinglyLinkedListLowLevel<T, SinglyLinkedListIterator<T>, SinglyLinkedListIterator<const T>>
{
public:
	static constexpr SinglyLinkedListEntry<T>& GetEntryFromHeldType(T &held)
	{
		return held;
	}
};

#endif /* SINGLY_LINKED_LIST_H */
