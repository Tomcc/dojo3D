#include "Wheel.h"
#include "Body.h"
#include "PhysUtil.h"

using namespace Phys;

void Wheel::_init() {
	auto vehicle = parent.getBtVehicle();
	DEBUG_ASSERT(vehicle, "The parent must have created the Vehicle at this point");

	index = vehicle->getNumWheels();
	auto& wheel = vehicle->addWheel(
		asBtVector(connectionPoint),
		asBtVector(direction),
		asBtVector(axle),
		suspensionRestLength,
		radius,
		suspensionTuning,
		true //does this do anything at all?
	);

	//TODO wtf is this stuff and why does it work with this numbers???

	float	wheelFriction = 1000;//BT_LARGE_FLOAT;
	float	suspensionStiffness = 20.f;
	float	suspensionDamping = 2.3f;
	float	suspensionCompression = 4.4f;
	float	rollInfluence = 0.1f;//1.0f;

	wheel.m_suspensionStiffness = suspensionStiffness;
	wheel.m_wheelsDampingRelaxation = suspensionDamping;
	wheel.m_wheelsDampingCompression = suspensionCompression;
	wheel.m_frictionSlip = wheelFriction;
	wheel.m_rollInfluence = rollInfluence;
}
