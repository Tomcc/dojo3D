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
		//btRigidBody* hitRigidBody = nullptr;
		Group group = 0;
		float dist;

		explicit RayResult(const World& world) :
			world(&world) {

		}

		Body* getHitBody() const;

		const Material* getHitMaterial() const;

	protected:
		const World* world;
	};
}



