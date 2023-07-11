#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <fstream>
#include <sstream>
#include "graphics_headers.h"

class Shader
{
public:
    Shader();
    Shader(const char* vertex, const char* fragment);
    ~Shader();
    bool Initialize();
    void Enable() const;
    bool AddShader(GLenum ShaderType);
    bool Finalize();
    void SetInt(const char* name, float value, bool useShader);
    GLuint GetShaderProgram() { return m_shaderProg; }
    GLint GetUniformLocation(const char* pUniformName) const;
    GLint GetAttribLocation(const char* pAttribName) const;


private:
    GLuint m_shaderProg;
    const char* vertexShaderFile;
    const char* fragmentShaderFile;
    std::vector<GLuint> m_shaderObjList;
};

#endif  /* SHADER_H */
