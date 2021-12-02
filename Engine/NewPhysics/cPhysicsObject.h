/*
	This struct can be used to represent the state of a
	rigid body in 3D space
*/

#ifndef EAE6320_NEWPHYSICS_CPHYSICSOBJECT_H
#define EAE6320_NEWPHYSICS_CPHYSICSOBJECT_H

// Includes
//=========

#include <Engine/Math/cQuaternion.h>
#include <Engine/Math/sVector.h>
#include <Engine/Math/cMatrix_transformation.h>
#include <Engine/Results/Results.h>
#include <string>

// Struct Declaration
//===================

namespace eae6320
{
	namespace PhysicsSys
	{
		class cPhysicsObject
		{
			// Interface
			//==========
		public:
			Math::sVector position;
			Math::cQuaternion rotation;

			cPhysicsObject();
			cPhysicsObject(Math::sVector i_position, Math::cQuaternion i_rotation, const std::string& i_physicsInfo = "", const float i_mass = 1.0f, const float i_friction = 0.5f, const float i_drag = 0.5f, const float i_moveForce = 5.0f);
			//method to be called in the update loop for each object
			void UpdatePhysics(const float i_elapsedSeconds);
			//should theoretically apply an impulse to an object, but is buggy. i would not recommend using this.
			void ApplyForce(Math::sVector i_force, Math::sVector i_position);
			
			//SETTERS
			void SetMass(const float i_mass);
			void SetFriction(const float i_friction);
			void SetDrag(const float i_drag);
			void SetMoveForce(const float i_moveForce);
			void SetBounds(Math::sVector i_minBounds, Math::sVector i_maxBounds);
			void ChangeRotationAxis(Math::sVector i_newRotationAxis);

		private:
			//INFORMATION
			std::string userInfo;
			bool moveable;
			Math::sVector minBounds; //by default, set to minimum float for all values-- set bounds by calling SetBounds function
			Math::sVector maxBounds; //by default, set to maximum float for all values-- set bounds by calling SetBounds function

			//MOVEMENT
			Math::sVector velocityLinear;
			Math::sVector acceleration;
			Math::sVector appliedForce;
			 
			//ROTATION
			float velocityAngular;
			Math::sVector rotationAxis;
			float accelerationAngular;
			float appliedRotation = 0.0f;
			
			//OBJECT PROPERTIES
			float mass;
			float moveForce;
			float drag; //resistance for rotational movement
			float friction; //resistance for linear movement -- negated in code, should not be negative unless low friction
			float torque;
			float momentOfInertia;
			float width;
			float height;
			float depth;

			//KEYS
			std::string upInput;
			std::string downInput;
			std::string rightInput;
			std::string leftInput;
			std::string rightRotateInput;
			std::string leftRotateInput;
			std::string forwardInput; // positive z
			std::string backwardInput; // negative z

			void CheckUserInput();
			bool CheckBounds(Math::sVector& i_newPosition);
			void UpdateForces();
			eae6320::cResult LoadUserInfo(const std::string& i_userInfo);
			float Dot(Math::sVector i_vector1, Math::sVector i_vector2);
			float CrossVectors(Math::sVector i_vector1, Math::sVector i_vector2);
			Math::sVector CrossFloatVector(float i_float, Math::sVector i_vector);

		};
	}
}

#endif	// EAE6320_NEWPHYSICS_CPHYSICSOBJECT_H
