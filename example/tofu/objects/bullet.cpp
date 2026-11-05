#include "bullet.h"

#include "../level-manager.h"

bool Objects::Bullet::Update()
{
	position.x += x_velocity;

	QueueForDisplay(0, 0, 0, 0, {.priority = false, .palette_line = 0, .y_flip = false, .x_flip = false, .tile_index = 16});

	// Disappear after a second.
	if (--life == 0)
		return false;

	// Disappear upon touching a block.
	if (level_manager.GetBlock(position) != 0)
		return false;

	return true;
}
