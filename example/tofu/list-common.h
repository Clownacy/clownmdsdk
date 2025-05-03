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
		Iterator(Derived::Entry* const entry = nullptr)
			: entry(entry)
		{}

		Iterator& operator++()
		{
			entry = entry->GetNext();
			return *this;
		}

		Iterator operator++(int)
		{
			const auto old = *this;
			operator++();
			return old;
		}

		bool operator!=(Iterator &other) const
		{
			return entry != other.entry;
		}

		template<typename Self>
		auto& operator*(this Self &self)
		{
			assert(self.entry != nullptr);
			return *static_cast<copy_const_t<Self, BaseType>*>(self.entry);
		}
	};
};

#endif // LIST_COMMON_H
