#include "Wheel.h"
#include "Body.h"
#include "PhysUtil.h"

using namespace Phys;

void Wheel::_init() {
	auto& vehicle = parent.getBtVehicle().unwrap();

	index = vehicle.getNumWheels();
	auto& wheel = vehicle.addWheel(
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

Wheel::Wheel(Body& parent, const Vector& connectionPoint, const Vector& direction, const Vector& axle, float radius, float suspensionRestLength, const btRaycastVehicle::btVehicleTuning& suspensionTuning) : parent(parent)
, connectionPoint(connectionPoint)
, direction(direction)
, axle(axle)
, radius(radius)
, suspensionRestLength(suspensionRestLength)
, suspensionTuning(suspensionTuning)
{
	DEBUG_ASSERT(direction != Vector::Zero, "Invalid direction");
	DEBUG_ASSERT(radius > 0, "Invalid radius");
	DEBUG_ASSERT(suspensionRestLength > 0, "Invalid rest length");
}

void Wheel::setSteering(Dojo::Radians angle) {
	if (mHasSteering) {
		parent.wakeUp();
		getInfo().unwrap().m_steering = angle;
	}
}

void Wheel::setBrake(float brake) {
	if (mHasBrakes) {
		parent.wakeUp();
		getInfo().unwrap().m_brake = brake;
	}
}

void Wheel::setEngineForce(float engineForce) {
	if (mHasEngine) {
		parent.wakeUp();
		getInfo().unwrap().m_engineForce = engineForce;
	}
}

void Wheel::_update() {
	auto& info = parent.getBtVehicle().unwrap().getWheelInfo(index);

	if (auto child = childObject.to_ref()) {
		auto& trans = info.m_worldTransform;
		child.get().position = asVector(trans.getOrigin());
		child.get().setRotation(asQuaternion(trans.getRotation()));
	}
}

optional_ref<btWheelInfo> Wheel::getInfo() {
	if (index >= 0) {
		return parent.getBtVehicle().unwrap().getWheelInfo(index);
	}
	return{};
}
