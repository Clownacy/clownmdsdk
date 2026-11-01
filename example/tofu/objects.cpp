#include "objects.h"

Objects::Pool Objects::pool;

void Objects::Update()
{
	for (auto it = pool.begin(); it != pool.end(); )
	{
		it->visit([&]<typename T>(T &&object)
		{
			if constexpr (!std::is_same_v<std::remove_cvref_t<T>, std::monostate>)
			{
				if (!object.Update())
				{
					// Destruct object.
					it = pool.deallocate(*it);
					return;
				}
			}

			++it;
		});
	}
}
