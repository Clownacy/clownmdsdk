#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"

// Split from the templated class to minimise duplicate code across each specialisation.
class SinglyLinkedListBase
{
protected:
	struct Entry
	{
		struct Entry *next;
	};

	Entry *head = nullptr;

	template<typename Self>
	auto& front(this Self &&self)
	{
		_assertm(self.head != nullptr, "Tried to access first element of empty list.");
		return *self.head;
	}

	void push_front(Entry &entry)
	{
		entry.next = head;
		head = &entry;
	}

	Entry& pop_front()
	{
		Entry &entry = front();
		head = entry.next;
		return entry;
	}

	bool empty() const
	{
		return head == nullptr;
	}
};

template<typename Derived>
class SinglyLinkedList : protected SinglyLinkedListBase, protected ListCommon<SinglyLinkedList<Derived>, Derived>
{
private:
	using Common = ListCommon<SinglyLinkedList<Derived>, Derived>;

public:
	class Entry : protected SinglyLinkedListBase::Entry
	{
	public:
		friend SinglyLinkedList<Derived>;

		static constexpr auto GetNull()
		{
			return static_cast<Derived*>(static_cast<SinglyLinkedListBase::Entry*>(nullptr));
		}

		template<typename Self>
		auto* GetNext(this Self &&self)
		{
			return static_cast<Common::template copy_const_t<Self, Derived>*>(self.next);
		}
	};

	template<typename Self>
	auto& front(this Self &&self)
	{
		return static_cast<Common::template copy_const_t<Self, Derived>&>(SinglyLinkedListBase::front(self));
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
		SinglyLinkedListBase::push_front(entry);
	}

	Derived& pop_front()
	{
		return static_cast<Derived&>(SinglyLinkedListBase::pop_front());
	}

	bool empty() const
	{
		return SinglyLinkedListBase::empty();
	}
};

#endif /* SINGLY_LINKED_LIST_H */
