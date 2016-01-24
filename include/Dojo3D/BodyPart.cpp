#include "stdafx.h"

#include "BodyPart.h"
#include "Material.h"

using namespace Phys;

BodyPart::BodyPart(Body& parent, const Material& material, Unique<btCollisionShape> shapeDesc, float volume) :
parent(parent),
material(material),
shape(std::move(shapeDesc)),
volume(volume) {
	DEBUG_ASSERT(shape, "Invalid shape");
	DEBUG_ASSERT(volume > 0, "Invalid volume");
}

float Phys::BodyPart::getMass() const {
	return volume * material.density;
}