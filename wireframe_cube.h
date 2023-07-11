#pragma once
#include "shader.h"
#include "camera.h"

class WireframeCube
{
public:
	WireframeCube(Shader& shader, float xPos, float yPos, float zPos, float xSize, float ySize, float zSize)
	{

        model = glm::mat4(1.f) * glm::translate(glm::vec3(xPos * 2, yPos * 2, zPos * 2)) * glm::scale(glm::vec3(2 * xSize, 2 * ySize, 2 * zSize));
        ShaderInfo(&shader);
        setupVertices();
        setupBuffers();
	}

    void Render()
    {
        // Send in the projection and view to the shader (stay the same while camera intrinsic(perspective) and extrinsic (view) parameters are the same
        glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(vao);

        // Enable vertex attribute arrays for each vertex attrib
        glEnableVertexAttribArray(0);

        // Bind your VBO
        glBindBuffer(GL_ARRAY_BUFFER, VB);

        // Set vertex attribute pointers to the load correct data. Update here to load the correct attributes.
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);

        // Bind your Element Array
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

        // Render
        glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, nullptr);

        // Disable vertex arrays
        glDisableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

private:

    void setupVertices()
    {
        Vertices.insert(Vertices.begin(), {
           0, 0, 0,
           1, 0, 0,
           0, 1, 0,
           1, 1, 0,
           0, 0, 1,
           1, 0, 1,
           0, 1, 1,
           1, 1, 1
        });

        Indices = {
          0, 1, 1,
          0, 2, 2,
          0, 4, 4,

          1, 3, 3,
          1, 5, 5,

          2, 3, 3,
          2, 6, 6,

          7, 6, 6,
          7, 5, 5,
          7, 3, 3,

          6, 4, 4,
          4, 5, 5
      

          /*2, 1, 0,
          4, 2, 0,
          2, 4, 6,
          4, 5, 6,
          7, 6, 5,
          1, 3, 5,
          7, 5, 3,
          0, 1, 4,
          5, 4, 1,
          6, 3, 2,
          3, 6, 7*/
        };
    }

    void setupBuffers() 
    {
        // For OpenGL 3
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &VB);
        glBindBuffer(GL_ARRAY_BUFFER, VB);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Vertices.size(), (Vertices).data(), GL_STATIC_DRAW);

        glGenBuffers(1, &IB);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), (Indices).data(), GL_STATIC_DRAW);
    }

    // get shader var locations
    bool ShaderInfo(Shader* shader)
    {
        bool anyProblem = true;
        // Locate the projection matrix in the shader
        m_projectionMatrix = shader->GetUniformLocation("projectionMatrix");
        if (m_projectionMatrix == INVALID_UNIFORM_LOCATION)
        {
            printf("m_projectionMatrix not found\n");
            anyProblem = false;
        }

        // Locate the view matrix in the shader
        m_viewMatrix = shader->GetUniformLocation("viewMatrix");
        if (m_viewMatrix == INVALID_UNIFORM_LOCATION)
        {
            printf("m_viewMatrix not found\n");
            anyProblem = false;
        }

        // Locate the model matrix in the shader
        m_modelMatrix = shader->GetUniformLocation("modelMatrix");
        if (m_modelMatrix == INVALID_UNIFORM_LOCATION)
        {
            printf("m_modelMatrix not found\n");
            anyProblem = false;
        }

        return anyProblem;
    }

    // Graphics/Rendering
    GLuint vao;
    GLuint VB;
    GLuint IB;

    std::vector<float> Vertices;
    std::vector<unsigned int> Indices;

    bool initialSetup = false;

    glm::mat4 model;
    glm::mat4 positionMatrix;

	GLint m_projectionMatrix;
	GLint m_viewMatrix;
	GLint m_modelMatrix;
};