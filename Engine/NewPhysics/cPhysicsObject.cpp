// NewPhysics.cpp : Defines the functions for the static library.
//

#include "cPhysicsObject.h"

#include "Engine/UserInput/UserInput.h"
#include "Engine/Logging/Logging.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Asserts/Asserts.h"

eae6320::PhysicsSys::cPhysicsObject::cPhysicsObject()
{
	position = Math::sVector(0.0, 0.0, 0.0);
	rotation = Math::cQuaternion();
	acceleration = Math::sVector(0.0, 0.0, 0.0);
	velocityLinear = Math::sVector(0.0, 0.0, 0.0);
	rotationAxis = Math::sVector(0.0, 0.0, 1.0);
	velocityAngular = 0.0f;
	accelerationAngular = 0.0f;

	//OBJECT PROPERTIES
	mass = 1.0f;
	moveForce = 5.0f;
	drag = 0.5f;
	friction = 0.5f;
	torque = 0.0f;
	width = 0.0f;
	height = 0.0f;
	depth = 0.0f;
	momentOfInertia = (mass * (width * width + height * height)) / 12;

	//KEYS
	upInput = "W";
	downInput = "S";
	rightInput = "D";
	leftInput = "A";
	rightRotateInput = "E";
	leftRotateInput = "Q";

	//USER INFO
	moveable = false;
	minBounds = Math::sVector(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	maxBounds = Math::sVector(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
}

eae6320::PhysicsSys::cPhysicsObject::cPhysicsObject(eae6320::Math::sVector i_position, eae6320::Math::cQuaternion i_rotation, const std::string& i_physicsInfo, const float i_mass, const float i_friction, const float i_drag, const float i_moveForce)
{
	position = i_position;
	rotation = i_rotation;
	acceleration = Math::sVector(0.0, 0.0, 0.0);
	velocityLinear = Math::sVector(0.0, 0.0, 0.0);
	rotationAxis = Math::sVector(0.0, 0.0, 1.0);
	velocityAngular = 0.0f;
	accelerationAngular = 0.0f;

	if (i_physicsInfo == "")
	{
		moveable = false;
		width = 0.0f;
		height = 0.0f;
		depth = 0.0f;
	}
	else
	{
		moveable = true;
		LoadUserInfo(i_physicsInfo);
	}

	mass = i_mass;
	moveForce = i_moveForce;
	drag = i_drag;
	friction = i_friction;

	minBounds = Math::sVector(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	maxBounds = Math::sVector(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

	//not changed by user
	torque = 0.0f;
	momentOfInertia = (mass * (width * width + height * height)) / 12;
}

eae6320::cResult eae6320::PhysicsSys::cPhysicsObject::LoadUserInfo(const std::string& i_userInfo)
{
	auto result = Results::Success;
	Platform::sDataFromFile dataFromFile;
	std::string errorMessage;
	if (!(result = Platform::LoadBinaryFile(i_userInfo.c_str(), dataFromFile, &errorMessage)))
	{
		EAE6320_ASSERTF(false, errorMessage.c_str());
		Logging::OutputError("Failed to load physics information from file %s: %s", i_userInfo.c_str(), errorMessage.c_str());
		return result;
	}

	//Time to extract from file! 
	//information goes width, height, depth, left, up, right, down, rotateleft, rotateright, forward, backward
	auto currentOffset = reinterpret_cast<uintptr_t>(dataFromFile.data);
	const auto finalOffset = currentOffset + dataFromFile.size;
	
	//WIDTH
	auto tempWidth = *reinterpret_cast<double*>(currentOffset);
	width = (float)tempWidth;
	currentOffset += sizeof(tempWidth);

	//HEIGHT
	auto tempHeight = *reinterpret_cast<double*>(currentOffset);
	height = (float)tempHeight;
	currentOffset += sizeof(tempHeight);

	//DEPTH
	auto tempDepth = *reinterpret_cast<double*>(currentOffset);
	depth = (float)tempDepth;
	currentOffset += sizeof(tempDepth);

	//LEFT
	auto stringSize = *reinterpret_cast<size_t*>(currentOffset);
	currentOffset += sizeof(stringSize);
	std::string leftKey;
	leftKey.resize(stringSize);
	leftKey = *reinterpret_cast<char*>(currentOffset);
	leftInput = leftKey;
	currentOffset += stringSize;

	//UP
	stringSize = *reinterpret_cast<size_t*>(currentOffset);
	currentOffset += sizeof(stringSize);
	std::string upKey;
	upKey.resize(stringSize);
	upKey = *reinterpret_cast<char*>(currentOffset);
	upInput = upKey;
	currentOffset += stringSize;

	//RIGHT
	stringSize = *reinterpret_cast<size_t*>(currentOffset);
	currentOffset += sizeof(stringSize);
	std::string rightKey;
	rightKey.resize(stringSize);
	rightKey = *reinterpret_cast<char*>(currentOffset);
	rightInput = rightKey;
	currentOffset += stringSize;

	//DOWN
	stringSize = *reinterpret_cast<size_t*>(currentOffset);
	currentOffset += sizeof(stringSize);
	std::string downKey;
	downKey.resize(stringSize);
	downKey = *reinterpret_cast<char*>(currentOffset);
	downInput = downKey;
	currentOffset += stringSize;

	//ROTATE LEFT
	stringSize = *reinterpret_cast<size_t*>(currentOffset);
	currentOffset += sizeof(stringSize);
	std::string rotLeftKey;
	rotLeftKey.resize(stringSize);
	rotLeftKey = *reinterpret_cast<char*>(currentOffset);
	leftRotateInput = rotLeftKey;
	currentOffset += stringSize;

	//ROTATE RIGHT
	stringSize = *reinterpret_cast<size_t*>(currentOffset);
	currentOffset += sizeof(stringSize);
	std::string rotRightKey;
	rotRightKey.resize(stringSize);
	rotRightKey = *reinterpret_cast<char*>(currentOffset);
	rightRotateInput = rotRightKey;
	currentOffset += stringSize;

	//FORWARD
	stringSize = *reinterpret_cast<size_t*>(currentOffset);
	currentOffset += sizeof(stringSize);
	std::string forwardKey;
	forwardKey.resize(stringSize);
	forwardKey = *reinterpret_cast<char*>(currentOffset);
	forwardInput = forwardKey;
	currentOffset += stringSize;

	//BACKWARD
	stringSize = *reinterpret_cast<size_t*>(currentOffset);
	currentOffset += sizeof(stringSize);
	std::string backwardKey;
	backwardKey.resize(stringSize);
	backwardKey = *reinterpret_cast<char*>(currentOffset);
	backwardInput = backwardKey;
	currentOffset += stringSize;

	return result;
}

void eae6320::PhysicsSys::cPhysicsObject::UpdatePhysics(const float i_elapsedSeconds)
{
	if (!moveable)
		return;

	CheckUserInput(); //This will update the velocity and rotation of the object
	UpdateForces(); //This will update the acceleration of the object based on drag and mass

	//UPDATING LINEAR MOVEMENT
	{
		velocityLinear = velocityLinear + acceleration * i_elapsedSeconds;
		Math::sVector newPosition = position + velocityLinear * i_elapsedSeconds;
		bool inBounds = CheckBounds(newPosition);
		position = newPosition;
	}

	//UPDATING ROTATION
	{
		velocityAngular = velocityAngular + accelerationAngular * i_elapsedSeconds;
		rotation = rotation * Math::cQuaternion::cQuaternion(velocityAngular * i_elapsedSeconds, rotationAxis);
		rotation.Normalize();
	}
}

void eae6320::PhysicsSys::cPhysicsObject::UpdateForces()
{
	//should update acceleration
	//take forces that have been applied as impulse
	Math::sVector velocitySquared = Math::sVector(velocityLinear.x * velocityLinear.x, velocityLinear.y * velocityLinear.y, velocityLinear.z * velocityLinear.z);
	Math::sVector dragForce = velocityLinear * -friction;
	acceleration = (appliedForce + dragForce) / mass;
	float airResistance = velocityAngular * -drag;
	if (torque >= -drag && torque <= drag)
	{
		torque = 0.0f;
	}
	else
	{
		torque += airResistance;
	}
	accelerationAngular = (torque / momentOfInertia) + airResistance + appliedRotation;
}

void eae6320::PhysicsSys::cPhysicsObject::ApplyForce(Math::sVector i_force, Math::sVector i_position)
{
	if (!moveable)
		return;

	Math::sVector armVector = i_position - position;
	//force is below object and negative, or above object and positive, so not applied
	if ((armVector.y < 0 && i_force.y < 0) || (armVector.y > 0 && i_force.y > 0))
		return;
	//force is to left of object and negative, or to right of object and positive, so not applied
	if ((armVector.x < 0 && i_force.x < 0) || (armVector.x > 0 && i_force.x > 0))
		return;
	Math::sVector relativeVelocity = velocityLinear + CrossFloatVector(velocityAngular, armVector) - i_force;
	torque = CrossVectors(armVector, relativeVelocity);
	appliedForce += i_force;
}

bool eae6320::PhysicsSys::cPhysicsObject::CheckBounds(Math::sVector& i_newPosition)
{
	//confirm that the new position is within the bounds set by the user
	//true means in bounds, false means out of bounds
	float max_x = i_newPosition.x + (width / 2);
	float min_x = i_newPosition.x - (width / 2);
	float max_y = i_newPosition.y + (height / 2);
	float min_y = i_newPosition.y - (height / 2);
	float max_z = i_newPosition.z + (depth / 2);
	float min_z = i_newPosition.z - (depth / 2);

	//checking X values
	if (min_x < minBounds.x)
	{
		i_newPosition.x = minBounds.x + (width / 2);
	}
	else if (max_x > maxBounds.x)
	{
		i_newPosition.x = maxBounds.x - (width / 2);
	}
	//checking Y values
	if (min_y < minBounds.y)
	{
		i_newPosition.y = minBounds.y + (height / 2);
	}
	else if (max_y > maxBounds.y)
	{
		i_newPosition.y = maxBounds.y - (height / 2);
	}
	//checking Z values
	if (min_z < minBounds.z)
	{
		i_newPosition.z = minBounds.z + (depth / 2);
	}
	else if (max_z > maxBounds.z)
	{
		i_newPosition.z = maxBounds.y - (depth / 2);
	}
	return true;
}

void eae6320::PhysicsSys::cPhysicsObject::CheckUserInput()
{
	//PLAYER MOVEMENT
	if (!moveable) //should never hit this if not moveable, but just here as a sanity check
		return;

	//Y DIRECTION
	if (UserInput::IsKeyPressed(upInput[0]))
	{
		appliedForce.y = moveForce;
	}
	else if (UserInput::IsKeyPressed(downInput[0]))
	{
		appliedForce.y = -moveForce;
	}
	else
	{
		appliedForce.y = 0;
	}
	
	//X DIRECTION
	if (UserInput::IsKeyPressed(leftInput[0]))
	{
		appliedForce.x = -moveForce;
	}
	else if (UserInput::IsKeyPressed(rightInput[0]))
	{
		appliedForce.x = moveForce;
	}
	else
	{
		appliedForce.x = 0;
	}
	
	//Z DIRECTION
	if (UserInput::IsKeyPressed(forwardInput[0]))
	{
		appliedForce.z = moveForce;
	}
	else if (UserInput::IsKeyPressed(backwardInput[0]))
	{
		appliedForce.z = -moveForce;
	}
	else
	{
		appliedForce.z = 0;
	}

	//ROTATION
	if (UserInput::IsKeyPressed(leftRotateInput[0]))
	{
		appliedRotation = moveForce;
	}
	else if (UserInput::IsKeyPressed(rightRotateInput[0]))
	{
		appliedRotation = -moveForce;
	}
	else
	{
		appliedRotation = 0;
	}
}

float eae6320::PhysicsSys::cPhysicsObject::Dot(Math::sVector i_vector1, Math::sVector i_vector2)
{
	return i_vector1.x * i_vector2.x + i_vector1.y * i_vector2.y + i_vector1.z * i_vector2.z;
}

float eae6320::PhysicsSys::cPhysicsObject::CrossVectors(Math::sVector i_vector1, Math::sVector i_vector2)
{
	return i_vector1.x * i_vector2.y - i_vector1.y * i_vector2.x;
}

eae6320::Math::sVector eae6320::PhysicsSys::cPhysicsObject::CrossFloatVector(float i_float, Math::sVector i_vector)
{
	return Math::sVector(-i_float * i_vector.y, i_float * i_vector.x, 0.0f);
}

void eae6320::PhysicsSys::cPhysicsObject::SetMass(const float i_mass) 
{
	mass = i_mass;
	momentOfInertia = (mass * (width * width + height * height)) / 12;
}

void eae6320::PhysicsSys::cPhysicsObject::SetFriction(const float i_friction) 
{
	friction = i_friction;
}

void eae6320::PhysicsSys::cPhysicsObject::SetDrag(const float i_drag) 
{
	drag = i_drag;
}

void eae6320::PhysicsSys::cPhysicsObject::SetMoveForce(const float i_moveForce)
{
	moveForce = i_moveForce;
}

void eae6320::PhysicsSys::cPhysicsObject::SetBounds(Math::sVector i_minBounds, Math::sVector i_maxBounds)
{
	minBounds = i_minBounds;
	maxBounds = i_maxBounds;
}

void eae6320::PhysicsSys::cPhysicsObject::ChangeRotationAxis(Math::sVector i_newAxis)
{
	rotationAxis = i_newAxis;
}