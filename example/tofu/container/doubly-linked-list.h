#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"
#include "singly-linked-list.h"

template<typename T>
class DoublyLinkedList : public SinglyLinkedList<T>
{
protected:
	using Base = SinglyLinkedList<T>;

	template<typename Base>
	class ReverseIteratorBase : public Base
	{
	public:
		using Base::Base;

		constexpr auto& Dereference()
		{
			return *this->GetHeldPointer();
		}

		constexpr auto* Advance()
		{
			return this->GetHeldPointer()->previous;
		}
	};

public:
	struct Entry : public Base::Entry
	{
		T *previous;
	};

	using reverse_iterator = ReverseIteratorBase<ListIterator<T, T>>;
	using const_reverse_iterator = ReverseIteratorBase<ListConstIterator<T, T>>;

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

	constexpr void push_front(T &entry)
	{
		++total_entries;

		if (this->head == nullptr)
			this->tail = &entry;
		else
			this->head->previous = &entry;

		entry.next = this->head;
		entry.previous = nullptr;

		this->head = &entry;
	}

	constexpr void push_back(T &entry)
	{
		++total_entries;

		if (this->tail == nullptr)
			this->head = &entry;
		else
			this->tail->next = &entry;

		entry.next = nullptr;
		entry.previous = this->tail;

		this->tail = &entry;
	}

	constexpr T* erase(T &entry)
	{
		--total_entries;

		T* const next = entry.next;

		if (entry.previous == nullptr)
			this->head = entry.next;
		else
			entry.previous->next = entry.next;

		if (entry.next == nullptr)
			this->tail = entry.previous;
		else
			entry.next->previous = entry.previous;

		return next;
	}

	constexpr T& pop_front()
	{
		T &entry = this->front();
		erase(entry);
		return entry;
	}

	constexpr T& pop_back()
	{
		T &entry = this->back();
		erase(entry);
		return entry;
	}

	constexpr std::size_t size() const
	{
		return total_entries;
	}
};

#endif /* DOUBLY_LINKED_LIST_H */
