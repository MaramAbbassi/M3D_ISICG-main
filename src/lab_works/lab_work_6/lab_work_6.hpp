#ifndef __LAB_WORK_6_HPP__
#define __LAB_WORK_6_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "define.hpp"
#include <map>
#include <vector>

namespace M3D_ISICG
{
	class LabWork6 : public BaseLabWork
	{
	  public:
		LabWork6() : BaseLabWork() {}
		~LabWork6();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;
		void resize( const int p_width, const int p_height ) override;

	  private:
		bool   _initProgram();
		void   _initCamera();
		void   _updateUniforms();
		GLuint _compileShader( GLenum type, const std::string & source );

		// ================ Scene data.
		TriangleMeshModel _bunnyModel;
		Camera			  _camera;
		// ================

		// ================ GL data.
		GLuint _programID			 = GL_INVALID_INDEX;
		GLint  _uMVPMatrixLoc		 = GL_INVALID_INDEX;
		GLint  _uModelMatrixLoc		 = GL_INVALID_INDEX;
		GLint  _uViewMatrixLoc		 = GL_INVALID_INDEX;
		GLint  _uProjectionMatrixLoc = GL_INVALID_INDEX;
		GLint  _uNormalMatrixLoc	 = GL_INVALID_INDEX;
		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		// Light
		Vec3f lightColor	= Vec3f( 1, 1, 1 );
		Vec3f lightPosition = Vec3f( 9, 10, -4 );
		// Camera
		float _cameraSpeed		 = 0.1f;
		float _cameraSensitivity = 0.1f;
		float _fovy				 = 60.f;
		// Lighting Models
		int Blinn_Phong = 1;
		int Gauss_light = 0;
		int NormalMaps	= 1;
		// ================
		void   _initGBuffuer();
		GLuint _gBufferTextures[ 6 ]; 
		GLuint FBO; 
		GLuint LightPOS;

		
		void _initGeometryPassProgram();
		void _geometryPass();
		void _initShadingPassProgram();
		void _shadingPass();
		void CreateCube();

		GLuint _geometryPassProgram; 
		GLuint _shadingPassProgram;
		GLenum outTexture = GL_COLOR_ATTACHMENT0; 

		GLuint _vbo		 = GL_INVALID_INDEX;
		GLuint _vao		 = GL_INVALID_INDEX;
		GLuint _ebo		 = GL_INVALID_INDEX;
		GLuint _colorVBO = GL_INVALID_INDEX;

		std::vector<GLuint> indices = { 0, 1, 2, 1, 3, 2 };
		std::vector<glm::vec3> vertexColors = { glm::vec3( 0.0f, 1.0f, 0.0f ),
												glm::vec3( 1.0f, 0.0f, 0.0f ),
												glm::vec3( 0.0f, 0.0f, 1.0f ),
												glm::vec3( 1.0f, 0.0f, 1.0f ) };

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_6_HPP__
