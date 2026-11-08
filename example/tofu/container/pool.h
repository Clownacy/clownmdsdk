#ifndef POOL_H
#define POOL_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>

#include "doubly-linked-list.h"
#include "singly-linked-list.h"

// Pool
template<typename T, std::size_t S>
class Pool
{
private:
	// Iterators
	template<typename Base, typename EntryUnion>
	class AllocatedIteratorBase : public Base
	{
	public:
		using Base::Base;

		constexpr auto& Dereference()
		{
			return static_cast<EntryUnion*>(this->entry)->value;
		}

		constexpr auto* Advance()
		{
			return static_cast<Base::held_type*>(this->entry->next);
		}

		constexpr auto* Regress()
		{
			return static_cast<Base::held_type*>(this->entry->previous);
		}
	};

	// EntryUnion
	struct EntryUnion : DoublyLinkedList<EntryUnion>::Entry
	{
		union
		{
			T value;
		};

		constexpr EntryUnion() {}
	};

	// Actual Pool stuff begins here.
	DoublyLinkedList<EntryUnion> allocated_list;
	SinglyLinkedList<EntryUnion> deallocated_list;
	std::array<EntryUnion, S> storage;

	template<typename ...Ts>
	constexpr EntryUnion& pop_from_deallocated(Ts &&...args)
	{
		auto &entry = deallocated_list.pop_front();
		std::construct_at(&entry.value, std::forward<Ts>(args)...);
		return entry;
	}

	constexpr void deallocate(EntryUnion &entry)
	{
		allocated_list.erase(entry);
		entry.value.~T();
		deallocated_list.push_front(entry);
	}

public:
	using iterator = AllocatedIteratorBase<ListBiDirectionalIterator<DoublyLinkedListBase::Entry, T>, EntryUnion>;
	using const_iterator = AllocatedIteratorBase<ListConstBiDirectionalIterator<DoublyLinkedListBase::Entry, T>, const EntryUnion>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

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
			allocate_back(entry);

		return *this;
	}

	constexpr Pool& operator=(Pool &&other)
	{
		clear();

		for (auto it = other.begin(); it != other.end(); )
		{
			allocate_back(std::move(*it));
			it = other.deallocate(it);
		}

		return *this;
	}

	template<typename ...Ts>
	constexpr iterator allocate_front(Ts &&...args)
	{
		auto &entry = pop_from_deallocated(std::forward<Ts>(args)...);
		allocated_list.push_front(entry);
		return iterator(&entry);
	}

	template<typename ...Ts>
	constexpr iterator allocate_back(Ts &&...args)
	{
		auto &entry = pop_from_deallocated(std::forward<Ts>(args)...);
		allocated_list.push_back(entry);
		return iterator(&entry);
	}

	constexpr iterator deallocate(iterator it)
	{
		auto &entry = static_cast<EntryUnion&>(*it.GetHeldPointer());
		++it;
		deallocate(entry);
		return it;
	}

	constexpr const_iterator cbegin() const
	{
		return allocated_list.cbegin().GetHeldPointer();
	}

	constexpr const_iterator cend() const
	{
		return allocated_list.cend().GetHeldPointer();
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
		return allocated_list.begin().GetHeldPointer();
	}

	constexpr iterator end()
	{
		return allocated_list.end().GetHeldPointer();
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

	constexpr std::size_t size() const
	{
		return allocated_list.size();
	}

	constexpr std::size_t max_size() const
	{
		return S;
	}

	constexpr void clear()
	{
		for (auto it = begin(); it != end(); )
			it = deallocate(it);
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
