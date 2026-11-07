#ifndef POOL_H
#define POOL_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <type_traits>

#include "doubly-linked-list.h"
#include "singly-linked-list.h"

// Pool
template<typename T, std::size_t S>
class Pool
{
private:
	// Define a bunch of internal classes.

	struct EntryUnion;

	// Iterators
	template<typename Base, typename EntryUnion>
	class AllocatedIteratorBase : public Base
	{
	public:
		using Base::Base;

		constexpr auto& Dereference()
		{
			return static_cast<EntryUnion*>(this->GetHeldPointer())->allocated.value;
		}

		constexpr auto* Advance()
		{
			return static_cast<Base::held_type*>(static_cast<EntryUnion*>(this->GetHeldPointer())->next);
		}
	};

	using AllocatedIterator = AllocatedIteratorBase<ListIterator<DoublyLinkedListBase::Entry, T>, EntryUnion>;
	using AllocatedConstIterator = AllocatedIteratorBase<ListConstIterator<DoublyLinkedListBase::Entry, T>, const EntryUnion>;

	template<typename Base, typename EntryUnion>
	class AllocatedReverseIteratorBase : public Base
	{
	public:
		using Base::Base;

		constexpr auto& Dereference()
		{
			return static_cast<EntryUnion*>(this->GetHeldPointer())->allocated.value;
		}

		constexpr auto* Advance()
		{
			return static_cast<Base::held_type*>(static_cast<EntryUnion*>(this->GetHeldPointer())->previous);
		}
	};

	using AllocatedReverseIterator = AllocatedReverseIteratorBase<ListIterator<DoublyLinkedListBase::Entry, T>, EntryUnion>;
	using AllocatedConstReverseIterator = AllocatedReverseIteratorBase<ListConstIterator<DoublyLinkedListBase::Entry, T>, const EntryUnion>;

	// EntryAllocated
	struct EntryAllocated
	{
		T value;

		template<typename... Args>
		constexpr EntryAllocated(Args &&...other)
			: value(std::forward<Args>(other)...)
		{}
	};

	// EntryDeallocated
	struct EntryDeallocated {};

	// EntryUnion
	struct EntryUnion : DoublyLinkedList<EntryUnion>::Entry
	{
		union
		{
			EntryAllocated allocated;
			EntryDeallocated deallocated;
		};

		constexpr EntryUnion() {}
	};

	// Actual Pool stuff begins here.
	using iterator = AllocatedIterator;
	using const_iterator = AllocatedConstIterator;
	using reverse_iterator = AllocatedReverseIterator;
	using const_reverse_iterator = AllocatedConstReverseIterator;

	DoublyLinkedList<EntryUnion> allocated_list;
	SinglyLinkedList<EntryUnion> deallocated_list;
	std::array<EntryUnion, S> storage;

	template<typename ...Ts>
	constexpr void push_to_deallocated(EntryUnion &entry, Ts &&...args)
	{
		std::construct_at(&entry.deallocated, std::forward<Ts>(args)...);
		deallocated_list.push_front(entry);
	}

	template<typename ...Ts>
	constexpr EntryUnion& pop_from_deallocated(Ts &&...args)
	{
		auto &entry = deallocated_list.pop_front();

		entry.deallocated.~EntryDeallocated();

		std::construct_at(&entry.allocated, std::forward<Ts>(args)...);

		return entry;
	}

	constexpr void deallocate(EntryUnion &entry)
	{
		// Remove from allocated object list.
		allocated_list.erase(entry);

		entry.allocated.~EntryAllocated();

		// Add to deallocated object list.
		push_to_deallocated(entry);
	}

public:
	constexpr Pool()
	{
		for (auto &entry : storage)
			push_to_deallocated(entry);
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

	template<typename Iterator> requires std::is_same_v<Iterator, iterator> || std::is_same_v<Iterator, reverse_iterator>
	constexpr Iterator deallocate(Iterator it)
	{
		auto &entry = static_cast<EntryUnion&>(*it.GetHeldPointer());
		++it;
		deallocate(entry);
		return it;
	}

	constexpr iterator begin()
	{
		return allocated_list.begin().GetHeldPointer();
	}

	constexpr iterator end()
	{
		return allocated_list.end().GetHeldPointer();
	}

	constexpr const_iterator begin() const
	{
		return allocated_list.begin().GetHeldPointer();
	}

	constexpr const_iterator end() const
	{
		return allocated_list.end().GetHeldPointer();
	}

	constexpr const_iterator cbegin() const
	{
		return allocated_list.cbegin().GetHeldPointer();
	}

	constexpr const_iterator cend() const
	{
		return allocated_list.cend().GetHeldPointer();
	}

	constexpr reverse_iterator rbegin()
	{
		return allocated_list.rbegin().GetHeldPointer();
	}

	constexpr reverse_iterator rend()
	{
		return allocated_list.rend().GetHeldPointer();
	}

	constexpr const_reverse_iterator rbegin() const
	{
		return allocated_list.rbegin().GetHeldPointer();
	}

	constexpr const_reverse_iterator rend() const
	{
		return allocated_list.rend().GetHeldPointer();
	}

	constexpr const_reverse_iterator crbegin() const
	{
		return allocated_list.crbegin().GetHeldPointer();
	}

	constexpr const_reverse_iterator crend() const
	{
		return allocated_list.crend().GetHeldPointer();
	}

	constexpr auto& front(this auto &&self)
	{
		return self.allocated_list.front().allocated.value;
	}

	constexpr auto& back(this auto &&self)
	{
		return self.allocated_list.back().allocated.value;
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
