#include "graphics.h"
#include "chunkHandler.h"
#include "debug_tools.h"

Graphics::Graphics()
{

}

Graphics::~Graphics()
{

}

bool Graphics::Initialize(int width, int height)
{
	// Init Camera
	m_camera = new Camera();
	if (!m_camera->Initialize(width, height))
	{
		printf("Camera Failed to Initialize\n");
		return false;
	}

	// Set up the shaders
	m_shader = new Shader("Shaders\\core.vs","Shaders\\core.frag");
	if (!m_shader->Initialize())
	{
		printf("Shader Failed to Initialize\n");
		return false;
	}

	// Add the vertex shader
	if (!m_shader->AddShader(GL_VERTEX_SHADER))
	{
		printf("Vertex Shader failed to Initialize\n");
		return false;
	}

	// Add the fragment shader
	if (!m_shader->AddShader(GL_FRAGMENT_SHADER))
	{
		printf("Fragment Shader failed to Initialize\n");
		return false;
	}

	// Connect the program
	if (!m_shader->Finalize())
	{
		printf("Program to Finalize\n");
		return false;
	}

	m_wireframe_shader = new Shader("Shaders\\wireframe.vs", "Shaders\\wireframe.frag");
	if (!m_wireframe_shader->Initialize())
	{
		printf("Shader Failed to Initialize\n");
		return false;
	}

	// Add the vertex shader
	if (!m_wireframe_shader->AddShader(GL_VERTEX_SHADER))
	{
		printf("Vertex Shader failed to Initialize\n");
		return false;
	}

	// Add the fragment shader
	if (!m_wireframe_shader->AddShader(GL_FRAGMENT_SHADER))
	{
		printf("Fragment Shader failed to Initialize\n");
		return false;
	}

	// Connect the program
	if (!m_wireframe_shader->Finalize())
	{
		printf("Program to Finalize\n");
		return false;
	}

	// Populate location bindings of the shader uniform/attribs
	if (!collectShPrLocs()) {
		printf("Some shader attribs not located!\n");
	}

	Debug_Tools::InputWireframeShader(m_wireframe_shader);
	
	m_chunks = new ChunkHandler(*m_shader);

	Debug_Tools::CreateWireframe(0, 0, 0, 16, 16, 16);
	Debug_Tools::CreateWireframe(16, 0, 0, 16, 16, 16);

	//enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Face culling
	glEnable(GL_CULL_FACE);

	// Wireframe
	glDepthFunc(GL_LESS);
	glLineWidth(4);

	// Transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

// Runs every frame, render objects here
void Graphics::Render()
{
	//clear the screen
	glClearColor(0.4, 0.5, 1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start the correct program
	m_shader->Enable();

	// Send in the projection and view to the shader (stay the same while camera intrinsic(perspective) and extrinsic (view) parameters are the same
	glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
	glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

	m_camera->Update(0.f);

	// Render Objects
	m_chunks->Render(m_shader, m_camera);

	m_wireframe_shader->Enable();
	glUniformMatrix4fv(m_wireframe_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
	glUniformMatrix4fv(m_wireframe_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

	Debug_Tools::RenderWireframes();
	
	// Get any errors from OpenGL
	auto error = glGetError();
	if (error != GL_NO_ERROR)
	{
		string val = ErrorString(error);
		std::cout << "Error initializing OpenGL! " << error << ", " << val << std::endl;
	}
}

// Gets the location in the shader for some variables
bool Graphics::collectShPrLocs() {
	bool anyProblem = true;
	// Locate the projection matrix in the shader
	m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
	if (m_projectionMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_projectionMatrix not found\n");
		anyProblem = false;
	}

	// Locate the view matrix in the shader
	m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
	if (m_viewMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_viewMatrix not found\n");
		anyProblem = false;
	}

	// Locate the model matrix in the shader
	m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");
	if (m_modelMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_modelMatrix not found\n");
		anyProblem = false;
	}

	m_wireframe_projectionMatrix = m_wireframe_shader->GetUniformLocation("projectionMatrix");
	if (m_wireframe_projectionMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_wireframe_projectionMatrix not found\n");
		anyProblem = false;
	}

	m_wireframe_viewMatrix = m_wireframe_shader->GetUniformLocation("viewMatrix");
	if (m_wireframe_viewMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_wireframe_viewMatrix not found\n");
		anyProblem = false;
	}

	return anyProblem;
}

// Helper function to print out more detailed error messages
std::string Graphics::ErrorString(GLenum error)
{
	if (error == GL_INVALID_ENUM)
	{
		return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
	}

	else if (error == GL_INVALID_VALUE)
	{
		return "GL_INVALID_VALUE: A numeric argument is out of range.";
	}

	else if (error == GL_INVALID_OPERATION)
	{
		return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
	}

	else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
	{
		return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
	}

	else if (error == GL_OUT_OF_MEMORY)
	{
		return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
	}
	else
	{
		return "None";
	}
}

