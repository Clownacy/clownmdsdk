#ifndef POOL_H
#define POOL_H

#include <array>
#include <cstddef>
#include <optional>
#include <span>

#include "doubly-linked-list.h"

template<typename T, std::size_t count>
class Pool;

template<typename T, std::size_t count>
using PoolCommon = ListCommon<Pool<T, count>, std::optional<T>>;

template<typename T, std::size_t count>
class Pool : protected PoolCommon<T, count>
{
private:
	using Common = PoolCommon<T, count>;

public:
	MAKE_DOUBLY_LINKED_LIST_ENTRY_TYPE(ListEntryBase, GetListEntryBase);

	class Entry : public std::optional<T>, public ListEntryBase<Entry>
	{
	public:
		friend Pool<T, count>;

		const auto& GetPoolEntry() const
		{
			return *this;
		}

		void SetPoolEntry(const Entry &other)
		{
			*this = other;
		}
	};

private:
	std::array<Entry, count> entry_buffer;

	DoublyLinkedList<Entry, ListEntryBase<Entry>> allocated;
	DoublyLinkedList<Entry, ListEntryBase<Entry>> deallocated;

public:
	Pool()
	{
		for (auto &entry : entry_buffer)
			deallocated.push_back(entry);
	}

	template<typename... Args>
	T& allocate_front(Args &&...args)
	{
		Entry &entry = deallocated.pop_front();

		allocated.push_front(entry);

		return *static_cast<T*>(&entry.emplace(std::forward<Args>(args)...));
	}

	template<typename... Args>
	T& allocate_back(Args &&...args)
	{
		Entry &entry = deallocated.pop_back();

		allocated.push_back(entry);

		return *static_cast<T*>(&entry.emplace(std::forward<Args>(args)...));
	}

	Common::Iterator deallocate(Common::Iterator &iterator)
	{
		// Remove from allocated object list.
		auto* const next = allocated.erase(static_cast<Entry&>(*iterator));

		*iterator = std::nullopt;

		// Add to deallocated object list.
		deallocated.push_front(static_cast<Entry&>(*iterator));

		return typename Common::Iterator(next);
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
