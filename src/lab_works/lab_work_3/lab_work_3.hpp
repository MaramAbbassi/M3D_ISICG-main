#ifndef __LAB_WORK_3_HPP__
#define __LAB_WORK_3_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork3 : public BaseLabWork
	{
	  public:
		LabWork3() : BaseLabWork() {}
		~LabWork3();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;
		void _updateViewMatrix();
		void _updateProjectionMatrix();
		void _initCamera();

	  private:
		// ================ Scene data.
		// ================

		GLuint _programID = GL_INVALID_INDEX;

		// ================ GL data.
		// ================
		GLuint _vbo					  = GL_INVALID_INDEX;
		GLuint _vao					  = GL_INVALID_INDEX;
		GLuint _ebo					  = GL_INVALID_INDEX;
		GLuint _colorVBO			  = GL_INVALID_INDEX;
		GLint  _uTranslationXLocation = -1;
		/* float _time				  = 0.0f;
		float  _brightness = 1.0f;
		GLuint _uBrightnessLocation;*/

		struct Mesh
		{
			std::vector<glm::vec3>	  positions;
			std::vector<glm::vec3>	  colors;
			std::vector<unsigned int> indices;
			glm::mat4				  transformation;

			GLuint VBO = 0;
			GLuint VAO = 0;
			GLuint EBO = 0;
		};
		Mesh	  _cube;
		void	  _createCube();
		void	  _initBuffers();
		GLint	  _modelMatrixLocation = -1;
		glm::mat4 modelMatrix		   = glm::mat4( 1.0f );
		Camera	  _camera;
		GLuint	  _viewMatrixLocation		= -1;
		GLuint	  _projectionMatrixLocation = -1;
		float	  _cameraFovy				= 60;
		float	  _cameraSpeed				= 0.1f;
		float	  _cameraSensitivity		= 0.1f;

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_3_HPP__
