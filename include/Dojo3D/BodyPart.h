#pragma once

namespace Phys {
	class Body;
	class Material;

	class BodyPart {
	public:
		BodyPart(Body& parent, const Phys::Material& material, Unique<btCollisionShape> shape);

		const btCollisionShape& getBTShape() const {
			return *shape;
		}

	protected:
		Body& parent;
		const Phys::Material& material;
		Unique<btCollisionShape> shape;
	private:
	};
}

