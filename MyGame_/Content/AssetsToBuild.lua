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
	},
}
