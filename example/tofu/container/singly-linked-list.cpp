#include "singly-linked-list.h"

void SinglyLinkedListBase::PushFront(Entry* const entry)
{
	if (head == nullptr)
		tail = entry;

	entry->next = head;

	head = entry;
}

void SinglyLinkedListBase::PushBack(Entry* const entry)
{
	if (tail == nullptr)
		head = entry;
	else
		tail->next = entry;

	entry->next = nullptr;

	tail = entry;
}

SinglyLinkedListBase::Entry* SinglyLinkedListBase::PopFront()
{
	Entry* const entry = head;

	if (entry != nullptr)
		head = entry->next;

	return entry;
}

std::size_t SinglyLinkedListBase::Length() const
{
	std::size_t length = 0;

	for (Entry *entry = head; entry != nullptr; entry = entry->next)
		++length;

	return length;
}

bool SinglyLinkedListBase::Empty() const
{
	return head == nullptr;
}
