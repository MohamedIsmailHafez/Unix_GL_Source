#ifndef __SHADER_GEOMETRY__
#define __SHADER_GEOMETRY__

#include "ustd.hpp"
#include "shader.hpp"



namespace OpenGLEngine {

  class GeometryShader : public Shader {

    public:
      GeometryShader(unsigned int id, std::string name);
      virtual ~GeometryShader();
      void clean();
      void reset(const std::string& filename);
      virtual void update(Transform* worldTransform, Camera* mainCamera, float deltaTime);

      virtual void loadShaderToGpu(const std::string& filename);
      virtual void bind();
      virtual void unbind();
      virtual GLuint Program();

    private:

      enum
      {
        VERTEX_SHADER,
        FRAGMENT_SHADER,
        GEOMETRY_SHADER,

        NUM_SHADERS
      };
      enum
      {
        MODEL_U,
        VIEW_U,
        PROJECTION_U,
        CAMERA_POSITION,
        CAMERA_DIRECTION,
        DELTATIME,
        TIME,

        NUM_UNIFORMS
      };

      GLuint shaders[NUM_SHADERS];
      GLuint uniforms[NUM_UNIFORMS];

      GLuint program;

      GLuint createShader(const std::string& text, unsigned int type);
      std::string loadShaderCode(const std::string& filename);
      void checkShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);

      void setupAttributes();
      void linkNValidate();
      void setupUniforms();

  };

}

#endif
