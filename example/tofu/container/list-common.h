#ifndef LIST_COMMON_H
#define LIST_COMMON_H

#include <cassert>

template<typename Derived, typename HeldType>
class ListCommon
{
public:
	class Iterator
	{
	private:
		HeldType *entry;

	public:
		constexpr Iterator(HeldType* const entry = nullptr)
			: entry(entry)
		{}

		constexpr Iterator& operator++()
		{
			entry = Derived::GetEntryFromHeldType(*entry).next;
			return *this;
		}

		constexpr Iterator operator++(int)
		{
			const auto old = *this;
			operator++();
			return old;
		}

		constexpr bool operator==(const Iterator &other) const
		{
			return entry == other.entry;
		}

		template<typename Self>
		constexpr auto* GetHeldTypePointer(this Self &&self)
		{
			return self.entry;
		}

		template<typename Self>
		constexpr auto* operator->(this Self &&self)
		{
			assert(self.entry != nullptr);
			return &Derived::DereferenceHeldType(*self.GetHeldTypePointer());
		}

		template<typename Self>
		constexpr auto& operator*(this Self &&self)
		{
			return *self.operator->();
		}
	};
};

#endif // LIST_COMMON_H
