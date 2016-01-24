#include "Body.h"

#include "BodyPart.h"

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
	auto part = make_unique<BodyPart>();
	auto& ref = *part;
	parts.emplace(std::move(part));

	return ref;
}
