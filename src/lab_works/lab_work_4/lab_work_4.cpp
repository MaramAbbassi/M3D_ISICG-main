#include "lab_work_4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork4::_shaderFolder = "src/lab_works/lab_work_4/shaders/";

	LabWork4::~LabWork4()
	{
		_bunnyModel.cleanGL();
		glDeleteProgram( _programID );
	}

	bool LabWork4::init()
	{
		std::cout << "Initializing lab work 4..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		glEnable( GL_DEPTH_TEST );

		if ( !_initProgram() )
			return false;

		_initCamera();

		//_bunnyModel.load( "Bunny", "data/models/bunny/bunny.obj" );
		

		_bunnyModel.load( "Conference", "data/models/conference/conference.obj" );

		_bunnyModel._transformation = glm::scale( _bunnyModel._transformation, Vec3f( 0.03f, 0.03f, 0.03f ) );
		
		_updateUniforms();

		glUseProgram( _programID );
		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork4::animate( const float p_deltaTime ) {}

	void LabWork4::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // Clear the color buffer.

		_updateUniforms();

		_uMVPMatrixLoc	  = glGetUniformLocation( _programID, "uMVPMatrix" );
		_uNormalMatrixLoc = glGetUniformLocation( _programID, "uNormalMatrix" );
		_uViewMatrixLoc	  = glGetUniformLocation( _programID, "uViewMatrix" );
		_uModelMatrixLoc  = glGetUniformLocation( _programID, "uModelMatrix" );
		Mat4f mvp		  = _camera.getProjectionMatrix() * _camera.getViewMatrix() * _bunnyModel._transformation;
		Mat3f NormalMatrix
			= Mat3f( glm::transpose( glm::inverse( _camera.getViewMatrix() * _bunnyModel._transformation ) ) );
		
		glProgramUniformMatrix3fv( _programID, _uNormalMatrixLoc, 1, GL_FALSE, glm::value_ptr( NormalMatrix ) );
		glProgramUniformMatrix4fv( _programID, _uMVPMatrixLoc, 1, GL_FALSE, glm::value_ptr( mvp ) );
		glProgramUniformMatrix4fv(
			_programID, _uViewMatrixLoc, 1, GL_FALSE, glm::value_ptr( _camera.getViewMatrix() ) );
		glProgramUniformMatrix4fv(
			_programID, _uModelMatrixLoc, 1, GL_FALSE, glm::value_ptr( _bunnyModel._transformation ) );
		glProgramUniformMatrix4fv(
			_programID, _uProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr( _camera.getProjectionMatrix() ) );

		_bunnyModel.render( _programID );
	}

	void LabWork4::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera.moveFront( _cameraSpeed );
				break;
			case SDL_SCANCODE_S: // Back
				_camera.moveFront( -_cameraSpeed );
				break;
			case SDL_SCANCODE_A: // Left
				_camera.moveRight( -_cameraSpeed );
				break;
			case SDL_SCANCODE_D: // Right
				_camera.moveRight( _cameraSpeed );
				break;
			case SDL_SCANCODE_R: // Up
				_camera.moveUp( _cameraSpeed );
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera.moveUp( -_cameraSpeed );
				break;
			case SDL_SCANCODE_SPACE: // Print camera info
				_camera.print();
				break;
			default: break;
			}
		}

		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
		}
	}

	void LabWork4::displayUI()
	{
		ImGui::Begin( "Settings lab work 4" );

		if ( ImGui::DragFloat3( "Light Position", &lightPosition.x, 0.05f ) )
		{
			glProgramUniform3fv(
				_programID, glGetUniformLocation( _programID, "LightPos" ), 1, glm::value_ptr( lightPosition ) );
		}

		if ( ImGui::ColorEdit3( "Light Color", &lightColor.r ) )
		{
			glProgramUniform3fv(
				_programID, glGetUniformLocation( _programID, "LightColor" ), 1, glm::value_ptr( lightColor ) );
		}
		

		// Background.
		if ( ImGui::ColorEdit3( "Background", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}

		// Camera.
		if ( ImGui::SliderFloat( "fovy", &_fovy, 10.f, 160.f, "%01.f" ) )
		{
			_camera.setFovy( _fovy );
		}
		if ( ImGui::SliderFloat( "Speed", &_cameraSpeed, 0.1f, 10.f, "%01.1f" ) ) {}

		//
		
		if ( ImGui::Checkbox( "Blinn-Phong lighting  Model", (bool *)&Blinn_Phong ) )
			glProgramUniform1i( _programID, glGetUniformLocation( _programID, "Blinn_Phong" ), Blinn_Phong );
		if ( ImGui::Checkbox( "Gaussian lighting Model", (bool *)&Gauss_light ) )
			glProgramUniform1i( _programID, glGetUniformLocation( _programID, "Gauss_light" ), Gauss_light );

		ImGui::End();
	}

	void LabWork4::resize( const int p_width, const int p_height )
	{
		BaseLabWork::resize( p_width, p_height );
		_camera.setScreenSize( p_width, p_height );
	}
	
	bool LabWork4::_initProgram()
	{
		const std::string vertexShaderSrc	= readFile( _shaderFolder + "mesh.vert" );
		const std::string fragmentShaderSrc = readFile( _shaderFolder + "mesh.frag" );

		const GLuint vertexShader	= _compileShader( GL_VERTEX_SHADER, vertexShaderSrc );
		const GLuint fragmentShader = _compileShader( GL_FRAGMENT_SHADER, fragmentShaderSrc );

		if ( !vertexShader || !fragmentShader )
			return false;

		_programID = glCreateProgram();

		glAttachShader( _programID, vertexShader );
		glAttachShader( _programID, fragmentShader );
		glLinkProgram( _programID );
		GLint linked;
		glGetProgramiv( _programID, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _programID, sizeof( log ), NULL, log );
			std::cerr << "Error linking program: " << log << std::endl;
			return false;
		}
		
		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );
		

		return true;
	}
	
	void LabWork4::_initCamera()

	{ 
		 _camera.setScreenSize( _windowWidth, _windowHeight );
		//  Pour bunny
		/* _camera.setPosition( Vec3f( 0, 0, 2 ) );
		_camera.setLookAt( Vec3f( 0, 0, 1 ) );*/

		//  Pour La conference
		_camera.setPosition( Vec3f( 10, 10, 10 ) );
		_camera.setLookAt( Vec3f( 5, 5, 5 ) );
	}


	void LabWork4::_updateUniforms()
	{
		glProgramUniform3fv( _programID,
							 glGetUniformLocation( _programID, "CameraPosition" ),
							 1,
							 glm::value_ptr( _camera.getPosition() ) );
		glProgramUniform3fv(
			_programID, glGetUniformLocation( _programID, "LightPos" ), 1, glm::value_ptr( lightPosition ) );
		glProgramUniform3fv(
			_programID, glGetUniformLocation( _programID, "LightColor" ), 1, glm::value_ptr( lightColor ) );
		glProgramUniform1i( _programID, glGetUniformLocation( _programID, "Blinn_Phong" ), Blinn_Phong );
		glProgramUniform1i( _programID, glGetUniformLocation( _programID, "Gauss_light" ), Gauss_light );
	}


	GLuint LabWork4::_compileShader( GLenum type, const std::string & source )
	{
		GLuint		   shader = glCreateShader( type );
		const GLchar * src	  = source.c_str();
		glShaderSource( shader, 1, &src, NULL );
		glCompileShader( shader );

		GLint compiled;
		glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( shader, sizeof( log ), NULL, log );
			std::cerr << "Error compiling shader: " << log << std::endl;
			glDeleteShader( shader );
			return 0;
		}
		return shader;
	}

} // namespace M3D_ISICG
