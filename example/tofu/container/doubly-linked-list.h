#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"
#include "singly-linked-list.h"

// Type-agnostic.
class DoublyLinkedListBase
{
public:
	struct Entry : SinglyLinkedListBase::Entry
	{
		Entry *previous;
	};

protected:
	Entry root = {};
	std::size_t total_entries = 0;

public:
	constexpr auto& front()
	{
		_assertm(!empty(), "Tried to access first element of empty list.");
		return static_cast<Entry&>(*root.next);
	}

	constexpr auto& front() const
	{
		_assertm(!empty(), "Tried to access first element of empty list.");
		return static_cast<const Entry&>(*root.next);
	}

	constexpr auto& back(this auto &&self)
	{
		_assertm(!empty(), "Tried to access last element of empty list.");
		return *self.root.previous;
	}

	constexpr void push_front(Entry &entry)
	{
		++total_entries;

		if (root.next == nullptr)
			root.previous = &entry;
		else
			static_cast<Entry*>(root.next)->previous = &entry;

		entry.next = root.next;
		entry.previous = nullptr;

		root.next = &entry;
	}

	constexpr void push_back(Entry &entry)
	{
		++total_entries;

		if (root.previous == nullptr)
			root.next = &entry;
		else
			root.previous->next = &entry;

		entry.next = nullptr;
		entry.previous = root.previous;

		root.previous = &entry;
	}

	constexpr Entry* erase(Entry &entry)
	{
		--total_entries;

		auto* const next = static_cast<Entry*>(entry.next);

		if (entry.previous == nullptr)
			root.next = entry.next;
		else
			entry.previous->next = entry.next;

		if (entry.next == nullptr)
			root.previous = entry.previous;
		else
			static_cast<Entry*>(entry.next)->previous = entry.previous;

		return next;
	}

	constexpr Entry& pop_front()
	{
		Entry &entry = front();
		erase(entry);
		return entry;
	}

	constexpr Entry& pop_back()
	{
		Entry &entry = back();
		erase(entry);
		return entry;
	}

	constexpr std::size_t size() const
	{
		return total_entries;
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
template<typename T>
class DoublyLinkedList : public SinglyLinkedListLowLevel<T, DoublyLinkedListBase>
{
protected:
	using Base = SinglyLinkedListLowLevel<T, DoublyLinkedListBase>;

	template<typename IteratorBaseBase>
	class ReverseIteratorBase : public IteratorBaseBase
	{
	public:
		using IteratorBaseBase::IteratorBaseBase;

		constexpr auto& Dereference()
		{
			return static_cast<IteratorBaseBase::value_type&>(*this->GetHeldPointer());
		}

		constexpr auto* Advance()
		{
			return static_cast<IteratorBaseBase::held_type*>(this->GetHeldPointer()->previous);
		}
	};

public:
	using reverse_iterator = ReverseIteratorBase<ListIterator<DoublyLinkedListBase::Entry, T>>;
	using const_reverse_iterator = ReverseIteratorBase<ListConstIterator<DoublyLinkedListBase::Entry, T>>;

public:
	constexpr auto& back()
	{
		return static_cast<T&>(Base::back());
	}

	constexpr auto& back() const
	{
		return static_cast<const T&>(Base::back());
	}

	constexpr T& pop_back()
	{
		return static_cast<T&>(Base::pop_back());
	}

	constexpr const_reverse_iterator crbegin() const
	{
		return const_reverse_iterator(this->root.previous);
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
		return reverse_iterator(this->root.previous);
	}

	constexpr reverse_iterator rend()
	{
		return reverse_iterator();
	}
};

#endif /* DOUBLY_LINKED_LIST_H */
