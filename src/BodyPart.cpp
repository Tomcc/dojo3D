#include "stdafx.h"

#include "BodyPart.h"

using namespace Phys;

BodyPart::BodyPart(Body& parent, Unique<btCollisionShape> shapeDesc, float volume) :
parent(parent),
shape(std::move(shapeDesc)),
volume(volume) {
	DEBUG_ASSERT(shape, "Invalid shape");
	DEBUG_ASSERT(volume >= 0, "Invalid volume");
}