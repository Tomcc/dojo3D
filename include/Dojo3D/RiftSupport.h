#pragma once

#include <OVR_CAPI.h>

namespace VR {
	class RiftSupport
	{
	public:
		static Unique<RiftSupport> initRiftSupport();

		RiftSupport(ovrSession session, ovrGraphicsLuid luid);
		~RiftSupport();

	private:
		ovrSession session;
		ovrGraphicsLuid GPULuid;

		ovrHmdDesc desc;
		ovrSizei resolution;
	};
}

