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
	auto& front(this Self &&self)
	{
		_assertm(self.head != nullptr, "Tried to access first element of empty list.");
		return *self.head;
	}

	template<typename Self>
	auto& back(this Self &&self)
	{
		_assertm(self.tail != nullptr, "Tried to access last element of empty list.");
		return *self.tail;
	}

	void push_front(Entry &entry)
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

	void push_back(Entry &entry)
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

	Entry* erase(Entry &entry)
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

	Entry& pop_front()
	{
		auto &entry = front();
		erase(entry);
		return entry;
	}

	Entry& pop_back()
	{
		auto &entry = back();
		erase(entry);
		return entry;
	}

	std::size_t size() const
	{
		return total_entries;
	}

	bool empty() const
	{
		return head == nullptr;
	}
};

template<typename Derived>
class DoublyLinkedList : protected DoublyLinkedListBase, protected ListCommon<DoublyLinkedList<Derived>, Derived>
{
private:
	using Common = ListCommon<DoublyLinkedList<Derived>, Derived>;

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
		auto* GetNext(this Self &&self)
		{
			return static_cast<Common::template copy_const_t<Self, Derived>*>(static_cast<Common::template copy_const_t<Self, Entry>*>(&self)->next);
		}

		template<typename Self>
		auto* GetPrevious(this Self &&self)
		{
			return static_cast<Common::template copy_const_t<Self, Derived>*>(static_cast<Common::template copy_const_t<Self, Entry>*>(&self)->previous);
		}
	};

	template<typename Self>
	auto& front(this Self &&self)
	{
		return static_cast<Common::template copy_const_t<Self, Derived>&>(DoublyLinkedListBase::front(self));
	}

	template<typename Self>
	auto& back(this Self &&self)
	{
		return static_cast<Common::template copy_const_t<Self, Derived>&>(DoublyLinkedListBase::back(self));
	}

	Common::Iterator begin()
	{
		return typename Common::Iterator(static_cast<Entry*>(head));
	}

	Common::Iterator end()
	{
		return typename Common::Iterator();
	}

	void push_front(Derived &entry)
	{
		DoublyLinkedListBase::push_front(entry);
	}

	void push_back(Derived &entry)
	{
		DoublyLinkedListBase::push_back(entry);
	}

	Derived& pop_front()
	{
		return static_cast<Derived&>(DoublyLinkedListBase::pop_front());
	}

	Derived& pop_back()
	{
		return static_cast<Derived&>(DoublyLinkedListBase::pop_back());
	}

	Derived* erase(Entry &entry)
	{
		return static_cast<Derived*>(DoublyLinkedListBase::erase(entry));
	}

	std::size_t size() const
	{
		return DoublyLinkedListBase::size();
	}

	bool empty() const
	{
		return DoublyLinkedListBase::empty();
	}
};

#endif /* DOUBLY_LINKED_LIST_H */
