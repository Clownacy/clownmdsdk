#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"

template<typename T>
class SinglyLinkedList
{
protected:
	template<typename Base>
	class IteratorBase : public Base
	{
	public:
		using Base::Base;

		constexpr auto& Dereference()
		{
			return *this->GetHeldPointer();
		}

		constexpr auto* Advance()
		{
			return this->GetHeldPointer()->next;
		}
	};

public:
	struct Entry
	{
		T *next;
	};

	using iterator = IteratorBase<ListIterator<T, T>>;
	using const_iterator = IteratorBase<ListConstIterator<T, T>>;

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

	constexpr void push_front(T &entry)
	{
		entry.next = head;
		head = &entry;
	}

	constexpr T& pop_front()
	{
		T &entry = front();
		head = entry.next;
		return entry;
	}

	constexpr bool empty() const
	{
		return head == nullptr;
	}

	constexpr void clear()
	{
		while (!empty())
			pop_front();
	}
};

#endif /* SINGLY_LINKED_LIST_H */
