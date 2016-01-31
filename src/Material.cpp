#include "Material.h"

using namespace Phys;
using namespace Dojo;

Material::Material(const utf::string& name, const Table& desc, const_optional_ref<ResourceGroup> group) :
	name(name) {
	DEBUG_ASSERT(name.not_empty(), "Invalid name");

	density = desc.getNumber("density", density);
	friction = desc.getNumber("friction", friction);
	restitution = desc.getNumber("restitution", restitution);

	//load sounds if available
	if (auto g = group.cast()) {
		if (desc.existsAs("soundPrefix", Table::FieldType::String)) {
			auto prefix = desc.getString("soundPrefix");

			impactHard = g.get().getSound(prefix + "_impact_hard");
			impactSoft = g.get().getSound(prefix + "_impact_soft");
		}
	}
}
