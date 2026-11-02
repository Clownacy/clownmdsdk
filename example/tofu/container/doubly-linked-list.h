#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"

class DoublyLinkedListBase
{
public:
	struct Entry
	{
		struct Entry *next;
		struct Entry *previous;
	};

	Entry *head = nullptr;
	Entry *tail = nullptr;
	std::size_t total_entries = 0;

	template<typename Self>
	constexpr auto& front(this Self &&self)
	{
		_assertm(self.head != nullptr, "Tried to access first element of empty list.");
		return *self.head;
	}

	template<typename Self>
	constexpr auto& back(this Self &&self)
	{
		_assertm(self.tail != nullptr, "Tried to access last element of empty list.");
		return *self.tail;
	}

	constexpr void push_front(Entry &entry)
	{
		++total_entries;

		if (head == nullptr)
			tail = &entry;
		else
			head->previous = &entry;

		entry.next = head;
		entry.previous = nullptr;

		head = &entry;
	}

	constexpr void push_back(Entry &entry)
	{
		++total_entries;

		if (tail == nullptr)
			head = &entry;
		else
			tail->next = &entry;

		entry.next = nullptr;
		entry.previous = tail;

		tail = &entry;
	}

	constexpr Entry* erase(Entry &entry)
	{
		--total_entries;

		Entry* const next = entry.next;

		if (entry.previous == nullptr)
			head = entry.next;
		else
			entry.previous->next = entry.next;

		if (entry.next == nullptr)
			tail = entry.previous;
		else
			entry.next->previous = entry.previous;

		return next;
	}

	constexpr Entry& pop_front()
	{
		auto &entry = front();
		erase(entry);
		return entry;
	}

	constexpr Entry& pop_back()
	{
		auto &entry = back();
		erase(entry);
		return entry;
	}

	constexpr std::size_t size() const
	{
		return total_entries;
	}

	constexpr bool empty() const
	{
		return head == nullptr;
	}
};

template<typename Derived>
class DoublyLinkedList : protected ListCommon<DoublyLinkedList<Derived>, Derived>
{
private:
	using Common = ListCommon<DoublyLinkedList<Derived>, Derived>;

	DoublyLinkedListBase base;

public:
	class Entry : protected DoublyLinkedListBase::Entry
	{
	public:
		friend DoublyLinkedList<Derived>;

		static constexpr auto GetNull()
		{
			return static_cast<Derived*>(static_cast<DoublyLinkedListBase::Entry*>(nullptr));
		}

		template<typename Self>
		constexpr auto* GetNext(this Self &&self)
		{
			return static_cast<Common::template copy_const_t<Self, Derived>*>(static_cast<Common::template copy_const_t<Self, Entry>*>(&self)->next);
		}

		template<typename Self>
		constexpr auto* GetPrevious(this Self &&self)
		{
			return static_cast<Common::template copy_const_t<Self, Derived>*>(static_cast<Common::template copy_const_t<Self, Entry>*>(&self)->previous);
		}
	};

	template<typename Self>
	constexpr auto& front(this Self &&self)
	{
		return static_cast<Common::template copy_const_t<Self, Derived>&>(self.base.front());
	}

	template<typename Self>
	constexpr auto& back(this Self &&self)
	{
		return static_cast<Common::template copy_const_t<Self, Derived>&>(self.base.back());
	}

	constexpr Common::Iterator begin()
	{
		return typename Common::Iterator(static_cast<Entry*>(base.head));
	}

	constexpr Common::Iterator end()
	{
		return typename Common::Iterator();
	}

	constexpr void push_front(Derived &entry)
	{
		base.push_front(entry);
	}

	constexpr void push_back(Derived &entry)
	{
		base.push_back(entry);
	}

	constexpr Derived& pop_front()
	{
		return static_cast<Derived&>(base.pop_front());
	}

	constexpr Derived& pop_back()
	{
		return static_cast<Derived&>(base.pop_back());
	}

	constexpr Derived* erase(Entry &entry)
	{
		return static_cast<Derived*>(base.erase(entry));
	}

	constexpr std::size_t size() const
	{
		return base.size();
	}

	constexpr bool empty() const
	{
		return base.empty();
	}
};

#endif /* DOUBLY_LINKED_LIST_H */
