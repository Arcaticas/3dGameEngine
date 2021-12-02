--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	shaders =
	{
		{ path = "Shaders/Vertex/standard.shader", arguments = { "vertex" } },
		{ path = "Shaders/Fragment/standard.shader", arguments = { "fragment" } },

		{ path = "Shaders/Fragment/flasher.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/flasher2.shader", arguments = { "fragment" } },

		{ path = "Shaders/Vertex/vertexInputLayout_mesh.shader", arguments = { "vertex" } },
	},
	meshes =
	{
		{ path = "Meshes/MeshOne.mesh"},
		{ path = "Meshes/test1.mesh"},
		{ path = "Meshes/test2.mesh"},
		{ path = "Meshes/test3.mesh"},
		{ path = "Meshes/test4.mesh"},
	},
	sounds =
	{
		{path = "Sounds/test.wav"},
		{path = "Sounds/background.wav"}
	},
	physics =
	{
		{path = "Physics/Player1.lua"},
	}
}
