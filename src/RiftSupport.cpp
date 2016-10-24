#include "RiftSupport.h"

#ifdef ENABLE_OVR
using namespace VR;
using namespace Dojo;

Unique<RiftSupport> VR::RiftSupport::initRiftSupport() {
	ovrResult result = ovr_Initialize(nullptr);
	if (OVR_FAILURE(result))
		return{};

	ovrSession session;
	ovrGraphicsLuid luid;
	result = ovr_Create(&session, &luid);
	if (OVR_FAILURE(result))
	{
		ovr_Shutdown();
		return{};
	}

	return make_unique<RiftSupport>(session, luid);
}

VR::RiftSupport::RiftSupport(ovrSession session, ovrGraphicsLuid luid) 
: session(session)
, GPULuid(luid) {
	DEBUG_ASSERT(session, "Invalid Session");

	//register itself to handle frame swaps
	Platform::singleton().getRenderer().setFrameSubmitter(self);

	desc = ovr_GetHmdDesc(session);
	resolution = desc.Resolution;
}

VR::RiftSupport::~RiftSupport() {
	ovr_Destroy(session);
	ovr_Shutdown();
}

void VR::RiftSupport::submitFrame() {
	//ovr_SubmitFrame(session, frameIndex, )
	DEBUG_TODO;
}
#endif
