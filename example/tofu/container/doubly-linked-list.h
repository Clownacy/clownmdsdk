#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <iterator>
#include <type_traits>

#include "list-common.h"
#include "singly-linked-list.h"

// Type-agnostic.
class DoublyLinkedListBase
{
public:
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	struct Entry : SinglyLinkedListBase::Entry
	{
		Entry *previous;

		constexpr Entry* Next() const
		{
			return static_cast<Entry*>(next);
		}
	};

protected:
	Entry root = {&root, &root};
	size_type total_entries = 0;

public:
	constexpr DoublyLinkedListBase() = default;

	constexpr DoublyLinkedListBase(const DoublyLinkedListBase &other) = delete;
	constexpr DoublyLinkedListBase(DoublyLinkedListBase &&other)
	{
		*this = std::move(other);
	}

	constexpr DoublyLinkedListBase& operator=(const DoublyLinkedListBase &other) = delete;
	constexpr DoublyLinkedListBase& operator=(DoublyLinkedListBase &&other)
	{
		// Point to our root node!
		auto &prev_a = root.Next()->previous;
		auto &prev_b = other.root.Next()->previous;
		auto &next_a = root.previous->next;
		auto &next_b = other.root.previous->next;
		std::swap(prev_a, prev_b);
		std::swap(next_a, next_b);

		std::swap(root, other.root);
		std::swap(total_entries, other.total_entries);

		return *this;
	}

	constexpr auto& front(this auto &&self)
	{
		_assertm(!self.empty(), "Tried to access first element of empty list.");
		return *self.root.Next();
	}

	constexpr auto& back(this auto &&self)
	{
		_assertm(!self.empty(), "Tried to access last element of empty list.");
		return *self.root.previous;
	}

	constexpr Entry* insert(Entry &position, Entry &value)
	{
		assert(&value != &root);

		++total_entries;

		position.previous->next = &value;

		value.next = &position;
		value.previous = position.previous;

		position.previous = &value;
		return &value;
	}

	constexpr void push_front(Entry &entry)
	{
		insert(*root.Next(), entry);
	}

	constexpr void push_back(Entry &entry)
	{
		insert(root, entry);
	}

	constexpr Entry* erase(Entry &position)
	{
		assert(&position != &root);

		--total_entries;

		auto* const next = position.Next();

		position.previous->next = position.next;
		position.Next()->previous = position.previous;

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

	constexpr size_type size() const
	{
		return total_entries;
	}

	constexpr bool empty() const
	{
		return root.next == &root;
	}

	constexpr void clear()
	{
		while (!empty())
			pop_front();
	}
};

// Type-specific.
template<typename T>
class DoublyLinkedList : public DoublyLinkedListBase
{
protected:
	using Base = DoublyLinkedListBase;

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
			return this->entry->Next();
		}

		constexpr IteratorBaseBase::held_type* Regress()
		{
			return this->entry->previous;
		}
	};

public:
	using value_type = T;
	using reference = value_type&;
	using const_reference = const value_type&;
	using iterator = IteratorBase<ListBiDirectionalIterator<DoublyLinkedListBase::Entry, T>>;
	using const_iterator = IteratorBase<ListConstBiDirectionalIterator<DoublyLinkedListBase::Entry, T>>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	using Base::Base;

	constexpr reference front()
	{
		return static_cast<reference>(Base::front());
	}

	constexpr const_reference front() const
	{
		return static_cast<const_reference>(Base::front());
	}

	constexpr reference back()
	{
		return static_cast<reference>(Base::back());
	}

	constexpr const_reference back() const
	{
		return static_cast<const_reference>(Base::back());
	}

	constexpr iterator insert(const_iterator position, reference value)
	{
		return iterator(Base::insert(*position.GetHeldPointer(), value));
	}

	constexpr void push_front(reference entry)
	{
		Base::push_front(entry);
	}

	constexpr void push_back(reference entry)
	{
		Base::push_back(entry);
	}

	constexpr iterator erase(const_iterator position)
	{
		return iterator(Base::erase(*position.GetHeldPointer()));
	}

	constexpr reference pop_front()
	{
		return static_cast<reference>(Base::pop_front());
	}

	constexpr reference pop_back()
	{
		return static_cast<reference>(Base::pop_back());
	}

	constexpr const_iterator cbegin() const
	{
		return const_iterator(root.Next());
	}

	constexpr const_iterator cend() const
	{
		return const_iterator(const_cast<Entry*>(&root));
	}

	constexpr const_iterator begin() const
	{
		return cbegin();
	}

	constexpr const_iterator end() const
	{
		return cend();
	}

	constexpr iterator begin()
	{
		return iterator(root.Next());
	}

	constexpr iterator end()
	{
		return iterator(&root);
	}

	constexpr const_reverse_iterator crbegin() const
	{
		return const_reverse_iterator(cend());
	}

	constexpr const_reverse_iterator crend() const
	{
		return const_reverse_iterator(cbegin());
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
		return reverse_iterator(end());
	}

	constexpr reverse_iterator rend()
	{
		return reverse_iterator(begin());
	}
};

#endif /* DOUBLY_LINKED_LIST_H */
