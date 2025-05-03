#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"

#define MAKE_DOUBLY_LINKED_LIST_ENTRY_TYPE(NAME, GETTER) \
template<typename Derived> \
class NAME : public DoublyLinkedList<Derived, NAME<Derived>>::Entry \
{ \
public: \
	auto& GETTER() \
	{ \
		return *this; \
	} \
}

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

	void PushFront(Entry *entry);
	void PushBack(Entry *entry);
	void Remove(Entry *entry);
	std::size_t Length() const;
	bool Empty() const;
};

template<typename BaseType, typename EntryType>
class DoublyLinkedList : protected DoublyLinkedListBase, protected ListCommon<DoublyLinkedList<BaseType, EntryType>, BaseType>
{
private:
	using Common = ListCommon<DoublyLinkedList<BaseType, EntryType>, BaseType>;

public:
	class Entry : protected DoublyLinkedListBase::Entry
	{
	public:
		friend DoublyLinkedList<BaseType, EntryType>;

		template<typename Self>
		auto GetNext(this Self &self)
		{
			return static_cast<Common::template copy_const_t<Self, BaseType>*>(static_cast<Common::template copy_const_t<Self, EntryType>*>(&self)->next);
		}

		template<typename Self>
		auto GetPrevious(this Self &self)
		{
			return static_cast<Common::template copy_const_t<Self, BaseType>*>(static_cast<Common::template copy_const_t<Self, EntryType>*>(&self)->previous);
		}
	};

	void push_front(BaseType &entry)
	{
		DoublyLinkedListBase::PushFront(&entry);
	}

	void push_back(BaseType &entry)
	{
		DoublyLinkedListBase::PushBack(&entry);
	}

	void erase(BaseType &entry)
	{
		DoublyLinkedListBase::Remove(&entry);
	}

	void erase(Entry &entry)
	{
		erase(*entry);
	}

	template<typename Self>
	auto& front(this Self &self)
	{
		assert(self.head != nullptr);
		return *static_cast<Common::template copy_const_t<Self, BaseType>*>(static_cast<Common::template copy_const_t<Self, EntryType>*>(self.head));
	}

	template<typename Self>
	auto& back(this Self &self)
	{
		assert(self.tail != nullptr);
		return *static_cast<Common::template copy_const_t<Self, BaseType>*>(static_cast<Common::template copy_const_t<Self, EntryType>*>(self.tail));
	}

	Common::Iterator begin()
	{
		return typename Common::Iterator(static_cast<Entry*>(head));
	}

	Common::Iterator end()
	{
		return typename Common::Iterator();
	}

	std::size_t length() const
	{
		return DoublyLinkedListBase::Length();
	}

	bool empty() const
	{
		return DoublyLinkedListBase::Empty();
	}
};

#endif /* DOUBLY_LINKED_LIST_H */
