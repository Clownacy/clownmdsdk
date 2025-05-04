#include "player.h"

#include <algorithm>

#include "../controller.h"
#include "../level.h"
#include "../objects.h"
#include "../sprite.h"

void Objects::Player::Update()
{
	constexpr unsigned long hitbox_radius_x =  4 * 0x10000;
	constexpr unsigned long hitbox_radius_y = 14 * 0x10000;

	// Templated for maximum inlining for maximum performance!
	// The 68000 has no cache, so unrolled code is always optimal.
	const auto DoCollision = [&]<bool vertical, bool flipped>
	{
		const auto offset = vertical ? (flipped ? hitbox_radius_y : -hitbox_radius_y) : (flipped ? hitbox_radius_x : -hitbox_radius_x);
		const Coordinate::Block lower(position + Coordinate::World(vertical ? -hitbox_radius_x     : offset, vertical ? offset : -hitbox_radius_y));
		const Coordinate::Block upper(position + Coordinate::World(vertical ?  hitbox_radius_x - 1 : offset, vertical ? offset :  hitbox_radius_y - 1));

		const bool collision = [&]()
		{
			for (Coordinate::Block block_position = lower; block_position.Dimension<!vertical>() <= upper.Dimension<!vertical>(); ++block_position.Dimension<!vertical>())
				if (Level::GetBlock(block_position) != 0)
					return true;

			return false;
		}();

		if (collision)
		{
			position.Dimension<vertical>() = Coordinate::World(lower + Coordinate::Block(vertical ? 0 : !flipped, vertical ? !flipped : 0)).Dimension<vertical>() - offset;

			if constexpr (vertical)
			{
				y_velocity = 0;

				if constexpr (flipped)
					on_ground = true;
			}
		}
	};

	const auto &controller = Controller::manager.GetControlPad(0);

	static constexpr auto step = 0x18000;

	// Handle horziontal collision and movement.
	if (controller.held.left)
	{
		position.x -= step;
		facing_left = true;
		DoCollision.template operator()<false, false>(); // C++ is so damn ugly, sometimes...
	}
	else if (controller.held.right)
	{
		position.x += step;
		facing_left = false;
		DoCollision.template operator()<false, true>();
	}

	// Vertical movement.
	position.y += y_velocity;

	on_ground = false;

	// Handle vertical collision.
	if (y_velocity < 0)
		DoCollision.template operator()<true, false>();
	else if (y_velocity > 0)
		DoCollision.template operator()<true, true>();

	if (on_ground && (controller.pressed.a || controller.pressed.c))
	{
		// Jump.
		y_velocity = -0x60000;
	}
	else if (!controller.held.a && !controller.held.c && y_velocity < 0)
	{
		// End jump prematurely.
		y_velocity = std::max(-0x8000L, y_velocity);
	}

	// Gravity.
	y_velocity += 0x5000;

	if (controller.pressed.b)
	{
		Objects::AllocateBack<Bullet>(position, facing_left ? -0x40000 : 0x40000);
	}

	// Updata camera.
	Level::camera = Coordinate::Pixel(position);

	// Restrict camera to bounds of level.
	Level::camera.x = std::clamp(Level::camera.x, Level::screen_size.x / 2, Level::level_width_in_blocks  * Coordinate::block_width_in_pixels  - Level::screen_size.x / 2);
	Level::camera.y = std::clamp(Level::camera.y, Level::screen_size.y / 2, Level::level_height_in_blocks * Coordinate::block_height_in_pixels - Level::screen_size.y / 2);

	// Centre camera on target.
	Level::camera.x -= Level::screen_size.x / 2;
	Level::camera.y -= Level::screen_size.y / 2;

	QueueForDisplay(facing_left ? -2 : 2, -2, 1, 3, {.priority = false, .palette_line = 0, .y_flip = false, .x_flip = facing_left, .tile_index = 8});
}
