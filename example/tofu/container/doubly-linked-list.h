#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"

template<typename T>
struct DoublyLinkedListEntry
{
	T *next;
	T *previous;
};

// Low-level interface; user decides how `T` contains `DoublyLinkedListEntry<T>` by specifying own `Iterator`.
template<typename T, typename Iterator, typename ConstIterator, typename ReverseIterator, typename ConstReverseIterator>
class DoublyLinkedListLowLevel
{
public:
	using Entry = DoublyLinkedListEntry<T>;

private:
	T *head = nullptr;
	T *tail = nullptr;
	std::size_t total_entries = 0;

public:
	constexpr auto& front(this auto &&self)
	{
		_assertm(self.head != nullptr, "Tried to access first element of empty list.");
		return *self.head;
	}

	constexpr auto& back(this auto &&self)
	{
		_assertm(self.tail != nullptr, "Tried to access last element of empty list.");
		return *self.tail;
	}

	constexpr ConstIterator cbegin() const
	{
		return ConstIterator(head);
	}

	constexpr ConstIterator cend() const
	{
		return ConstIterator();
	}

	constexpr ConstIterator begin() const
	{
		return cbegin();
	}

	constexpr ConstIterator end() const
	{
		return cend();
	}

	constexpr Iterator begin()
	{
		return Iterator(head);
	}

	constexpr Iterator end()
	{
		return Iterator();
	}

	constexpr ConstReverseIterator crbegin() const
	{
		return ConstReverseIterator(tail);
	}

	constexpr ConstReverseIterator crend() const
	{
		return ConstReverseIterator();
	}

	constexpr ConstReverseIterator rbegin() const
	{
		return crbegin();
	}

	constexpr ConstReverseIterator rend() const
	{
		return crend();
	}

	constexpr ReverseIterator rbegin()
	{
		return ReverseIterator(tail);
	}

	constexpr ReverseIterator rend()
	{
		return ReverseIterator();
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
		auto &entry = self.front();
		self.erase(entry);
		return entry;
	}

	constexpr T& pop_back(this auto &&self)
	{
		auto &entry = self.back();
		self.erase(entry);
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

	constexpr void clear(this auto &&self)
	{
		while (!self.empty())
			self.pop_front();
	}
};

// High-level interface; `T` must derive from `DoublyLinkedListEntry<T>`.
template<typename Base>
class DoublyLinkedListIteratorBase : public Base
{
public:
	using Base::Base;

	constexpr auto& Dereference()
	{
		return *this->GetHeldPointer();
	}
};

template<typename Base>
class DoublyLinkedListIterator : public DoublyLinkedListIteratorBase<Base>
{
public:
	constexpr auto* Advance()
	{
		return this->GetHeldPointer()->next;
	}
};

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
