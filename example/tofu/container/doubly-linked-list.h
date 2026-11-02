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

	void PushFront(Entry *entry);
	void PushBack(Entry *entry);
	void Remove(Entry *entry);
	std::size_t Size() const;
	bool Empty() const;
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

	void push_front(Derived &entry)
	{
		DoublyLinkedListBase::PushFront(&entry);
	}

	void push_back(Derived &entry)
	{
		DoublyLinkedListBase::PushBack(&entry);
	}

	Derived& pop_front()
	{
		auto &entry = front();
		erase(entry);
		return entry;
	}

	Derived& pop_back()
	{
		auto &entry = back();
		erase(entry);
		return entry;
	}

	Derived* erase(Entry &entry)
	{
		const auto next = static_cast<Derived*>(entry.GetNext());
		DoublyLinkedListBase::Remove(&entry);
		return next;
	}

	template<typename Self>
	auto& front(this Self &&self)
	{
		_assertm(self.head != nullptr, "Tried to access first element of empty list.");
		return *static_cast<Common::template copy_const_t<Self, Derived>*>(static_cast<Common::template copy_const_t<Self, Entry>*>(self.head));
	}

	template<typename Self>
	auto& back(this Self &&self)
	{
		_assertm(self.tail != nullptr, "Tried to access last element of empty list.");
		return *static_cast<Common::template copy_const_t<Self, Derived>*>(static_cast<Common::template copy_const_t<Self, Entry>*>(self.tail));
	}

	Common::Iterator begin()
	{
		return typename Common::Iterator(static_cast<Entry*>(head));
	}

	Common::Iterator end()
	{
		return typename Common::Iterator();
	}

	std::size_t size() const
	{
		return DoublyLinkedListBase::Size();
	}

	bool empty() const
	{
		return DoublyLinkedListBase::Empty();
	}
};

#endif /* DOUBLY_LINKED_LIST_H */
