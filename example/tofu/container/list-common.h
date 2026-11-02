#ifndef LIST_COMMON_H
#define LIST_COMMON_H

#include <cassert>

template<typename Derived, typename BaseType>
class ListCommon
{
public:
	template<typename Source, typename Destination>
	struct copy_const
	{
		using type = std::conditional_t<std::is_const_v<std::remove_reference_t<Source>>, const Destination, Destination>;
	};

	template<typename Source, typename Destination>
	using copy_const_t = typename copy_const<Source, Destination>::type;

	class Iterator
	{
	private:
		Derived::Entry *entry;

	public:
		constexpr Iterator(Derived::Entry* const entry = Derived::Entry::GetNull())
			: entry(entry)
		{}

		constexpr Iterator& operator++()
		{
			entry = entry->GetNext();
			return *this;
		}

		constexpr Iterator operator++(int)
		{
			const auto old = *this;
			operator++();
			return old;
		}

		constexpr bool operator!=(const Iterator &other) const
		{
			return entry != other.entry;
		}

		template<typename Self>
		constexpr auto* GetEntryPointer(this Self &&self)
		{
			return static_cast<copy_const_t<Self, BaseType>*>(self.entry);
		}

		template<typename Self>
		constexpr auto* operator->(this Self &&self)
		{
			assert(self.entry != Derived::Entry::GetNull());
			return self.GetEntryPointer();
		}

		template<typename Self>
		constexpr auto& operator*(this Self &&self)
		{
			return *self.operator->();
		}
	};
};

#endif // LIST_COMMON_H
