#pragma once

#include "common_header.h"

namespace Phys {
	class Body;
	class World;
	class Material;

	struct RayResult {
		bool hit = false;
		Vector position;
		Vector normal;
		optional_ref<Body> hitBody;
		Group group = 0;
		float dist = FLT_MAX;
	};
}



