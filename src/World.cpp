#include "World.h"

#include "PhysUtil.h"

float makeTimeStep(int stepsPerSecond) {
	DEBUG_ASSERT(stepsPerSecond > 0, "Invalid number of steps per second");
	return 1.f / stepsPerSecond;
}

Phys::World::World(const Vector& gravity, int stepsPerSecond) :
timeStep( makeTimeStep(stepsPerSecond) ) {
	/// collision configuration contains default setup for memory , collision setup . Advanced users can create their own configuration .
	collisionConfiguration = make_unique<btDefaultCollisionConfiguration>();
	/// use the default collision dispatcher . For parallel processing you can use a diffent dispatcher(see Extras / BulletMultiThreaded)
	dispatcher = make_unique<btCollisionDispatcher>(collisionConfiguration.get());
	
	/// btDbvtBroadphase is a good general purpose broadphase . You can also try out btAxis3Sweep .
	overlappingPairCache = make_unique<btDbvtBroadphase>();
	
	/// the default constraint solver . For parallel processing you can use a different solver (see Extras / BulletMultiThreaded)
	solver = make_unique<btSequentialImpulseConstraintSolver>();
	world = make_unique<btDiscreteDynamicsWorld>(
		dispatcher.get(),
		overlappingPairCache.get(),
		solver.get(),
		collisionConfiguration.get()
	);
	
	world->setGravity(asBtVector(gravity));

	//inter-thread communication
	commands = make_unique<Dojo::Pipe<Job>>();
	callbacks = make_unique<Dojo::Pipe<Command>>();
	deferredCollisions = make_unique<Dojo::Pipe<DeferredCollision>>();
	deferredSensorCollisions = make_unique<Dojo::Pipe<DeferredSensorCollision>>();

	thread = std::thread([=]() {
		Dojo::Timer timer;
		Job job;

		while (running) {

			//process all available commands
			while ((simulationPaused || timer.getElapsedTime() < timeStep) && commands->try_dequeue(job)) {
				job.command();

				if (job.callback) {
					callbacks->enqueue(std::move(job.callback));
				}
			}

			if (!simulationPaused && timer.getElapsedTime() >= timeStep) {
				timer.reset();
				world->stepSimulation((btScalar)timer.getElapsedTime(), 10, timeStep);

				for (auto&& listener : listeners) {
					listener->onPostSimulationStep();
				}
			}
			else {
				std::this_thread::yield();
			}
		}
	});
}

Phys::World::~World() {
	running = false;
	thread.join();
	//ensure that the order of teardown is correct
	world = {};
}
