#include "doubly-linked-list.h"

void DoublyLinkedListBase::PushFront(Entry* const entry)
{
	if (head == nullptr)
		tail = entry;
	else
		head->previous = entry;

	entry->next = head;
	entry->previous = nullptr;

	head = entry;
}

void DoublyLinkedListBase::PushBack(Entry* const entry)
{
	if (tail == nullptr)
		head = entry;
	else
		tail->next = entry;

	entry->next = nullptr;
	entry->previous = tail;

	tail = entry;
}

void DoublyLinkedListBase::Remove(Entry* const entry)
{
	if (entry->previous == nullptr)
		head = entry->next;
	else
		entry->previous->next = entry->next;

	if (entry->next == nullptr)
		tail = entry->previous;
	else
		entry->next->previous = entry->previous;
}

std::size_t DoublyLinkedListBase::Length() const
{
	std::size_t length = 0;

	for (Entry *entry = head; entry != nullptr; entry = entry->next)
		++length;

	return length;
}

bool DoublyLinkedListBase::Empty() const
{
	return head == nullptr;
}
