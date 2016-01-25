#include "Body.h"

#include "BodyPart.h"
#include "PhysUtil.h"
#include "World.h"
#include "Material.h"

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

}

BodyPart& Body::addBoxShape(const Vector& dimensions, const Vector& center /*= Vector::Zero*/, bool sensor /*= false*/) {
	DEBUG_ASSERT(!isInitialized(), "Cannot add a shape when the object is active");

	DEBUG_ASSERT(center == Vector::Zero, "Offset not yet supported");

	float volume = dimensions.x * dimensions.y * dimensions.z;

	auto shape = make_unique<btBoxShape>( asBtVector( dimensions * 0.5f ) );

	auto part = make_unique<BodyPart>(*this, std::move(shape), volume);
	auto& ref = *part;
	parts.emplace(std::move(part));

	return ref;
}

void Body::onAttach() {
	DEBUG_ASSERT(parts.size() > 0, "At least one shape is needed to add a physics body");
	DEBUG_ASSERT(parts.size() <= 1, "Multishape not yet supported");

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

	//add the body to the world
	world.addBody(*this);
}

void Body::_postSimulation() {
	//update the graphics too (todo: interpolate?)
	auto& worldTrans = body->getWorldTransform();
	self.position = asVector(worldTrans.getOrigin());
	self.setRotation(asQuaternion(worldTrans.getRotation()));
}
