#include "bullet.h"

void Objects::Bullet::Update()
{
	position.x += x_velocity;

	QueueForDisplay(0, 0, 0, 0, {.priority = false, .palette_line = 0, .y_flip = false, .x_flip = false, .tile_index = 8});
}
