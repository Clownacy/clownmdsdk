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
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	struct Entry
	{
		Entry *next;
	};

protected:
	Entry root = {nullptr};

public:
	constexpr auto& front(this auto &&self)
	{
		_assertm(!self.empty(), "Tried to access first element of empty list.");
		return *self.root.next;
	}

	constexpr Entry* insert_after(Entry &position, Entry &value)
	{
		value.next = position.next;
		position.next = &value;
		return &value;
	}

	constexpr void push_front(Entry &entry)
	{
		insert_after(root, entry);
	}

	constexpr Entry* erase_after(Entry &position)
	{
		position.next = position.next->next;
		return position.next;
	}

	constexpr void pop_front()
	{
		erase_after(root);
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
class SinglyLinkedList : public SinglyLinkedListBase
{
protected:
	using Base = SinglyLinkedListBase;

	template<typename IteratorBaseBase>
	class IteratorBase : public IteratorBaseBase
	{
	public:
		using IteratorBaseBase::IteratorBaseBase;

		constexpr auto& Dereference()
		{
			return static_cast<IteratorBaseBase::value_type&>(*this->entry);
		}

		constexpr IteratorBaseBase::held_type* Advance()
		{
			return this->entry->next;
		}
	};

public:
	using value_type = T;
	using reference = value_type&;
	using const_reference = const value_type&;
	using iterator = IteratorBase<ListForwardIterator<Base::Entry, T>>;
	using const_iterator = IteratorBase<ListConstForwardIterator<Base::Entry, T>>;

	using Base::Base;

	constexpr reference front()
	{
		return static_cast<reference>(Base::front());
	}

	constexpr const_reference front() const
	{
		return static_cast<const_reference>(Base::front());
	}

	constexpr iterator insert_after(const_iterator position, reference value)
	{
		return iterator(Base::insert_after(*position.GetHeldPointer(), value));
	}

	constexpr void push_front(reference entry)
	{
		Base::push_front(entry);
	}

	constexpr iterator erase_after(const_iterator position)
	{
		return iterator(Base::erase_after(*position.GetHeldPointer()));
	}

	constexpr const_iterator cbefore_begin() const
	{
		return const_iterator(&root);
	}

	constexpr const_iterator cbegin() const
	{
		return const_iterator(root.next);
	}

	constexpr const_iterator cend() const
	{
		return const_iterator();
	}

	constexpr const_iterator before_begin() const
	{
		return cbefore_begin();
	}

	constexpr const_iterator begin() const
	{
		return cbegin();
	}

	constexpr const_iterator end() const
	{
		return cend();
	}

	constexpr iterator before_begin()
	{
		return iterator(&root);
	}

	constexpr iterator begin()
	{
		return iterator(root.next);
	}

	constexpr iterator end()
	{
		return iterator();
	}
};

#endif /* SINGLY_LINKED_LIST_H */
