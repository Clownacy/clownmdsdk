#include "object-manager.h"

ObjectManager object_manager;

void ObjectManager::Update()
{
	for (auto it = pool.begin(); it != pool.end(); )
	{
		it->visit(
			[&](auto &object)
			{
				if (!object.Update())
				{
					// Destruct object and move onto the next.
					it = pool.deallocate(it);
				}
				else
				{
					// Next object.
					++it;
				}
			}
		);
	}
}
