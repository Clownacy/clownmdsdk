#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"

template<typename Derived, typename T>
class SinglyLinkedListLowLevel : protected ListCommon<Derived, T>
{
private:
	using Common = ListCommon<Derived, T>;

public:
	struct Entry
	{
		T *next;
	};

private:
	T *head = nullptr;

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

	constexpr void push_front(T &entry)
	{
		Derived::GetEntryFromHeldType(entry).next = head;
		head = &entry;
	}

	constexpr T& pop_front()
	{
		T &entry = front();
		head = Derived::GetEntryFromHeldType(entry).next;
		return entry;
	}

	constexpr bool empty() const
	{
		return head == nullptr;
	}
};

template<typename T>
class SinglyLinkedList;

template<typename T>
using SinglyLinkedListBase = SinglyLinkedListLowLevel<SinglyLinkedList<T>, T>;

template<typename T>
class SinglyLinkedList : public SinglyLinkedListBase<T>
{
public:
	using Base = SinglyLinkedListBase<T>;

	static constexpr Base::Entry& GetEntryFromHeldType(T &held)
	{
		return held;
	}

	static constexpr T& DereferenceHeldType(T &held)
	{
		return held;
	}
};

#endif /* SINGLY_LINKED_LIST_H */
