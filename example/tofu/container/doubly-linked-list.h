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
	struct Entry : SinglyLinkedListBase::Entry
	{
		Entry *previous;

		constexpr auto* Next(this auto &self)
		{
			return static_cast<decltype(&self)>(self.next);
		}
	};

protected:
	Entry root = {&root, &root};
	std::size_t total_entries = 0;

public:
	constexpr DoublyLinkedListBase() = default;

	constexpr DoublyLinkedListBase(const DoublyLinkedListBase &other) = delete;
	constexpr DoublyLinkedListBase(DoublyLinkedListBase &&other)
	{
		assert(root.next == &root);
		assert(root.previous == &root);
		assert(total_entries == 0);

		*this = std::move(other);
	}

	constexpr DoublyLinkedListBase& operator=(const DoublyLinkedListBase &other) = delete;
	constexpr DoublyLinkedListBase& operator=(DoublyLinkedListBase &&other)
	{
		// Point to our root node!
		auto &prev_a = this->root.Next()->previous;
		auto &prev_b = other.root.Next()->previous;
		auto &next_a = this->root.previous->next;
		auto &next_b = other.root.previous->next;
		std::swap(prev_a, prev_b);
		std::swap(next_a, next_b);

		std::swap(this->root, other.root);
		std::swap(this->total_entries, other.total_entries);

		assert(root.Next()->previous == &root);
		assert(root.previous->next == &root);

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

	constexpr void push_front(Entry &entry)
	{
		assert(&entry != &root);

		++total_entries;

		root.Next()->previous = &entry;

		entry.next = root.next;
		entry.previous = &root;

		root.next = &entry;
	}

	constexpr void push_back(Entry &entry)
	{
		assert(&entry != &root);

		++total_entries;

		root.previous->next = &entry;

		entry.next = &root;
		entry.previous = root.previous;

		root.previous = &entry;
	}

	constexpr Entry* erase(Entry &entry)
	{
		assert(&entry != &root);

		--total_entries;

		auto* const next = entry.Next();

		entry.previous->next = entry.next;
		entry.Next()->previous = entry.previous;

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
	using iterator = IteratorBase<ListBiDirectionalIterator<DoublyLinkedListBase::Entry, T>>;
	using const_iterator = IteratorBase<ListConstBiDirectionalIterator<DoublyLinkedListBase::Entry, T>>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
	using Base::Base;

	constexpr auto& front()
	{
		return static_cast<T&>(Base::front());
	}

	constexpr auto& front() const
	{
		return static_cast<const T&>(Base::front());
	}

	constexpr auto& back()
	{
		return static_cast<T&>(Base::back());
	}

	constexpr auto& back() const
	{
		return static_cast<const T&>(Base::back());
	}

	constexpr auto& pop_front()
	{
		return static_cast<T&>(Base::pop_front());
	}

	constexpr auto& pop_back()
	{
		return static_cast<T&>(Base::pop_back());
	}

	constexpr const_iterator cbegin() const
	{
		return const_iterator(this->root.Next());
	}

	constexpr const_iterator cend() const
	{
		return const_iterator(&this->root);
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
		return iterator(this->root.Next());
	}

	constexpr iterator end()
	{
		return iterator(&this->root);
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
