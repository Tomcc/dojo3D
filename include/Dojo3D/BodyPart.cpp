#include "stdafx.h"

#include "BodyPart.h"

using namespace Phys;

BodyPart::BodyPart(Body& parent, const Material& material, Unique<btCollisionShape> shapeDesc) :
parent(parent),
material(material),
shape(std::move(shapeDesc)) {
	DEBUG_ASSERT(shape, "Invalid shape");
}
