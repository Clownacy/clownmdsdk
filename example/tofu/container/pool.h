#ifndef POOL_H
#define POOL_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <type_traits>

#include "doubly-linked-list.h"
#include "singly-linked-list.h"

template<typename T>
struct PoolEntry;

template<typename T>
struct PoolEntryEmpty;

template<typename T>
union PoolEntryUnion
{
	PoolEntry<T> entry;
	PoolEntryEmpty<T> entry_empty;

	constexpr PoolEntryUnion() {}
};

template<typename T>
struct PoolEntry;

template<typename T>
using PoolEntryList = DoublyLinkedListLowLevel<PoolEntryUnion<T>, PoolEntry<T>>;

template<typename T>
struct PoolEntry : public PoolEntryList<T>::Entry, public T
{
	using T::T;

	static constexpr PoolEntryList<T>::Entry& GetEntryFromHeldType(PoolEntryUnion<T> &held)
	{
		return held.entry;
	}
};

template<typename T>
struct PoolEntryEmpty;

template<typename T>
using PoolEntryEmptyList = SinglyLinkedListLowLevel<PoolEntryUnion<T>, PoolEntryEmpty<T>>;

template<typename T>
struct PoolEntryEmpty : public PoolEntryEmptyList<T>::Entry
{
	static constexpr PoolEntryEmptyList<T>::Entry& GetEntryFromHeldType(PoolEntryUnion<T> &held)
	{
		return held.entry_empty;
	}
};

template<typename T, std::size_t S>
class Pool;

template<typename T, std::size_t S>
using PoolCommon = ListCommon<Pool<T, S>, PoolEntryUnion<T>>;

template<typename T, std::size_t S>
class Pool : protected PoolCommon<T, S>
{
public:
	using Entry = PoolEntry<T>;

private:
	using EntryEmpty = PoolEntryEmpty<T>;
	using EntryUnion = PoolEntryUnion<T>;
	using Common = PoolCommon<T, S>;

	PoolEntryList<T> allocated;
	PoolEntryEmptyList<T> deallocated;
	std::array<PoolEntryUnion<T>, S> storage;

	template<typename ...Ts>
	constexpr EntryUnion& pop_from_deallocated(Ts &&...args)
	{
		auto &entry_union = deallocated.pop_front();

		entry_union.entry_empty.~EntryEmpty();

		std::construct_at(&entry_union.entry, std::forward<Ts>(args)...);

		return entry_union;
	}

public:
	constexpr Pool()
	{
		for (auto &entry : storage)
		{
			std::construct_at(&entry.entry_empty);
			deallocated.push_front(entry);
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
		allocated.push_front(entry);
		return entry.entry;
	}

	template<typename ...Ts>
	constexpr T& allocate_back(Ts &&...args)
	{
		auto &entry = pop_from_deallocated(std::forward<Ts>(args)...);
		allocated.push_back(entry);
		return entry.entry;
	}

	constexpr Common::Iterator deallocate(Common::Iterator it)
	{
		EntryUnion &entry_union = *it.GetHeldTypePointer();

		++it;

		// Remove from allocated object list.
		allocated.erase(entry_union);

		entry_union.entry.~Entry();

		// Add to deallocated object list.
		std::construct_at(&entry_union.entry_empty);
		deallocated.push_front(entry_union);

		return it;
	}

	constexpr Common::Iterator begin()
	{
		return typename Common::Iterator(allocated.begin().GetHeldTypePointer());
	}

	constexpr Common::Iterator end()
	{
		return typename Common::Iterator();
	}

	template<typename Self>
	constexpr T& front(this Self &self)
	{
		return self.allocated.front().entry;
	}

	template<typename Self>
	constexpr T& back(this Self &self)
	{
		return self.allocated.back().entry;
	}

	constexpr std::size_t size() const
	{
		return allocated.size();
	}

	constexpr std::size_t max_size() const
	{
		return S;
	}

	constexpr bool empty() const
	{
		return allocated.empty();
	}

	constexpr bool full() const
	{
		return deallocated.empty();
	}

	static constexpr Entry& GetEntryFromHeldType(EntryUnion &held)
	{
		return held.entry;
	}

	static constexpr T& DereferenceHeldType(EntryUnion &held)
	{
		return held.entry;
	}
};

#endif /* POOL_H */
