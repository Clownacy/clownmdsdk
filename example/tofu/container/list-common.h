#ifndef LIST_COMMON_H
#define LIST_COMMON_H

#include <cassert>

template<typename Held, typename Dereferenced>
class ListForwardIterator
{
private:
	Held *entry;

public:
	using difference_type = std::ptrdiff_t;
	using value_type = Dereferenced;
	using held_type = Held;

	constexpr ListForwardIterator(Held* const entry = nullptr)
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

	constexpr bool operator==(const ListForwardIterator &other) const
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
using ListConstForwardIterator = ListForwardIterator<const Held, const Dereferenced>;

template<typename Held, typename Dereferenced>
class ListBiDirectionalIterator : public ListForwardIterator<Held, Dereferenced>
{
public:
	using Base = ListForwardIterator<Held, Dereferenced>;
	using Base::Base;

	constexpr auto& operator--(this auto &&self)
	{
		self.entry = self.Regress();
		return self;
	}

	constexpr auto operator--(this auto &&self, int)
	{
		auto old = self;
		self.operator--();
		return old;
	}
};

template<typename Held, typename Dereferenced>
using ListConstBiDirectionalIterator = ListBiDirectionalIterator<const Held, const Dereferenced>;

#endif // LIST_COMMON_H
