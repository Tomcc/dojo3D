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

		void assignTerrainData(Shared<std::vector<float>> data) {
			terrainData = data;
		}

	protected:
		Body& parent;
		Unique<btCollisionShape> shape;
		Shared<std::vector<float>> terrainData;
	private:
	};
}

