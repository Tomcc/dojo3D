#pragma once

#include "dojo/Radians.h"

namespace Phys {
	class Body;

	class Wheel {
	public:
		Body& parent;

		const Vector connectionPoint, direction, axle;
		const float radius, suspensionRestLength;
		const btRaycastVehicle::btVehicleTuning suspensionTuning;

		optional_ref<Dojo::Object> childObject;
		float engineForce = 0.f;

		Wheel(
			Body& parent,
			const Vector& connectionPoint,
			const Vector& direction,
			const Vector& axle,
			float radius,
			float suspensionRestLength,
			const btRaycastVehicle::btVehicleTuning& suspensionTuning);

		void _init();

		void setSteering(Dojo::Radians angle);

		void setBrake(float brake);

		void setEngineForce(float engineForce);

		void setHasEngine(bool enable) {
			if (!enable) {
				setEngineForce(0);
			}
			mHasEngine = enable;
		}

		void setHasBrakes(bool enable) {
			if (!enable) {
				setBrake(0);
			}
			mHasBrakes = enable;
		}

		void setHasSteering(bool enable) {
			if (!enable) {
				setSteering(Dojo::Radians(0));
			}
			mHasSteering = enable;
		}

		void _update();

	protected:
		int index = -1;
		bool mHasEngine = false, mHasBrakes = false, mHasSteering = false;

		optional_ref<btWheelInfo> getInfo();
	};

}

