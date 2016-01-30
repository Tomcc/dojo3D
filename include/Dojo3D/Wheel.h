#pragma once

namespace Phys {
	class Body;

	class Wheel {
		Body& parent;

		const Vector connectionPoint, direction, axle;
		const float radius, suspensionRestLength;
		const btRaycastVehicle::btVehicleTuning suspensionTuning;

	public:
		Dojo::Object* childObject = nullptr;

		Wheel(
			Body& parent,
			const Vector& connectionPoint,
			const Vector& direction,
			const Vector& axle,
			float radius,
			float suspensionRestLength,
			const btRaycastVehicle::btVehicleTuning& suspensionTuning) 
		: parent(parent)
		, connectionPoint(connectionPoint)
		, direction(direction)
		, axle(axle)
		, radius(radius)
		, suspensionRestLength(suspensionRestLength)
		, suspensionTuning(suspensionTuning) {
			DEBUG_ASSERT(direction != Vector::Zero, "Invalid direction");
			DEBUG_ASSERT(radius > 0, "Invalid radius");
			DEBUG_ASSERT(suspensionRestLength > 0, "Invalid rest length");
		}

		void _init();
	protected:
		int index = -1;
	};

}

