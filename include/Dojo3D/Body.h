#pragma once

#include "common_header.h"

namespace Phys {

	class Material;
	class World;
	class BodyPart;
	class Body;
	class Wheel;

	enum class BodyType {
		Dynamic,
		Static
	};

	class CollisionListener {
	public:
		virtual void onCollision(Phys::Body& me, Phys::Body& other, float force, const Vector& point) {}
		virtual void onSensorCollision(Body& other, btRigidBody& sensor) {};
	};

	class Body :
		public Dojo::Component {

	protected:
		
	public:
		void* operator new(size_t i) {
			return _mm_malloc(i, alignof(Body));
		}

		void operator delete(void* p) {
			_mm_free(p);
		}

		static const int ID = 1;

		CollisionListener* collisionListener = nullptr;

		Body(Dojo::Object& object, World& world, const Material& material, Group group, BodyType type = BodyType::Dynamic);

		virtual ~Body();

		BodyPart& addBoxShape(const Vector& dimensions, const Vector& center = Vector::Zero, const Quaternion& rotation = {});
		BodyPart& addConvexHullShape(const std::vector<Vector>& points, const Vector& center = Vector::Zero, const Quaternion& rotation = {});
		BodyPart& addSphereShape(float radius, const Vector& center = Vector::Zero);
		BodyPart& addAndCenterHeightmap(Shared<std::vector<float>> heightmap, size_t w, size_t h, const Vector& center = Vector::Zero);

		Wheel& addWheel(
			const Vector& connectionPoint,
			const Vector& direction,
			const Vector& axle,
			float radius,
			float suspensionRestLength,
			const btRaycastVehicle::btVehicleTuning& suspensionTuning = {});

		///Removes physical behaviors from this object
		void destroyPhysics();

		void applyForce(const Vector& force);
		void applyForceAtWorldPoint(const Vector& force, const Vector& localPoint);
		void applyForceAtLocalPoint(const Vector& force, const Vector& localPoint);

		void applyTorque(float t);

		void forcePosition(const Vector& position);
		void forceVelocity(const Vector& velocity);
		void forceRotation(Radians angle);

		void setActive();

		virtual float getMass() const;

		Vector getLocalPoint(const Vector& worldPosition) const;
		Vector getWorldPoint(const Vector& localPosition) const;

		Vector getVelocity() const;
		Vector getVelocityAtLocalPoint(const Vector& localPoint) const;

		Vector getPosition() const;

		float getLinearDamping() const;
		float getAngularDamping() const;

		void setDamping(float linear, float angular);

		void setTransform(const Vector& position, Radians angle);

		void setSleepingEnabled(bool enabled);

		void setPushable(bool p) {
			pushable = p;
		}

		Group getGroup() const {
			return group;
		}

		optional_ref<btRigidBody> getBtBody() {
			return *body;
		}

		optional_ref<btRaycastVehicle> getBtVehicle() {
			return *vehicle;
		}

		bool isStatic() const {
			return bodyType == BodyType::Static;
		}

		void onSimulationPaused();

		void updateObject();

		World& getWorld() const {
			return world;
		}

		float getMinimumDistanceTo(const Vector& pos) const;

		virtual void onDispose() override {
			destroyPhysics();
		}

		virtual bool canDestroy() const override {
			//wait until the World has really destroyed the body
			return body == nullptr;
		}

		bool isPushable() const;

		bool isInitialized() const {
			return body != nullptr;
		}

		virtual void onAttach() override;

		void _postSimulation();

	protected:
		World& world;
		bool pushable = true;

		Unique<btRigidBody> body;
		Group group = Group::invalid();
		BodyType bodyType;
		const Phys::Material& material;

		Dojo::SmallSet<Unique<BodyPart>> parts;

		Unique<btRaycastVehicle> vehicle;
		Unique<btVehicleRaycaster> vehicleRaycaster;
		Dojo::SmallSet<Unique<Wheel>> wheels;

		void _waitForBody() const;
		BodyPart& _addShape(Unique<btCollisionShape> shape, float volume, const Vector& offset, const Quaternion& rotation);
	};
}
