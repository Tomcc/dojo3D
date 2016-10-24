#include "World.h"

#include "PhysUtil.h"
#include "Body.h"

using namespace Phys;

float makeTimeStep(int stepsPerSecond) {
	DEBUG_ASSERT(stepsPerSecond > 0, "Invalid number of steps per second");
	return 1.f / stepsPerSecond;
}

World::World(const Vector& gravity, int stepsPerSecond, int collisionIterations) :
timeStep( makeTimeStep(stepsPerSecond) ) {
	DEBUG_ASSERT(collisionIterations > 0, "Invalid iterations");

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
	world->getSolverInfo().m_numIterations = collisionIterations;
	world->setGravity(asBtVector(gravity));

	//inter-thread communication
	commands = make_unique<Dojo::SPSCQueue<Job>>();
	callbacks = make_unique<Dojo::SPSCQueue<Command>>();
	deferredCollisions = make_unique<Dojo::SPSCQueue<DeferredCollision>>();
	deferredSensorCollisions = make_unique<Dojo::SPSCQueue<DeferredSensorCollision>>();

	thread = std::thread([=]() {
		Dojo::Timer timer;
		Job job;
		simulationPaused = false;

		while (running) {

			//process all available commands
			while ((simulationPaused || timer.getElapsedTime() < timeStep) && commands->try_dequeue(job)) {
				job.command();

				if (job.callback) {
					callbacks->enqueue(std::move(job.callback));
				}
			}

			world->stepSimulation((btScalar)timer.getAndReset(), 10, timeStep);

			for (auto& body : bodies) {
				auto state = body->getBtBody().unwrap().getActivationState();
				if (state != ISLAND_SLEEPING && state != DISABLE_SIMULATION) {
					body->_postSimulation();
				}
			}
		}
	});
}

World::~World() {
	running = false;
	thread.join();
	//ensure that the order of teardown is correct
	world = {};
}

bool World::isWorkerThread() const {
	return std::this_thread::get_id() == thread.get_id();
}

void World::asyncCommand(Command command, const Command& callback /*= Command()*/) const {
	DEBUG_ASSERT(command, "Command can't be a NOP");

	if (isWorkerThread()) {
		command();

		if (callback) {
			callbacks->enqueue(callback);
		}
	}
	else {
		commands->enqueue(std::move(command), callback);
	}
}

void World::asyncCallback(const Command& callback) const {
	DEBUG_ASSERT(callback, "Command can't be a NOP");

	if (isWorkerThread()) {
		callbacks->enqueue(callback);
	}
	else {
		callback();
	}
}

void World::sync() const {
	if (!isWorkerThread()) {
		std::atomic<bool> done = false;
		asyncCommand([&] {
			done = true;
		});

		while (!done) {
			std::this_thread::yield();
		}
	}
}


void World::registerBody(Body& body) {
	bodies.emplace(&body);
}

std::future<RayResult> Phys::World::raycast(const Vector& start, const Vector& end, Phys::Group rayBelongsToGroup /*= 0*/) const {
	auto promise = make_shared<std::promise<RayResult>>();

	auto future = promise->get_future();
	asyncCommand([this, start, end, promise] {
		auto result = RayResult{};

		btCollisionWorld::ClosestRayResultCallback rayCallback(asBtVector(start), asBtVector(end));

		world->rayTest(rayCallback.m_rayFromWorld, rayCallback.m_rayToWorld, rayCallback);
		result.hit = rayCallback.hasHit();
		if (result.hit) {
			result.position = asVector(rayCallback.m_hitPointWorld);
			result.normal = asVector(rayCallback.m_hitNormalWorld);
			result.hitBody = *(Body*)rayCallback.m_collisionObject->getUserPointer();
		}
		else {
			result.position = end;
			result.normal = (end - start).normalized();
		}

		promise->set_value(result);
	});

	return future;
}
