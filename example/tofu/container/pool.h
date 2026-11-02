#ifndef POOL_H
#define POOL_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <type_traits>

#include "doubly-linked-list.h"
#include "singly-linked-list.h"

template<typename T, std::size_t S>
class Pool;

template<typename T, std::size_t S>
using PoolCommon = ListCommon<Pool<T, S>, T>;

template<typename T, std::size_t S>
class Pool : protected PoolCommon<T, S>
{
private:
	using Common = PoolCommon<T, S>;

	struct Entry : public DoublyLinkedList<Entry>::Entry, public T
	{
	public:
		using T::T;

		Entry* GetNext()
		{
			return DoublyLinkedList<Entry>::Entry::GetNext();
		}

		const auto& GetPoolEntry() const
		{
			return *this;
		}

		void SetPoolEntry(const Entry &other)
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

	std::array<AlignedUnion<Entry, EntryEmpty>, S> storage;

public:
	using Entry = Entry;

private:
	DoublyLinkedList<Entry> allocated;
	SinglyLinkedList<EntryEmpty> deallocated;

	template<typename ...Ts>
	Entry& pop_from_deallocated(Ts &&...args)
	{
		auto &entry = deallocated.pop_front();

		entry.~EntryEmpty();

		return *new(&entry) Entry(std::forward<Ts>(args)...);
	}

public:
	Pool()
	{
		for (auto &entry : storage)
			deallocated.push_front(*new(std::data(entry.bytes)) EntryEmpty());
	}

	~Pool()
	{
		for (auto it = begin(); it != end(); )
			it = deallocate(*it);
	}

	Pool(const Pool &other) = delete;
	Pool& operator=(const Pool &other) = delete;

	template<typename ...Ts>
	T& allocate_front(Ts &&...args)
	{
		auto &entry = pop_from_deallocated(std::forward<Ts>(args)...);
		allocated.push_front(entry);
		return entry;
	}

	template<typename ...Ts>
	T& allocate_back(Ts &&...args)
	{
		auto &entry = pop_from_deallocated(std::forward<Ts>(args)...);
		allocated.push_back(entry);
		return entry;
	}

	// TODO: Make this take an iterator, like a standard library container does.
	Common::Iterator deallocate(T &thing)
	{
		auto &entry = static_cast<Entry&>(thing);

		typename Common::Iterator next = entry.GetNext();

		// Remove from allocated object list.
		allocated.erase(entry);

		entry.~Entry();

		// Add to deallocated object list.
		deallocated.push_front(*new(&entry) EntryEmpty());

		return next;
	}

	Common::Iterator begin()
	{
		return typename Common::Iterator(allocated.begin().GetEntryPointer());
	}

	Common::Iterator end()
	{
		return typename Common::Iterator();
	}

	template<typename Self>
	auto& front(this Self &self)
	{
		return self.allocated.front();
	}

	template<typename Self>
	auto& back(this Self &self)
	{
		return self.allocated.back();
	}

	std::size_t length() const
	{
		return allocated.length();
	}

	bool empty() const
	{
		return allocated.empty();
	}

	bool full() const
	{
		return deallocated.empty();
	}
};

#endif /* POOL_H */
