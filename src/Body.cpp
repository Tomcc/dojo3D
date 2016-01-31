#include "Body.h"

#include "BodyPart.h"
#include "PhysUtil.h"
#include "World.h"
#include "Material.h"
#include "Wheel.h"

using namespace Phys;

Body::Body(Dojo::Object& object, World& world, const Material& material, Group group, BodyType type /*= false*/) :
Component(object),
world(world),
group(group),
bodyType(type),
material(material) {

}

Body::~Body() {

}

float Body::getMass() const {
	float mass = 0;
	if (bodyType == BodyType::Dynamic) {
		for (auto&& part : parts) {
			mass += part->volume * material.density;
		}
	}
	return mass;
}

void Body::destroyPhysics() {
	DEBUG_TODO;
}

BodyPart& Body::_addShape(Unique<btCollisionShape> shape, float volume, const Vector& offset, const Quaternion& rotation) {
	DEBUG_ASSERT(!isInitialized(), "Cannot add a shape when the object is active");

	DEBUG_ASSERT(offset == Vector::Zero, "Offset not yet supported");
	DEBUG_ASSERT(rotation == Quaternion(), "Rotation not yet supported");

	auto part = make_unique<BodyPart>(*this, std::move(shape), volume);
	auto& ref = *part;
	parts.emplace(std::move(part));

	return ref;
}

BodyPart& Body::addBoxShape(const Vector& dimensions, const Vector& center, const Quaternion& rotation) {
	return _addShape(
		make_unique<btBoxShape>(asBtVector(dimensions * 0.5f)),
		dimensions.x * dimensions.y * dimensions.z,
		center,
		rotation
	);
}

BodyPart& Body::addConvexHullShape(const std::vector<Vector>& points, const Vector& center, const Quaternion& rotation) {
	return _addShape(
		make_unique<btConvexHullShape>((const btScalar*)points.data(), points.size()),
		Dojo::AABB::fromPoints(points).getVolume(), // TODO actually make it correct
		center,
		rotation
	);
}

BodyPart& Body::addSphereShape(float radius, const Vector& center) {
	return _addShape(
		make_unique<btSphereShape>(radius),
		(4.f/3.f) * Dojo::Math::PI * radius * radius * radius,
		center,
		{}
	);
}


void Body::onAttach() {
	DEBUG_ASSERT(parts.size() > 0, "At least one shape is needed to add a physics body");
	DEBUG_ASSERT(parts.size() <= 1, "Multishape not yet supported");

	world.asyncCommand([this] {
		//TODO compose compound shape if there are multiple parts
		auto& parentShape = parts[0]->getBtShape();

		//find mass 
		btVector3 inertia = btVector3(0, 0, 0);
		float mass = getMass();
		if (bodyType == BodyType::Dynamic) {
			parentShape.calculateLocalInertia(mass, inertia);
		}

		//setup the object
		auto desc = btRigidBody::btRigidBodyConstructionInfo(mass, nullptr, &parentShape, inertia);

		//initialize the transform with the user-set Object transform
		desc.m_startWorldTransform = btTransform{ asBtQuaternion(self.getRotation()), asBtVector(self.position) };
		desc.m_friction = material.friction;
		desc.m_restitution = material.restitution;

		body = make_unique<btRigidBody>(desc);

		//TODO group & mask
		world.getBtWorld().addRigidBody(body.get());

		//configure the wheels
		if (wheels.size() > 0) {
			vehicleRaycaster = make_unique<btDefaultVehicleRaycaster>(&world.getBtWorld());

			btRaycastVehicle::btVehicleTuning tuning; //TODO what's this even used for, each wheel has it already
			vehicle = make_unique<btRaycastVehicle>(tuning, getBtBody(), vehicleRaycaster.get());
			world.getBtWorld().addVehicle(vehicle.get());

			vehicle->setCoordinateSystem(0, 1, 2);

			for (auto&& wheel : wheels) {
				wheel->_init();
			}
		}

		//add the body to the world
		world.registerBody(*this);
	});
}

void Body::_postSimulation() {
	//update the graphics too (todo: interpolate?)
	auto& worldTrans = body->getWorldTransform();
	self.position = asVector(worldTrans.getOrigin());
	self.setRotation(asQuaternion(worldTrans.getRotation()));

	for(auto i : range(wheels.size())) {
		vehicle->updateWheelTransform(i, true);
		auto& wheel = wheels[i];
		if (wheel->childObject) {
			auto& trans = vehicle->getWheelInfo(i).m_worldTransform;
			wheel->childObject->position = asVector(trans.getOrigin());
			wheel->childObject->setRotation(asQuaternion(trans.getRotation()));
		}
	}
}

Wheel& Body::addWheel( const Vector& connectionPoint, const Vector& direction, const Vector& axle, float radius, float suspensionRestLength, const btRaycastVehicle::btVehicleTuning& suspensionTuning ) {
	DEBUG_ASSERT(!isInitialized(), "Cannot add a wheel to a vehicle that is initialized");
	
	auto wheel = make_unique<Wheel>(
		*this,
		connectionPoint,
		direction,
		axle,
		radius,
		suspensionRestLength,
		suspensionTuning);

	auto& wref = *wheel;
	wheels.emplace(std::move(wheel));
	return wref;
}

void Body::setSleepingEnabled(bool enabled) {
	world.asyncCommand([this, enabled] {
		body->setActivationState(enabled ? ACTIVE_TAG : DISABLE_DEACTIVATION);
	});
}
