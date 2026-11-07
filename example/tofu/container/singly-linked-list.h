#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"

// Type-agnostic.
class SinglyLinkedListBase
{
public:
	struct Entry
	{
		Entry *next;
	};

protected:
	Entry root = {};

public:
	constexpr auto& front(this auto &&self)
	{
		_assertm(!self.empty(), "Tried to access first element of empty list.");
		return *self.root.next;
	}

	constexpr void push_front(Entry &entry)
	{
		entry.next = root.next;
		root.next = &entry;
	}

	constexpr Entry& pop_front()
	{
		Entry &entry = front();
		root.next = entry.next;
		return entry;
	}

	constexpr bool empty() const
	{
		return root.next == nullptr;
	}

	constexpr void clear()
	{
		while (!empty())
			pop_front();
	}
};

// Type-specific.
template<typename T, typename Base>
class SinglyLinkedListLowLevel : public Base
{
protected:
	template<typename IteratorBaseBase>
	class IteratorBase : public IteratorBaseBase
	{
	public:
		using IteratorBaseBase::IteratorBaseBase;

		constexpr auto& Dereference()
		{
			return static_cast<IteratorBaseBase::value_type&>(*this->GetHeldPointer());
		}

		constexpr auto* Advance()
		{
			return static_cast<IteratorBaseBase::held_type*>(this->GetHeldPointer()->next);
		}
	};

public:
	using iterator = IteratorBase<ListIterator<typename Base::Entry, T>>;
	using const_iterator = IteratorBase<ListConstIterator<typename Base::Entry, T>>;

	constexpr auto& front()
	{
		return static_cast<T&>(Base::front());
	}

	constexpr auto& front() const
	{
		return static_cast<const T&>(Base::front());
	}

	constexpr T& pop_front()
	{
		return static_cast<T&>(Base::pop_front());
	}

	constexpr iterator begin()
	{
		return iterator(static_cast<Base::Entry*>(this->root.next));
	}

	constexpr iterator end()
	{
		return iterator();
	}

	constexpr const_iterator begin() const
	{
		return const_iterator(static_cast<Base::Entry*>(this->root.next));
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
};

template<typename T>
using SinglyLinkedList = SinglyLinkedListLowLevel<T, SinglyLinkedListBase>;

#endif /* SINGLY_LINKED_LIST_H */
