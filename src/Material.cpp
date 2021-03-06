#include "Material.h"

using namespace Phys;

Material::Material(const utf::string& name, const Dojo::Table& desc, optional_ref<const Dojo::ResourceGroup> group) :
	name(name) {
	DEBUG_ASSERT(name.not_empty(), "Invalid name");

	density = desc.getNumber("density", density);
	friction = desc.getNumber("friction", friction);
	restitution = desc.getNumber("restitution", restitution);

	//load sounds if available
	if (auto g = group.to_ref()) {
		if (desc.existsAs("soundPrefix", Dojo::Table::FieldType::String)) {
			auto prefix = desc.getString("soundPrefix");

			impactHard = g.get().getSound(prefix + "_impact_hard");
			impactSoft = g.get().getSound(prefix + "_impact_soft");
		}
	}
}
