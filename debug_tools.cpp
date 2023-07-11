#include "debug_tools.h"

// Static Declarations
std::vector<WireframeCube> Debug_Tools::wireframes;
Shader* Debug_Tools::m_wireframe_shader;


// Functions
void Debug_Tools::CreateWireframe(float xPos, float yPos, float zPos, float xSize, float ySize, float zSize)
{
	WireframeCube cube(*m_wireframe_shader, xPos, yPos, zPos, xSize, ySize, zSize);
	wireframes.push_back(cube);
}

void Debug_Tools::RenderWireframes()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);

	m_wireframe_shader->Enable();
	for (auto wireframe : wireframes)
		wireframe.Render();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
}