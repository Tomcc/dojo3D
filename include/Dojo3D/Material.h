#pragma once

#include "common_header.h"

namespace Phys {
	class Material {
	public:
		const utf::string name;
		float density = 1.f;

		//solid parameters
		float friction = 0.5f;
		float restitution = 0.f;

		Dojo::SoundSet
			*impactHard = nullptr,
			*impactSoft = nullptr;

		Material(const utf::string& name, const Dojo::Table& desc = Dojo::Table::Empty, const Dojo::ResourceGroup* group = nullptr);
	};
}



