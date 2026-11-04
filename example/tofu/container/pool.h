#ifndef POOL_H
#define POOL_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <type_traits>

#include "doubly-linked-list.h"
#include "singly-linked-list.h"

template<typename T>
struct PoolEntry : public DoublyLinkedList<PoolEntry<T>>::Entry, public T
{
public:
	using T::T;

	constexpr PoolEntry* GetNext()
	{
		return DoublyLinkedList<PoolEntry<T>>::Entry::next;
	}

	constexpr const auto& GetPoolEntry() const
	{
		return *this;
	}

	constexpr void SetPoolEntry(const PoolEntry &other)
	{
		*this = other;
	}
};

template<typename... Ts>
struct AlignedUnion
{
	alignas(Ts...) std::array<std::byte, std::max(sizeof(Ts)...)> bytes;
};

template<typename T>
class PoolEntryEmpty : public SinglyLinkedListLowLevel<AlignedUnion<PoolEntry<T>, PoolEntryEmpty<T>>, PoolEntryEmpty<T>>::Entry {};

template<typename T>
using PoolEntryUnion = AlignedUnion<PoolEntry<T>, PoolEntryEmpty<T>>;

template<typename T, std::size_t S>
class Pool;

template<typename T, std::size_t S>
using PoolCommon = ListCommon<Pool<T, S>, PoolEntry<T>>;

template<typename T, std::size_t S>
class Pool : protected PoolCommon<T, S>
{
public:
	using Entry = PoolEntry<T>;

private:
	using EntryEmpty = PoolEntryEmpty<T>;
	using EntryUnion = PoolEntryUnion<T>;
	using Common = PoolCommon<T, S>;

	class DeallocatedList : public SinglyLinkedListLowLevel<EntryUnion, DeallocatedList>
	{
	public:
		static constexpr SinglyLinkedListLowLevel<EntryUnion, EntryEmpty>::Entry& GetEntryFromHeldType(EntryUnion &held)
		{
			return reinterpret_cast<EntryEmpty&>(held);
		}
	};

	DoublyLinkedList<Entry> allocated;
	DeallocatedList deallocated;
	std::array<PoolEntryUnion<T>, S> storage;

	template<typename ...Ts>
	constexpr Entry& pop_from_deallocated(Ts &&...args)
	{
		auto &entry = deallocated.pop_front();

		reinterpret_cast<EntryEmpty&>(entry).~EntryEmpty();

		return *new(&entry) Entry(std::forward<Ts>(args)...);
	}

public:
	constexpr Pool()
	{
		for (auto &entry : storage)
		{
			new(&reinterpret_cast<EntryEmpty&>(entry)) EntryEmpty();
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
		return entry;
	}

	template<typename ...Ts>
	constexpr T& allocate_back(Ts &&...args)
	{
		auto &entry = pop_from_deallocated(std::forward<Ts>(args)...);
		allocated.push_back(entry);
		return entry;
	}

	constexpr Common::Iterator deallocate(Common::Iterator &thing)
	{
		auto &entry = static_cast<Entry&>(*thing);

		typename Common::Iterator next = entry.GetNext();

		// Remove from allocated object list.
		allocated.erase(entry);

		entry.~Entry();

		// Add to deallocated object list.
		auto &entry_union = reinterpret_cast<EntryUnion&>(entry);
		new(&entry_union) EntryEmpty();
		deallocated.push_front(entry_union);

		return next;
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
	constexpr auto& front(this Self &self)
	{
		return self.allocated.front();
	}

	template<typename Self>
	constexpr auto& back(this Self &self)
	{
		return self.allocated.back();
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

	static constexpr Entry& GetEntryFromHeldType(Entry &held)
	{
		return held;
	}

	static constexpr T& DereferenceHeldType(Entry &held)
	{
		return held;
	}
};

#endif /* POOL_H */
