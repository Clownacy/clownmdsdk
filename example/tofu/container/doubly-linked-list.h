#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"
#include "singly-linked-list.h"

template<typename T>
struct DoublyLinkedListEntry : public SinglyLinkedListEntry<T>
{
	T *previous;
};

// Low-level interface; user decides how `T` contains `DoublyLinkedListEntry<T>` by specifying own `Iterator`.
template<typename T, typename Iterator, typename ConstIterator, typename ReverseIterator, typename ConstReverseIterator>
class DoublyLinkedListLowLevel : public SinglyLinkedListLowLevel<T, Iterator, ConstIterator>
{
public:
	using Entry = DoublyLinkedListEntry<T>;

	using iterator = Iterator;
	using const_iterator = ConstIterator;
	using reverse_iterator = ReverseIterator;
	using const_reverse_iterator = ConstReverseIterator;

protected:
	T *tail = nullptr;
	std::size_t total_entries = 0;

public:
	constexpr auto& back(this auto &&self)
	{
		_assertm(self.tail != nullptr, "Tried to access last element of empty list.");
		return *self.tail;
	}

	constexpr const_reverse_iterator crbegin() const
	{
		return const_reverse_iterator(tail);
	}

	constexpr const_reverse_iterator crend() const
	{
		return const_reverse_iterator();
	}

	constexpr const_reverse_iterator rbegin() const
	{
		return crbegin();
	}

	constexpr const_reverse_iterator rend() const
	{
		return crend();
	}

	constexpr reverse_iterator rbegin()
	{
		return reverse_iterator(tail);
	}

	constexpr reverse_iterator rend()
	{
		return reverse_iterator();
	}

	constexpr void push_front(this auto &&self, T &value)
	{
		++self.total_entries;

		Entry &entry = self.GetEntryFromHeldType(value);

		if (self.head == nullptr)
			self.tail = &value;
		else
			self.GetEntryFromHeldType(*self.head).previous = &value;

		entry.next = self.head;
		entry.previous = nullptr;

		self.head = &value;
	}

	constexpr void push_back(this auto &&self, T &value)
	{
		++self.total_entries;

		Entry &entry = self.GetEntryFromHeldType(value);

		if (self.tail == nullptr)
			self.head = &value;
		else
			self.GetEntryFromHeldType(*self.tail).next = &value;

		entry.next = nullptr;
		entry.previous = self.tail;

		self.tail = &value;
	}

	constexpr T* erase(this auto &&self, T &value)
	{
		--self.total_entries;

		Entry &entry = self.GetEntryFromHeldType(value);

		T* const next = entry.next;

		if (entry.previous == nullptr)
			self.head = entry.next;
		else
			self.GetEntryFromHeldType(*entry.previous).next = entry.next;

		if (entry.next == nullptr)
			self.tail = entry.previous;
		else
			self.GetEntryFromHeldType(*entry.next).previous = entry.previous;

		return next;
	}

	constexpr T& pop_front(this auto &&self)
	{
		T &entry = self.front();
		self.erase(entry);
		return entry;
	}

	constexpr T& pop_back(this auto &&self)
	{
		T &entry = self.back();
		self.erase(entry);
		return entry;
	}

	constexpr std::size_t size() const
	{
		return total_entries;
	}
};

// High-level interface; `T` must derive from `DoublyLinkedListEntry<T>`.
template<typename Base>
using DoublyLinkedListIteratorBase = SinglyLinkedListIteratorBase<Base>;

template<typename Base>
using DoublyLinkedListIterator = SinglyLinkedListIterator<Base>;

template<typename Base>
class DoublyLinkedListReverseIterator : public DoublyLinkedListIteratorBase<Base>
{
public:
	constexpr auto* Advance()
	{
		return this->GetHeldPointer()->previous;
	}
};

template<typename T>
class DoublyLinkedList : public DoublyLinkedListLowLevel<T, DoublyLinkedListIterator<ListIterator<T, T>>, DoublyLinkedListIterator<ListConstIterator<T, T>>, DoublyLinkedListReverseIterator<ListIterator<T, T>>, DoublyLinkedListReverseIterator<ListConstIterator<T, T>>>
{
public:
	static constexpr DoublyLinkedListEntry<T>& GetEntryFromHeldType(T &held)
	{
		return held;
	}
};

#endif /* DOUBLY_LINKED_LIST_H */
