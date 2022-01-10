// Includes
//=========

#include "Dodge.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Graphics/Graphics.h>
#include<Engine/Application/cGameObject.h>
#include <Engine/Application/cCamera.h>
#include <Engine/Math/cMatrix_transformation.h>
#include <Engine/Sound/Sound.h>
#include <Engine/Time/Time.h>
#include <Engine/Collision/Collision.h>

#include<Windows.h>

eae6320::Application::Camera gameCam;
eae6320::Application::GameObject* AllGameObjects[500];
eae6320::Application::GameObject* Background;
uint16_t GameObjectCount = 0;
uint16_t timesHit = 0;



void SpawnBall(eae6320::Math::sVector i_vector)
{
	eae6320::Application::GameObject::CreateGameObject("data/meshes/small_square.mesh", "data/shaders/fragment/flasher.shader", i_vector, AllGameObjects[GameObjectCount]);
	
	AllGameObjects[GameObjectCount]->translation.acceleration = eae6320::Math::sVector(0, 0, .5f);

	GameObjectCount++;
}

void eae6320::Dodge::ResolveCollision(eae6320::Collision::sCollision i_coll)
{
	eae6320::Sound::Play("data/sounds/test.wav");

	i_coll.colliderB->rigidbody->position.z = -5;
	srand((int)i_coll.timeSeparation);
	i_coll.colliderB->rigidbody->position.x = -3 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3 - -3)));
	i_coll.colliderB->rigidbody->position.y = -3 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3 - -3)));
	timesHit++;
	
}

void eae6320::Dodge::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{


	
	eae6320::Graphics::SetBackgroundColor(0, .5, .5, 1.0);
	eae6320::Graphics::SubmitGameObject(
		Background->mesh,
		Background->effect,
		Background->translation.PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate)
	);

	for (int i = 0; i < GameObjectCount; i++)
	{
		eae6320::Graphics::SubmitGameObject(
			AllGameObjects[i]->mesh,
			AllGameObjects[i]->effect,
			AllGameObjects[i]->translation.PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
	}
	
	
	eae6320::Graphics::SubmitGameCamera(gameCam.translation);
}


void eae6320::Dodge::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{

	

	srand((unsigned int)eae6320::Time::GetCurrentSystemTimeTickCount());

	for (int i = 0; i < GameObjectCount; i++)
	{
		//AllGameObjects[i]->translation.Update(i_elapsedSecondCount_sinceLastUpdate);
		const double time = fmod(eae6320::Time::ConvertTicksToSeconds(eae6320::Time::GetCurrentSystemTimeTickCount()), rand() % 7);

		
		if ((time > 0.0 && time < .06) && AllGameObjects[i]->translation.position.z > 10)
		{
			AllGameObjects[i]->translation.position.x = -3 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3 - -3)));
			AllGameObjects[i]->translation.position.y = -3 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3 - -3)));
			AllGameObjects[i]->translation.position.z = -5;
		}
	}
	gameCam.translation.Update(i_elapsedSecondCount_sinceLastUpdate);
	eae6320::Collision::UpdateCollisions(i_elapsedSecondCount_sinceLastUpdate);

	if (timesHit >= 3)
	{
		eae6320::Sound::PlaySync("data/sounds/smb_mariodie.wav");
		eae6320::Application::iApplication::SetSimulationRate(0);
		
	}
}

void eae6320::Dodge::UpdateSimulationBasedOnInput()
{

	//Camera Controls
	if (UserInput::IsKeyPressed('W') && gameCam.translation.position.y < 3)
	{
		gameCam.translation.velocity = Math::sVector(0, 2, 0);
	}
	else if (UserInput::IsKeyPressed('S') && gameCam.translation.position.y > -3)
	{
		gameCam.translation.velocity = Math::sVector(0, -2, 0);
	}
	else if (UserInput::IsKeyPressed('D') && gameCam.translation.position.x < 3)
	{
		gameCam.translation.velocity = Math::sVector(2, 0, 0);
	}
	else if (UserInput::IsKeyPressed('A') && gameCam.translation.position.x > -3)
	{
		gameCam.translation.velocity = Math::sVector(-2, 0, 0);
	}
	else
	{
		gameCam.translation.velocity = Math::sVector(0, 0, 0);
	}

}

void eae6320::Dodge::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		Logging::OutputMessage("Escape was pressed");
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Dodge::Initialize()
{

	const char* const shaderPath1 = "data/shaders/fragment/flasher.shader";
	const char* const shaderPath2 = "data/shaders/fragment/flasher2.shader";

	const char* const meshPath2 = "data/meshes/small_square.mesh";
	const char* const meshPath4 = "data/meshes/large_square.mesh";

	eae6320::Application::GameObject::CreateGameObject(meshPath4, shaderPath2, Math::sVector(0, 0, -10), Background);
	
	int numberOfObjects = 10;

	for (int i = 0; i < numberOfObjects; i++)
	{
		SpawnBall(Math::sVector(-3 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3 - -3))),
			-3 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3 - -3))), -5));

		eae6320::Collision::cCollider::CreateCollider(&AllGameObjects[i]->translation, Math::sVector(.5, .5, 1), true, AllGameObjects[i]->collider);
	}
	

	
	gameCam = eae6320::Application::Camera::Camera(Math::cQuaternion(), Math::sVector(0, 0, 10));
	eae6320::Collision::cCollider::CreateCollider(&gameCam.translation, Math::sVector(.5,.5,1), true, gameCam.collider);
	
	gameCam.collider->ListenToCollision(std::bind(&eae6320::Dodge::ResolveCollision, this, std::placeholders::_1));

	return Results::Success;
}

eae6320::cResult eae6320::Dodge::CleanUp()
{
	delete Background;
	for (int i = 0; i < 500; i++)
	{
		if (AllGameObjects[i] != nullptr)
			delete AllGameObjects[i];
	}
	return Results::Success;
}

