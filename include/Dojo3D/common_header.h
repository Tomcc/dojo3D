#pragma once

#include <dojo.h>

#include <unordered_set>
#include <mutex>
#include <future>

#pragma warning(push)
#pragma warning(disable: 4127) // possible loss of data

#include "btBulletDynamicsCommon.h" 

#pragma warning(pop)

namespace Phys {

	struct Group : public Dojo::PseudoEnumClass<uint16_t> {
		template<typename T>
		Group(T raw) : PseudoEnumClass(raw) {}
	};

	using Vector = Dojo::Vector;
	using Quaternion = Dojo::Quaternion;
	using Matrix = Dojo::Matrix;
	using Radians = Dojo::Radians;

	typedef std::lock_guard<std::mutex> ScopedLock;

	template<typename T>
	using optional_ref = Dojo::optional_ref<T>;
}
