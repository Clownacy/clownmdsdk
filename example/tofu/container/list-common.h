#ifndef LIST_COMMON_H
#define LIST_COMMON_H

#include <cassert>

template<typename Held, typename Dereferenced>
class ListIterator
{
private:
	Held *entry;

public:
	using difference_type = std::ptrdiff_t;
	using value_type = Dereferenced;
	using held_type = Held;

	constexpr ListIterator(Held* const entry = nullptr)
		: entry(entry)
	{}

	constexpr auto& operator++(this auto &&self)
	{
		self.entry = self.Advance();
		return self;
	}

	constexpr auto operator++(this auto &&self, int)
	{
		auto old = self;
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
