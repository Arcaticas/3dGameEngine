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

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	eae6320::Graphics::SetBackgroundColor(0, .5, .5, 1.0);
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
	return Results::Success;
	
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	return Results::Success;
}
