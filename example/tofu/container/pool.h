#ifndef POOL_H
#define POOL_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <type_traits>

#include "doubly-linked-list.h"
#include "singly-linked-list.h"

namespace PoolEntry
{
	// Union
	template<typename T>
	struct Allocated;

	template<typename T>
	struct Deallocated;

	template<typename T>
	union Union
	{
		Allocated<T> allocated;
		Deallocated<T> deallocated;

		constexpr Union() {}
	};

	// Allocated
	template<typename T>
	using AllocatedList = DoublyLinkedListLowLevel<Union<T>, Allocated<T>>;

	template<typename T>
	struct Allocated : public AllocatedList<T>::Entry, public T
	{
		using T::T;

		static constexpr AllocatedList<T>::Entry& GetEntryFromHeldType(Union<T> &held)
		{
			return held.allocated;
		}
	};

	// Deallocated
	template<typename T>
	using DeallocatedList = SinglyLinkedListLowLevel<Union<T>, Deallocated<T>>;

	template<typename T>
	struct Deallocated : public DeallocatedList<T>::Entry
	{
		static constexpr DeallocatedList<T>::Entry& GetEntryFromHeldType(Union<T> &held)
		{
			return held.deallocated;
		}
	};
}

// Pool
template<typename T, std::size_t S>
class Pool;

template<typename T, std::size_t S>
using PoolCommon = ListCommon<Pool<T, S>, PoolEntry::Union<T>>;

template<typename T, std::size_t S>
class Pool : protected PoolCommon<T, S>
{
private:
	using Common = PoolCommon<T, S>;
	using Allocated = PoolEntry::Allocated<T>;
	using Deallocated = PoolEntry::Deallocated<T>;
	using Union = PoolEntry::Union<T>;

	PoolEntry::AllocatedList<T> allocated_list;
	PoolEntry::DeallocatedList<T> deallocated_list;
	std::array<PoolEntry::Union<T>, S> storage;

	template<typename ...Ts>
	constexpr Union& pop_from_deallocated(Ts &&...args)
	{
		auto &entry_union = deallocated_list.pop_front();

		entry_union.deallocated.~Deallocated();

		std::construct_at(&entry_union.allocated, std::forward<Ts>(args)...);

		return entry_union;
	}

public:
	constexpr Pool()
	{
		for (auto &entry : storage)
		{
			std::construct_at(&entry.deallocated);
			deallocated_list.push_front(entry);
		}
	}

	constexpr ~Pool()
	{
		for (auto it = begin(); it != end(); )
			it = deallocate(it);
	}

	constexpr Pool(const Pool &other) = delete;
	constexpr Pool& operator=(const Pool &other) = delete;

	template<typename ...Ts>
	constexpr T& allocate_front(Ts &&...args)
	{
		auto &entry = pop_from_deallocated(std::forward<Ts>(args)...);
		allocated_list.push_front(entry);
		return entry.allocated;
	}

	template<typename ...Ts>
	constexpr T& allocate_back(Ts &&...args)
	{
		auto &entry = pop_from_deallocated(std::forward<Ts>(args)...);
		allocated_list.push_back(entry);
		return entry.allocated;
	}

	constexpr Common::Iterator deallocate(Common::Iterator it)
	{
		Union &entry_union = *it.GetHeldTypePointer();

		++it;

		// Remove from allocated object list.
		allocated_list.erase(entry_union);

		entry_union.allocated.~Allocated();

		// Add to deallocated object list.
		std::construct_at(&entry_union.deallocated);
		deallocated_list.push_front(entry_union);

		return it;
	}

	constexpr Common::Iterator begin()
	{
		return typename Common::Iterator(allocated_list.begin().GetHeldTypePointer());
	}

	constexpr Common::Iterator end()
	{
		return typename Common::Iterator();
	}

	template<typename Self>
	constexpr T& front(this Self &self)
	{
		return self.allocated_list.front().allocated;
	}

	template<typename Self>
	constexpr T& back(this Self &self)
	{
		return self.allocated_list.back().allocated;
	}

	constexpr std::size_t size() const
	{
		return allocated_list.size();
	}

	constexpr std::size_t max_size() const
	{
		return S;
	}

	constexpr bool empty() const
	{
		return allocated_list.empty();
	}

	constexpr bool full() const
	{
		return deallocated_list.empty();
	}

	static constexpr Allocated& GetEntryFromHeldType(Union &held)
	{
		return held.allocated;
	}

	static constexpr T& DereferenceHeldType(Union &held)
	{
		return held.allocated;
	}
};

#endif /* POOL_H */
