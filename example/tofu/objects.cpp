#include "objects.h"

std::array<Objects::PoolEntry, 0x10> Objects::pool_buffer;
Pool<Objects::PoolEntry> Objects::pool(pool_buffer);

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
					it->emplace<std::monostate>();

					it = pool.deallocate(*it);
					return;
				}
			}

			++it;
		});
	}
}
