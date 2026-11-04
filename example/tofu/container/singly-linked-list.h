#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"

template<typename HeldType, typename Derived>
class SinglyLinkedListLowLevel : protected ListCommon<Derived, HeldType>
{
private:
	using Common = ListCommon<Derived, HeldType>;

public:
	class Entry
	{
	public:
		HeldType *next;

		static constexpr auto GetNull()
		{
			return static_cast<HeldType*>(nullptr);
		}
	};

private:
	HeldType *head = nullptr;

public:
	template<typename Self>
	constexpr auto& front(this Self &&self)
	{
		_assertm(self.head != nullptr, "Tried to access first element of empty list.");
		return *self.head;
	}

	constexpr Common::Iterator begin()
	{
		return typename Common::Iterator(head);
	}

	constexpr Common::Iterator end()
	{
		return typename Common::Iterator();
	}

	constexpr void push_front(HeldType &entry)
	{
		Derived::GetEntryFromHeldType(entry).next = head;
		head = &entry;
	}

	constexpr HeldType& pop_front()
	{
		HeldType &entry = front();
		head = Derived::GetEntryFromHeldType(entry).next;
		return entry;
	}

	constexpr bool empty() const
	{
		return head == nullptr;
	}
};

template<typename HeldType>
class SinglyLinkedList;

template<typename HeldType>
using SinglyLinkedListBase = SinglyLinkedListLowLevel<HeldType, SinglyLinkedList<HeldType>>;

template<typename HeldType>
class SinglyLinkedList : public SinglyLinkedListBase<HeldType>
{
public:
	using Base = SinglyLinkedListBase<HeldType>;

	static constexpr Base::Entry& GetEntryFromHeldType(HeldType &held)
	{
		return held;
	}

	static constexpr HeldType& DereferenceHeldType(HeldType &held)
	{
		return held;
	}
};

#endif /* SINGLY_LINKED_LIST_H */
