#ifndef POOL_H
#define POOL_H

#include <cstddef>
#include <span>

#include "doubly-linked-list.h"

template<typename T>
class Pool : protected ListCommon<Pool<T>, T>
{
private:
	using Common = ListCommon<Pool<T>, T>;

public:
	MAKE_DOUBLY_LINKED_LIST_ENTRY_TYPE(ListEntryBase, GetListEntryBase);

	class Entry : public ListEntryBase<Entry>
	{
	public:
		friend Pool<T>;

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
	DoublyLinkedList<Entry, ListEntryBase<Entry>> allocated;
	DoublyLinkedList<Entry, ListEntryBase<Entry>> deallocated;

public:
	Pool(const std::span<T> &array)
	{
		for (auto &entry : array)
			deallocated.push_back(entry);
	}

	T& allocate_front()
	{
		Entry &entry = deallocated.pop_front();

		allocated.push_front(entry);

		return *static_cast<T*>(&entry);
	}

	T& allocate_back()
	{
		Entry &entry = deallocated.pop_back();

		allocated.push_back(entry);

		return *static_cast<T*>(&entry);
	}

	Common::Iterator deallocate(T &entry)
	{
		// Remove from allocated object list.
		const auto next = static_cast<T*>(allocated.erase(entry));

		// Add to deallocated object list.
		deallocated.push_front(entry);

		return typename Common::Iterator(next);
	}

	Common::Iterator begin()
	{
		if (empty())
			return typename Common::Iterator();

		return typename Common::Iterator(static_cast<Entry*>(&allocated.front()));
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
