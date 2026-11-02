#include "singly-linked-list.h"

void SinglyLinkedListBase::PushFront(Entry* const entry)
{
	entry->next = head;
	head = entry;
}

SinglyLinkedListBase::Entry* SinglyLinkedListBase::PopFront()
{
	Entry* const entry = head;

	if (entry != nullptr)
		head = entry->next;

	return entry;
}

bool SinglyLinkedListBase::Empty() const
{
	return head == nullptr;
}
