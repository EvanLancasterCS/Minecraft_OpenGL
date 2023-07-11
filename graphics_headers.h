#ifndef GRAPHICS_HEADERS_H
#define GRAPHICS_HEADERS_H

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#define GLM_ENABLE_EXPERIMENTAL 

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>

// GLM for matricies
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#define INVALID_UNIFORM_LOCATION 0x7fffffff

struct Vertex
{
	/*
	glm::vec3 vertex;
	glm::vec2 texcoord;
	glm::float32 sidelighting;
  
	Vertex(glm::vec3 v, glm::vec2 tc, glm::float32 sl): vertex(v), texcoord(tc), sidelighting(sl) {}
	*/
	glm::uint d1; // d1: [XXXX X][Y YYYY YYYY][ZZZZ Z] | [UUUU UVVV VV] | unused
	glm::uint d2; // d2: [LLLL]

	Vertex(glm::uint _d1, glm::uint _d2) :
		d1(_d1), d2(_d2) {}
};
#endif /* GRAPHICS_HEADERS_H */
