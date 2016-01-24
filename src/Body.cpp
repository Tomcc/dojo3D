#include "Body.h"

#include "BodyPart.h"
#include "PhysUtil.h"

using namespace Phys;

Body::Body(Dojo::Object& object, World& world, Group group, bool staticShape /*= false*/) :
Component(object),
world(world),
group(group),
staticShape(staticShape) {

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

	auto shape = make_unique<btBoxShape>( asBtVector( dimensions * 0.5f ) );

	auto part = make_unique<BodyPart>(*this, material, std::move(shape));
	auto& ref = *part;
	parts.emplace(std::move(part));

	return ref;
}

void Body::onAttach() {
	DEBUG_ASSERT(parts.size() > 0, "At least one shape is needed to add a physics body");
	DEBUG_ASSERT(parts.size() <= 1, "Multishape not yet supported");

	//initialize and add the body
	worldTransform = btTransform{ asBtQuaternion(self.getRotation()), asBtVector(self.position) };
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
