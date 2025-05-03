#ifndef OBJECTS_BASE_H
#define OBJECTS_BASE_H

#include <clownmdsdk.h>

#include "../coordinate.h"

using namespace ClownMDSDK::MainCPU;

namespace Objects
{
	class Base
	{
	public:
		Coordinate::World position;

		void QueueForDisplay(int x_offset, int y_offset, unsigned int width, unsigned int height, const VDP::VRAM::TileMetadata &tile_metadata);
	};
}

#endif // OBJECTS_BASE_H
