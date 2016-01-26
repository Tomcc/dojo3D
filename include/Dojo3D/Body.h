#pragma once

#include "common_header.h"

namespace Phys {

	class Material;
	class World;
	class BodyPart;
	class Body;

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

		void removeShape(BodyPart& part);

		///Removes physical behaviors from this object
		void destroyPhysics();

		void setFixedRotation(bool enable);

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

		void setPushable(bool p) {
			pushable = p;
		}

		Group getGroup() const {
			return group;
		}

		btRigidBody* getBtBody() {
			return body.get();
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

		void _waitForBody() const;
		BodyPart& _addShape(Unique<btCollisionShape> shape, float volume, const Vector& offset, const Quaternion& rotation);
	};
}
