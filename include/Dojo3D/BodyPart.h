#pragma once

namespace Phys {
	class Body;
	class Material;

	class BodyPart {
	public:
		const float volume;

		BodyPart(Body& parent, Unique<btCollisionShape> shape, float volume);

		btCollisionShape& getBtShape() {
			return *shape;
		}

	protected:
		Body& parent;
		Unique<btCollisionShape> shape;
	private:
	};
}

