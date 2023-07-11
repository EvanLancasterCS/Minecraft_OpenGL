#pragma once
#include "shader.h"
#include "wireframe_cube.h"

static class Debug_Tools
{
public:
	static void CreateWireframe(float xPos, float yPos, float zPos, float xSize, float ySize, float zSize);

	static void RenderWireframes();

	static void InputWireframeShader(Shader* shader) { m_wireframe_shader = shader; }
private:

	static std::vector<WireframeCube> wireframes;
	static Shader* m_wireframe_shader;
};