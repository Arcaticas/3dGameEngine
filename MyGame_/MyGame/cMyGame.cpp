// Includes
//=========

#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Graphics/Graphics.h>

// Inherited Implementation
//=========================

// Run
//----

bool hide;
bool switchEffect;


void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	const char* const shaderPath1 = "data/Shaders/Fragment/flasher.shader";
	const char* const shaderPath2 = "data/Shaders/Fragment/flasher2.shader";

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

	eae6320::Graphics::s_meshData input1;
	input1.i_vertexInputs = stuff1;
	input1.i_vSize = stuff1Size;
	input1.i_indexArray = stuff2;
	input1.i_iSize = stuff2Size;

	eae6320::Graphics::s_meshData input2;
	input2.i_vertexInputs = stuff3;
	input2.i_vSize = stuff3Size;
	input2.i_indexArray = stuff4;
	input2.i_iSize = stuff4Size;


	
	eae6320::Graphics::SetBackgroundColor(0, .5, .5, 1.0);
	
	if (hide)
	{

	}
	else
	{
		eae6320::Graphics::SubmitMeshEffectPair(input1, shaderPath1);
	}

	if (switchEffect)
	{
		eae6320::Graphics::SubmitMeshEffectPair(input2, shaderPath1);

	}
	else
	{
		eae6320::Graphics::SubmitMeshEffectPair(input2, shaderPath2);

	}
	

	//CreateGameObject(mesh, shader, vector);
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

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space))
	{
		hide = true;
	}
	else
	{
		hide = false;
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Shift))
	{
		switchEffect = true;
	}
	else
	{
		switchEffect = false;
	}
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	hide = false;
	switchEffect = false;
	return Results::Success;
	
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	return Results::Success;
}
