#ifndef LIST_COMMON_H
#define LIST_COMMON_H

#include <cassert>

template<typename Held, typename Dereferenced>
class ListForwardIterator
{
protected:
	Held *entry;

public:
	using difference_type = std::ptrdiff_t;
	using value_type = Dereferenced;
	using held_type = Held;

	explicit constexpr ListForwardIterator(Held* const entry = nullptr)
		: entry(entry)
	{}

	constexpr ListForwardIterator(const ListForwardIterator<std::remove_const_t<Held>, std::remove_const_t<Dereferenced>> &iterator)
		: ListForwardIterator(iterator.GetHeldPointer())
	{}

	constexpr auto&& operator++(this auto &&self)
	{
		self.entry = self.Advance();
		return std::forward<decltype(self)>(self);
	}

	constexpr auto operator++(this auto &&self, int)
	{
		auto old = self;
		++self;
		return old;
	}

	constexpr bool operator==(const ListForwardIterator &other) const
	{
		return entry == other.entry;
	}

	constexpr auto* GetHeldPointer() const
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

	constexpr auto&& operator--(this auto &&self)
	{
		self.entry = self.Regress();
		return std::forward<decltype(self)>(self);
	}

	constexpr auto operator--(this auto &&self, int)
	{
		auto old = self;
		--self;
		return old;
	}
};

template<typename Held, typename Dereferenced>
using ListConstBiDirectionalIterator = ListBiDirectionalIterator<const Held, const Dereferenced>;

#endif // LIST_COMMON_H
