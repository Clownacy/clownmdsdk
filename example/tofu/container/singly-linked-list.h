#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"

class SinglyLinkedListBase
{
protected:
	struct Entry
	{
		struct Entry *next;
	};

	Entry *head = nullptr;

	void PushFront(Entry *entry);
	Entry* PopFront();
	bool Empty() const;
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
			return static_cast<Common::template copy_const_t<Self, Derived>*>(static_cast<Common::template copy_const_t<Self, Entry>*>(&self)->next);
		}
	};

	void push_front(Derived &entry)
	{
		SinglyLinkedListBase::PushFront(&entry);
	}

	Derived& pop_front()
	{
		const auto pointer = SinglyLinkedListBase::PopFront();
		_assertm(pointer != nullptr, "Tried to pop empty list.");
		return *static_cast<Derived*>(static_cast<Entry*>(pointer));
	}

	template<typename Self>
	auto& front(this Self &&self)
	{
		_assertm(self.head != nullptr, "Tried to access first element of empty list.");
		return *static_cast<Common::template copy_const_t<Self, Derived>*>(static_cast<Common::template copy_const_t<Self, Entry>*>(self.head));
	}

	Common::Iterator begin()
	{
		return typename Common::Iterator(static_cast<Entry*>(head));
	}

	Common::Iterator end()
	{
		return typename Common::Iterator();
	}

	bool empty() const
	{
		return SinglyLinkedListBase::Empty();
	}
};

#endif /* SINGLY_LINKED_LIST_H */
