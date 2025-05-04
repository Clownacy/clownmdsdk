#include "objects.h"

std::array<Objects::PoolEntry, 0x10> Objects::pool_buffer;
Pool<Objects::PoolEntry> Objects::pool(pool_buffer);

void Objects::Update()
{
	for (auto &entry : pool)
	{
		entry.visit([]<typename T>(T &&object)
		{
			if constexpr (!std::is_same_v<std::remove_cvref_t<T>, std::monostate>)
				object.Update();
		});
	}
}
