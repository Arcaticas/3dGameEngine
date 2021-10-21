// Includes
//=========

#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Graphics/Graphics.h>
#include<Engine/Application/cGameObject.h>
#include <Engine/Application/cCamera.h>
#include <Engine/Math/cMatrix_transformation.h>

// Inherited Implementation
//=========================

// Run
//----

bool hide;
bool switchEffect;

eae6320::Application::Camera gameCam;
eae6320::Application::GameObject* AllGameObjects[500];
uint16_t GameObjectCount;


void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{


	
	eae6320::Graphics::SetBackgroundColor(0, .5, .5, 1.0);
	
	for (int i = 0; i < 3; i++)
	{
		eae6320::Graphics::SubmitGameObject(
			AllGameObjects[i]->mesh,
			AllGameObjects[i]->effect,
			AllGameObjects[i]->translation.PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
	}
	
	
	eae6320::Graphics::SubmitGameCamera(gameCam.translation);
}


void eae6320::cMyGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	AllGameObjects[0]->translation.Update(i_elapsedSecondCount_sinceLastUpdate);
	gameCam.translation.Update(i_elapsedSecondCount_sinceLastUpdate);
}

void eae6320::cMyGame::UpdateSimulationBasedOnInput()
{
	constexpr int stuff1Size = 5;
	eae6320::Graphics::VertexFormats::sVertex_mesh stuff1[stuff1Size];
	{
		stuff1[0].x = 0.0f;
		stuff1[0].y = -1.0f;
		stuff1[0].z = 0.0f;

		stuff1[1].x = .5f;
		stuff1[1].y = 0.0f;
		stuff1[1].z = 0.0f;

		stuff1[2].x = 1.0f;
		stuff1[2].y = -1.0f;
		stuff1[2].z = 0.0f;

		stuff1[3].x = -.5f;
		stuff1[3].y = 0.0f;
		stuff1[3].z = 0.0f;

		stuff1[4].x = -1.0f;
		stuff1[4].y = -1.0f;
		stuff1[4].z = 0.0f;
	}
	const auto stuff2Size = 6;
	uint16_t stuff2[stuff2Size];
	{
		stuff2[0] = 0;
		stuff2[1] = 1;
		stuff2[2] = 2;
		stuff2[3] = 0;
		stuff2[4] = 4;
		stuff2[5] = 3;
	}

	eae6320::Graphics::s_meshData input1;
	input1.i_vertexInputs = stuff1;
	input1.i_vSize = stuff1Size;
	input1.i_indexArray = stuff2;
	input1.i_iSize = stuff2Size;

	constexpr int stuff3Size = 3;
	eae6320::Graphics::VertexFormats::sVertex_mesh stuff3[stuff3Size];
	{
		stuff3[0].x = 0.0f;
		stuff3[0].y = 1.0f;
		stuff3[0].z = 0.0f;

		stuff3[1].x = .5f;
		stuff3[1].y = 0.0f;
		stuff3[1].z = 0.0f;

		stuff3[2].x = -.5f;
		stuff3[2].y = 0.0f;
		stuff3[2].z = 0.0f;

	}
	const auto stuff4Size = 6;
	uint16_t stuff4[stuff4Size];
	{
		stuff4[0] = 0;
		stuff4[1] = 1;
		stuff4[2] = 2;

	}

	eae6320::Graphics::s_meshData input2;
	input2.i_vertexInputs = stuff3;
	input2.i_vSize = stuff3Size;
	input2.i_indexArray = stuff4;
	input2.i_iSize = stuff4Size;

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space))
	{
		AllGameObjects[0]->UpdateMesh(input2);
	}
	else
	{
		//AllGameObjects[0]->UpdateMesh(input1);
	}

	//Object controls
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
	{
		AllGameObjects[0]->translation.velocity = Math::sVector(0, 1, 0);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
	{
		AllGameObjects[0]->translation.velocity = Math::sVector(0, -1, 0);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
	{
		AllGameObjects[0]->translation.velocity = Math::sVector(1, 0, 0);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
	{
		AllGameObjects[0]->translation.velocity = Math::sVector(-1, 0, 0);
	}
	else
	{
		AllGameObjects[0]->translation.velocity = Math::sVector(0, 0, 0);
	}
	//Camera Controls
	if (UserInput::IsKeyPressed('W'))
	{
		gameCam.translation.velocity = Math::sVector(0, 0, -1);
	}
	else if (UserInput::IsKeyPressed('S'))
	{
		gameCam.translation.velocity = Math::sVector(0, 0, 1);
	}
	else if (UserInput::IsKeyPressed('D'))
	{
		gameCam.translation.velocity = Math::sVector(1, 0, 0);
	}
	else if (UserInput::IsKeyPressed('A'))
	{
		gameCam.translation.velocity = Math::sVector(-1, 0, 0);
	}
	else
	{
		gameCam.translation.velocity = Math::sVector(0, 0, 0);
	}

}

void eae6320::cMyGame::UpdateBasedOnInput()
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

eae6320::cResult eae6320::cMyGame::Initialize()
{
	hide = false;
	switchEffect = false;

	const char* const shaderPath1 = "data/shaders/fragment/flasher.shader";
	const char* const meshPath1 = "data/meshes/meshone.mesh";
	const char* const meshPath2 = "data/meshes/test2.mesh";
	const char* const meshPath3 = "data/meshes/test4.mesh";

	eae6320::Application::GameObject::CreateGameObject(meshPath1, shaderPath1, Math::sVector(), AllGameObjects[0]);
	eae6320::Application::GameObject::CreateGameObject(meshPath2, shaderPath1, Math::sVector(), AllGameObjects[1]);
	eae6320::Application::GameObject::CreateGameObject(meshPath3, shaderPath1, Math::sVector(3,0,0), AllGameObjects[2]);
	
	gameCam = eae6320::Application::Camera::Camera(Math::cQuaternion(), Math::sVector(0, 0, 9));

	return Results::Success;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	for (int i = 0; i < 500; i++)
	{
		if (AllGameObjects[i] != nullptr)
			delete AllGameObjects[i];
	}
	return Results::Success;
}
