#include "Body.h"

#include "BodyPart.h"
#include "PhysUtil.h"
#include "World.h"

using namespace Phys;

Body::Body(Dojo::Object& object, World& world, Group group, BodyType type /*= false*/) :
Component(object),
world(world),
group(group),
bodyType(type) {

}

Body::~Body() {

}

float Body::getMass() const {
	//TODO
	return 0; 
}

void Body::destroyPhysics() {

}

BodyPart& Body::addBoxShape(const Material& material, const Vector& dimensions, const Vector& center /*= Vector::Zero*/, bool sensor /*= false*/) {
	DEBUG_ASSERT(!isInitialized(), "Cannot add a shape when the object is active");

	DEBUG_ASSERT(center == Vector::Zero, "Offset not yet supported");

	float volume = dimensions.x * dimensions.y * dimensions.z;

	auto shape = make_unique<btBoxShape>( asBtVector( dimensions * 0.5f ) );

	auto part = make_unique<BodyPart>(*this, material, std::move(shape), volume);
	auto& ref = *part;
	parts.emplace(std::move(part));

	return ref;
}

void Body::onAttach() {
	DEBUG_ASSERT(parts.size() > 0, "At least one shape is needed to add a physics body");
	DEBUG_ASSERT(parts.size() <= 1, "Multishape not yet supported");

	//initialize the transform with the user-set Object transform
	worldTransform = btTransform{ asBtQuaternion(self.getRotation()), asBtVector(self.position) };

	//TODO compose compound shape if there are multiple parts
	auto& parentShape = parts[0]->getBtShape();

	//find mass 
	float mass = 0;
	btVector3 inertia = btVector3(0, 0, 0);
	if (bodyType == BodyType::Dynamic) { //0 mass means static
		for (auto&& part : parts) {
			mass += part->getMass();
		}
		parentShape.calculateLocalInertia(mass, inertia);
	}

	auto desc = btRigidBody::btRigidBodyConstructionInfo(mass, this, &parentShape, inertia);
	body = make_unique<btRigidBody>(desc);

	//add the body to the world
	world.addBody(*this);
}

void Body::getWorldTransform(btTransform& worldTrans) const {
	worldTrans = worldTransform;
}

void Body::setWorldTransform(const btTransform& worldTrans) {
	//NOTE: this method is only called when the object has moved

	worldTransform = worldTrans;

	//update the graphics too (todo: interpolate?)
	self.position = asVector(worldTrans.getOrigin());
 	self.setRotation(asQuaternion(worldTrans.getRotation()));
}
