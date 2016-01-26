#include "Wheel.h"
#include "Body.h"
#include "PhysUtil.h"

using namespace Phys;

void Wheel::_init() {
	auto vehicle = parent.getBtVehicle();
	DEBUG_ASSERT(vehicle, "The parent must have created the Vehicle at this point");

	index = vehicle->getNumWheels();
	vehicle->addWheel(
		asBtVector(connectionPoint),
		asBtVector(direction),
		asBtVector(axle),
		suspensionRestLength,
		radius,
		suspensionTuning,
		true //does this do anything at all?
	);
}
