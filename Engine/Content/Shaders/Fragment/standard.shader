/*
	This is the standard fragment shader

	A fragment shader is responsible for telling the GPU what color a specific fragment should be
*/

#include <Shaders/shaders.inc>



#if defined( EAE6320_PLATFORM_GL )
out FLOAT4 o_color;
#endif


void main(

	// Input
	//======
#if defined( EAE6320_PLATFORM_D3D )
	in const FLOAT4 i_fragmentPosition : SV_POSITION,

	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out FLOAT4 o_color : SV_TARGET
#endif
)
{
	// Output solid white
	o_color = FLOAT4(
		// RGB (color)
		1.0, 1.0, 1.0,
		// Alpha (opacity)
		1.0 );
}

