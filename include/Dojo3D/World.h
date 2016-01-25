#pragma once

#include "common_header.h"

#include "RayResult.h"
#include "ContactMode.h"
#include "WorldListener.h"

namespace Phys {
	class Body;
	class BodyPart;

	class World {
	public:
		typedef std::function<void()> Command;

	protected:
		struct DeferredCollision {
			BodyPart* A, *B;
			float force;
			btVector3 point;
			DeferredCollision() {}
			DeferredCollision(BodyPart& A, BodyPart& B, float force, const btVector3& point) :
				A(&A),
				B(&B),
				force(force),
				point(point) {

			}
		};

		struct DeferredSensorCollision {
			Body* other, *me;
			btRigidBody* sensor;
			DeferredSensorCollision() {}
			DeferredSensorCollision(Body& other, Body& me, btRigidBody& sensor) :
				other(&other),
				me(&me),
				sensor(&sensor) {

			}
		};

		struct Job {
			Command command, callback;
			Job() {}
			Job(const Command& command, const Command& callback) :
				command(command),
				callback(callback) {

			}
		};

	public:

		enum class FieldType {
			ApplyToSurface,
			ApplyToVolume
		};

		typedef std::unordered_set<Body*> BodyList;
		typedef std::vector<btRigidBody*> RigidBodyList;

		const float timeStep;
		
		World(const Vector& gravity, int stepsPerSecond, int collisionIterations = 10);

		virtual ~World();

		void addListener(WorldListener& listener);
		void removeListener(WorldListener& listener);

		void setContactMode(Phys::Group A, Phys::Group B, ContactMode mode);
		ContactMode getContactModeFor(Phys::Group A, Phys::Group B) const;

		Vector getGravity() const;

		void playCollisionSound(const DeferredCollision& collision);
		RayResult raycast(const Vector& start, const Vector& end, Phys::Group rayBelongsToGroup = 0) const;
		void asyncRaycast(const Vector& start, const Vector& end, Phys::Group rayBelongsToGroup, RayResult& result, const Command& callback = {}) const;
		bool _AABBQuery(const Dojo::AABB& area, Group group, BodyList* resultBody, RigidBodyList* resultRigidBody, bool precise, bool onlyPushable) const;

		void AABBQuery(const Dojo::AABB& area, Group group, RigidBodyList& result, bool precise = false) const;
		void AABBQuery(const Dojo::AABB& area, Group group, BodyList& result, bool precise = false) const;

		bool AABBQueryEmpty(const Dojo::AABB& area, Group group, bool precise = false) const;

		void applyForceField(const Dojo::AABB& area, Group group, const Vector& force, FieldType type);

		void update(float dt);

		void _notifyDestroyed(Body& body);

		void sync() const;

		void asyncCommand(Command command, const Command& callback = {}) const;
		void asyncCallback(const Command& callback) const;
		bool isWorkerThread() const;

		void addBody(Body& body);
		void removeBody(Body& body);

		void pause();

		void resume();

		bool isPaused() const {
			return simulationPaused;
		}

		void startPhysics() {
			resume();
		}

	protected:
		Unique<btCollisionConfiguration> collisionConfiguration;
		Unique<btDispatcher> dispatcher;
		Unique<btBroadphaseInterface> overlappingPairCache;
		Unique<btConstraintSolver> solver;
		Unique<btDynamicsWorld> world;

		Unique<Dojo::Pipe<Job>> commands;
		Unique<Dojo::Pipe<Command>> callbacks;
		Unique<Dojo::Pipe<DeferredCollision>> deferredCollisions;
		Unique<Dojo::Pipe<DeferredSensorCollision>> deferredSensorCollisions;

		Dojo::SmallSet<Body*> bodies, deletedBodies;

		Dojo::SmallSet<WorldListener*> listeners;

		std::thread thread;

		bool running = true;

		static const int GROUP_COUNT = 256; //HACK
		ContactMode collideMode[GROUP_COUNT][GROUP_COUNT];
		
		float removeNextSound = 0;
		std::deque<Vector> recentlyPlayedSoundPositions;

		bool simulationPaused = true;

		float _closestRecentlyPlayedSound(const Vector& point);
	};
}



