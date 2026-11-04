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

class EntryEmpty : public SinglyLinkedList<EntryEmpty>::Entry {};

template<typename T>
using PoolEntryUnion = AlignedUnion<PoolEntry<T>, EntryEmpty>;

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
	using Common = PoolCommon<T, S>;

	DoublyLinkedList<Entry> allocated;
	SinglyLinkedList<EntryEmpty> deallocated;
	std::array<PoolEntryUnion<T>, S> storage;

	template<typename ...Ts>
	constexpr Entry& pop_from_deallocated(Ts &&...args)
	{
		auto &entry = deallocated.pop_front();

		entry.~EntryEmpty();

		return *new(&entry) Entry(std::forward<Ts>(args)...);
	}

public:
	constexpr Pool()
	{
		for (auto &entry : storage)
			deallocated.push_front(*new(std::data(entry.bytes)) EntryEmpty());
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
		deallocated.push_front(*new(&entry) EntryEmpty());

		return next;
	}

	constexpr Common::Iterator begin()
	{
		return typename Common::Iterator(allocated.begin().GetEntryPointer());
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

	static constexpr Entry& GetEntry(Entry &held)
	{
		return held;
	}
};

#endif /* POOL_H */
