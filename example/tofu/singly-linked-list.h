#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include <cassert>
#include <cstddef>
#include <type_traits>

#include "list-common.h"

#define MAKE_SINGLY_LINKED_LIST_ENTRY_TYPE(NAME, GETTER) \
template<typename Derived> \
class NAME : public SinglyLinkedList<Derived, NAME<Derived>>::Entry \
{ \
public: \
	auto& GETTER() \
	{ \
		return *this; \
	} \
}

class SinglyLinkedListBase
{
protected:
	struct Entry
	{
		struct Entry *next;
	};

	Entry *head = nullptr;
	Entry *tail = nullptr;

	void PushFront(Entry *entry);
	void PushBack(Entry *entry);
	Entry* PopFront();
	std::size_t Length() const;
	bool Empty() const;
};

template<typename BaseType, typename EntryType>
class SinglyLinkedList : protected SinglyLinkedListBase, protected ListCommon<SinglyLinkedList<BaseType, EntryType>, BaseType>
{
private:
	using Common = ListCommon<SinglyLinkedList<BaseType, EntryType>, BaseType>;

public:
	class Entry : protected SinglyLinkedListBase::Entry
	{
	public:
		friend SinglyLinkedList<BaseType, EntryType>;

		static constexpr auto GetNull()
		{
			return static_cast<BaseType*>(static_cast<SinglyLinkedListBase::Entry*>(nullptr));
		}

		template<typename Self>
		auto GetNext(this Self &self)
		{
			return static_cast<Common::template copy_const_t<Self, BaseType>*>(static_cast<Common::template copy_const_t<Self, Entry>*>(&self)->next);
		}
	};

	void push_front(BaseType &entry)
	{
		SinglyLinkedListBase::PushFront(&entry);
	}

	void push_back(BaseType &entry)
	{
		SinglyLinkedListBase::PushBack(&entry);
	}

	BaseType& pop_front()
	{
		const auto pointer = SinglyLinkedListBase::PopFront();
		_assertm(pointer != nullptr, "Tried to pop empty list!");
		return *static_cast<BaseType*>(static_cast<Entry*>(pointer));
	}

	template<typename Self>
	auto& front(this Self &self)
	{
		assert(self.head != nullptr);
		return *static_cast<Common::template copy_const_t<Self, BaseType>*>(static_cast<Common::template copy_const_t<Self, Entry>*>(self.head));
	}

	template<typename Self>
	auto& back(this Self &self)
	{
		assert(self.tail != nullptr);
		return *static_cast<Common::template copy_const_t<Self, BaseType>*>(static_cast<Common::template copy_const_t<Self, Entry>*>(self.tail));
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
		return SinglyLinkedListBase::Length();
	}

	bool empty() const
	{
		return SinglyLinkedListBase::Empty();
	}
};

#endif /* SINGLY_LINKED_LIST_H */
