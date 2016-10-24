#pragma once
#ifdef ENABLE_OVR

#include <OVR_CAPI.h>
#include "dojo\FrameSubmitter.h"

namespace VR {
	class RiftSupport : public Dojo::FrameSubmitter
	{
	public:
		static Unique<RiftSupport> initRiftSupport();

		RiftSupport(ovrSession session, ovrGraphicsLuid luid);
		virtual ~RiftSupport();

		virtual void submitFrame() override;

	private:
		ovrSession session;
		ovrGraphicsLuid GPULuid;
		int64_t frameIndex = 0;

		ovrHmdDesc desc;
		ovrSizei resolution;
	};
}

#endif
