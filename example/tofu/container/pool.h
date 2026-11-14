#ifndef POOL_H
#define POOL_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>

#include "doubly-linked-list.h"
#include "singly-linked-list.h"

template<typename T, std::size_t S>
class Pool
{
private:
	template<typename IteratorBaseBase, typename Entry>
	class IteratorBase : public IteratorBaseBase
	{
	public:
		using IteratorBaseBase::IteratorBaseBase;

		constexpr auto& Dereference()
		{
			return static_cast<Entry*>(this->entry)->value;
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

	struct Entry : DoublyLinkedList<Entry>::Entry
	{
		union
		{
			T value;
		};

		constexpr Entry() {}
	};

public:
	using value_type = T;
	using size_type = std::size_t;
	using reference = value_type&;
	using const_reference = const value_type&;
	using iterator = IteratorBase<ListBiDirectionalIterator<DoublyLinkedListBase::Entry, T>, Entry>;
	using const_iterator = IteratorBase<ListConstBiDirectionalIterator<DoublyLinkedListBase::Entry, T>, const Entry>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
	using AllocatedList = DoublyLinkedList<Entry>;
	using DeallocatedList = SinglyLinkedList<Entry>;

	AllocatedList allocated_list;
	DeallocatedList deallocated_list;
	std::array<Entry, S> storage;

	template<typename ...Ts>
	constexpr iterator emplace(const auto &callback, Ts &&...args)
	{
		auto &entry = deallocated_list.pop_front();
		std::construct_at(&entry.value, std::forward<Ts>(args)...);
		callback(entry);
		return iterator(&entry);
	}

public:
	constexpr Pool()
	{
		for (auto &entry : storage)
			deallocated_list.push_front(entry);
	}

	constexpr ~Pool()
	{
		clear();
	}

	constexpr Pool(const Pool &other)
		: Pool()
	{
		*this = other;
	}

	constexpr Pool(Pool &&other)
		: Pool()
	{
		*this = std::move(other);
	}

	constexpr Pool& operator=(const Pool &other)
	{
		clear();

		for (const auto &entry : other)
			emplace_back(entry);

		return *this;
	}

	constexpr Pool& operator=(Pool &&other)
	{
		clear();

		for (auto it = other.begin(); it != other.end(); )
		{
			emplace_back(std::move(*it));
			it = other.erase(it);
		}

		return *this;
	}

	template<typename ...Ts>
	constexpr iterator emplace_front(Ts &&...args)
	{
		return emplace(
			[this](Entry &entry)
			{
				allocated_list.push_front(entry);
			},
			std::forward<Ts>(args)...
		);
	}

	template<typename ...Ts>
	constexpr iterator emplace_back(Ts &&...args)
	{
		return emplace(
			[this](Entry &entry)
			{
				allocated_list.push_back(entry);
			},
			std::forward<Ts>(args)...
		);
	}

	constexpr iterator push_front(const T &value)
	{
		return emplace_front(value);
	}

	constexpr iterator push_front(T &&value)
	{
		return emplace_front(std::move(value));
	}

	constexpr iterator push_back(const T &value)
	{
		return emplace_back(value);
	}

	constexpr iterator push_back(T &&value)
	{
		return emplace_back(std::move(value));
	}

	constexpr iterator erase(iterator it)
	{
		auto &entry = static_cast<Entry&>(*it.GetHeldPointer());
		++it;
		allocated_list.erase(typename AllocatedList::iterator(&entry));
		entry.value.~T();
		deallocated_list.push_front(entry);
		return it;
	}

	constexpr const_iterator cbegin() const
	{
		return const_iterator(allocated_list.cbegin().GetHeldPointer());
	}

	constexpr const_iterator cend() const
	{
		return const_iterator(allocated_list.cend().GetHeldPointer());
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
		return iterator(allocated_list.begin().GetHeldPointer());
	}

	constexpr iterator end()
	{
		return iterator(allocated_list.end().GetHeldPointer());
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

	constexpr auto& front(this auto &&self)
	{
		return self.allocated_list.front().value;
	}

	constexpr auto& back(this auto &&self)
	{
		return self.allocated_list.back().value;
	}

	constexpr size_type size() const
	{
		return allocated_list.size();
	}

	constexpr size_type max_size() const
	{
		return S;
	}

	constexpr void clear()
	{
		for (auto it = begin(); it != end(); )
			it = erase(it);
	}

	constexpr bool empty() const
	{
		return allocated_list.empty();
	}

	constexpr bool full() const
	{
		return deallocated_list.empty();
	}
};

#endif /* POOL_H */
