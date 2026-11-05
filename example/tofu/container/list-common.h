#ifndef LIST_COMMON_H
#define LIST_COMMON_H

#include <cassert>

template<typename Held, typename Dereferenced>
class ListIterator
{
private:
	Held *entry;

public:
	constexpr ListIterator(Held* const entry = nullptr)
		: entry(entry)
	{}

	constexpr ListIterator& operator++(this auto &&self)
	{
		self.entry = self.Advance();
		return self;
	}

	constexpr ListIterator operator++(this auto &&self, int)
	{
		const auto old = self;
		self.operator++();
		return old;
	}

	constexpr bool operator==(const ListIterator &other) const
	{
		return entry == other.entry;
	}

	constexpr auto* GetHeldPointer()
	{
		return entry;
	}

	constexpr Dereferenced& operator*(this auto &&self)
	{
		assert(self.entry != nullptr);
		return self.Dereference();
	}

	constexpr Dereferenced* operator->(this auto &&self)
	{
		return &self.operator*();
	}
};

template<typename Held, typename Dereferenced>
using ListConstIterator = ListIterator<const Held, const Dereferenced>;

#endif // LIST_COMMON_H
