#pragma once

namespace Phys {
	class Body;
	class Material;

	class BodyPart {
	public:
		const float volume;

		BodyPart(Body& parent, const Phys::Material& material, Unique<btCollisionShape> shape, float volume);

		btCollisionShape& getBtShape() {
			return *shape;
		}

		float getMass() const;

	protected:
		Body& parent;
		const Phys::Material& material;
		Unique<btCollisionShape> shape;
	private:
	};
}

