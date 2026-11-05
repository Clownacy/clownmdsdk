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

	// EntryUnion
	struct EntryAllocated;
	struct EntryDeallocated;

	union EntryUnion
	{
		EntryAllocated allocated;
		EntryDeallocated deallocated;

		constexpr EntryUnion() {}
	};

	// Iterators
	template<typename Held, typename Dereferenced>
	class AllocatedIteratorBase : public ListIterator<Held, Dereferenced>
	{
	public:
		constexpr Dereferenced& Dereference()
		{
			return this->GetHeldPointer()->allocated.value;
		}

		constexpr Held* Advance()
		{
			return this->GetHeldPointer()->allocated.next;
		}
	};

	using AllocatedIterator = AllocatedIteratorBase<EntryUnion, T>;
	using AllocatedConstIterator = AllocatedIteratorBase<const EntryUnion, const T>;

	template<typename Held, typename Dereferenced>
	class AllocatedReverseIteratorBase : public ListIterator<Held, Dereferenced>
	{
	public:
		constexpr Dereferenced& Dereference()
		{
			return this->GetHeldPointer()->allocated.value;
		}

		constexpr Held* Advance()
		{
			return this->GetHeldPointer()->allocated.previous;
		}
	};

	using AllocatedReverseIterator = AllocatedReverseIteratorBase<EntryUnion, T>;
	using AllocatedConstReverseIterator = AllocatedReverseIteratorBase<const EntryUnion, const T>;

	template<typename Held, typename Dereferenced>
	class DeallocatedIteratorBase : public ListIterator<Held, Dereferenced>
	{
	public:
		constexpr Dereferenced& Dereference()
		{
			return this->GetHeldPointer()->deallocated;
		}

		constexpr Held* Advance()
		{
			return this->GetHeldPointer()->deallocated.next;
		}
	};

	using DeallocatedIterator = DeallocatedIteratorBase<EntryUnion, T>;
	using DeallocatedConstIterator = DeallocatedIteratorBase<const EntryUnion, const T>;

	// EntryAllocated
	using EntryAllocatedListBase = DoublyLinkedListLowLevel<EntryUnion, AllocatedIterator, AllocatedConstIterator, AllocatedReverseIterator, AllocatedConstReverseIterator>;

	class EntryAllocatedList : public EntryAllocatedListBase
	{
	public:
		using Base = EntryAllocatedListBase;

		static constexpr Base::Entry& GetEntryFromHeldType(EntryUnion &held)
		{
			return held.allocated;
		}
	};

	struct EntryAllocated : public EntryAllocatedList::Entry
	{
		T value;

		template<typename... Args>
		constexpr EntryAllocated(Args &&...other)
			: value(std::forward<Args>(other)...)
		{}
	};

	// EntryDeallocated
	using EntryDeallocatedListBase = SinglyLinkedListLowLevel<EntryUnion, DeallocatedIterator, DeallocatedConstIterator>;

	class EntryDeallocatedList : public EntryDeallocatedListBase
	{
	public:
		using Base = EntryDeallocatedListBase;

		static constexpr Base::Entry& GetEntryFromHeldType(EntryUnion &held)
		{
			return held.deallocated;
		}
	};

	struct EntryDeallocated : public EntryDeallocatedList::Entry {};

	// Actual Pool stuff begins here.
	using Iterator = AllocatedIterator;
	using ConstIterator = AllocatedConstIterator;
	using ReverseIterator = AllocatedReverseIterator;
	using ConstReverseIterator = AllocatedConstReverseIterator;

	EntryAllocatedList allocated_list;
	EntryDeallocatedList deallocated_list;
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
	constexpr Iterator allocate_front(Ts &&...args)
	{
		auto &entry = pop_from_deallocated(std::forward<Ts>(args)...);
		allocated_list.push_front(entry);
		return Iterator(&entry);
	}

	template<typename ...Ts>
	constexpr Iterator allocate_back(Ts &&...args)
	{
		auto &entry = pop_from_deallocated(std::forward<Ts>(args)...);
		allocated_list.push_back(entry);
		return Iterator(&entry);
	}

	template<typename IteratorT> requires std::is_same_v<IteratorT, Iterator> || std::is_same_v<IteratorT, ReverseIterator>
	constexpr IteratorT deallocate(IteratorT it)
	{
		EntryUnion &entry = *it.GetHeldPointer();

		++it;

		deallocate(entry);

		return it;
	}

	constexpr auto begin(this auto &&self)
	{
		return self.allocated_list.begin();
	}

	constexpr auto end(this auto &&self)
	{
		return self.allocated_list.end();
	}

	constexpr ConstIterator cbegin() const
	{
		return allocated_list.cbegin();
	}

	constexpr ConstIterator cend() const
	{
		return allocated_list.cend();
	}

	constexpr auto rbegin(this auto &&self)
	{
		return self.allocated_list.rbegin();
	}

	constexpr auto rend(this auto &&self)
	{
		return self.allocated_list.rend();
	}

	constexpr ConstReverseIterator crbegin() const
	{
		return allocated_list.crbegin();
	}

	constexpr ConstReverseIterator crend() const
	{
		return allocated_list.crend();
	}

	template<typename Self>
	constexpr T& front(this Self &&self)
	{
		return self.allocated_list.front().allocated.value;
	}

	template<typename Self>
	constexpr T& back(this Self &&self)
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
