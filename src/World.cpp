#include "World.h"

#include "PhysUtil.h"

Phys::World::World(const Vector& gravity) {
	/// collision configuration contains default setup for memory , collision setup . Advanced users can create their own configuration .
	collisionConfiguration = make_unique<btDefaultCollisionConfiguration>();
	/// use the default collision dispatcher . For parallel processing you can use a diffent dispatcher(see Extras / BulletMultiThreaded)
	dispatcher = make_unique<btCollisionDispatcher>(collisionConfiguration.get());
	
	/// btDbvtBroadphase is a good general purpose broadphase . You can also try out btAxis3Sweep .
	overlappingPairCache = make_unique<btDbvtBroadphase>();
	
	/// the default constraint solver . For parallel processing you can use a different solver (see Extras / BulletMultiThreaded)
	solver = make_unique<btSequentialImpulseConstraintSolver>();
	dynamicsWorld = make_unique<btDiscreteDynamicsWorld>(
		dispatcher.get(),
		overlappingPairCache.get(),
		solver.get(),
		collisionConfiguration.get()
	);
	
	dynamicsWorld->setGravity(asBtVector(gravity));
}

Phys::World::~World() {
	//ensure that the order of teardown is correct
	dynamicsWorld = {};
}
