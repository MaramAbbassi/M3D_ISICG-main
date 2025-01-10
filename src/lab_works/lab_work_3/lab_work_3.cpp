#include "lab_work_3.hpp"
#include "imgui.h"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork3::_shaderFolder = "src/lab_works/lab_work_3/shaders/";

	LabWork3::~LabWork3()
	{
		if ( _vbo != GL_INVALID_INDEX )
		{
			glDeleteBuffers( 1, &_vbo );
			_vbo = GL_INVALID_INDEX;
		}
		if ( _vao != GL_INVALID_INDEX )
		{
			glDisableVertexArrayAttrib( _vao, 0 ); // Disable attribute 0
			glDeleteVertexArrays( 1, &_vao );	   // Delete the VAO
			_vao = GL_INVALID_INDEX;
		}
		if ( _ebo != GL_INVALID_INDEX )
		{
			glDeleteBuffers( 1, &_ebo );
			_ebo = GL_INVALID_INDEX;
		}

		if ( _colorVBO != GL_INVALID_INDEX )
		{
			glDeleteBuffers( 1, &_colorVBO );
			_colorVBO = GL_INVALID_INDEX;
		}
	}

	bool LabWork3::init()
	{
		std::cout << "Initializing lab work 3..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		

		const std::string vertexShaderStr	= readFile( _shaderFolder + "lw3.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw3.frag" );

		GLuint vertexShader	  = glCreateShader( GL_VERTEX_SHADER );
		GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		const GLchar * vSrc = vertexShaderStr.c_str();
		glShaderSource( vertexShader, 1, &vSrc, NULL );

		const GLchar * fSrc = fragmentShaderStr.c_str();
		glShaderSource( fragmentShader, 1, &fSrc, NULL );

		//
		glCompileShader( vertexShader );
		GLint compiled;
		glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertexShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std::cerr << "Error compiling vertex shader: " << log << std::endl;
			return false;
		}

		glCompileShader( fragmentShader );
		glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( fragmentShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std::cerr << "Error compiling fragment shader: " << log << std::endl;
			return false;
		}

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
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			glDeleteProgram( _programID );
			std::cerr << "Error linking program: " << log << std::endl;
			return false;
		}

		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );
		/*
		_modelMatrixLocation = glGetUniformLocation( _programID, "uModelMatrix" );
		modelMatrix = glm::mat4( 1.0f );
		_initCamera();
		_createCube();
		_initBuffers();
		_updateViewMatrix();
		_updateProjectionMatrix();
		glEnable( GL_DEPTH_TEST );
		*/
		_modelMatrixLocation	  = glGetUniformLocation( _programID, "uModelMatrix" );
		_viewMatrixLocation		  = glGetUniformLocation( _programID, "uViewMatrix" );
		_projectionMatrixLocation = glGetUniformLocation( _programID, "uProjectionMatrix" );
		modelMatrix				  = glm::mat4( 1.0f );

		// Initialize the camera and matrices

		_initCamera();
		_createCube();
		_initBuffers();
		_updateViewMatrix();
		_updateProjectionMatrix();
		glEnable( GL_DEPTH_TEST );


		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork3::animate( float p_deltaTime )
	{
		glm::mat4 rotation
			= glm::rotate( glm::mat4( 1.0f ), p_deltaTime, glm::vec3( 0.0f, 1.0f, 1.0f ) ); // Rotate around (0,1,1)
		modelMatrix = rotation * modelMatrix;
		glProgramUniformMatrix4fv( _programID, _modelMatrixLocation, 1, GL_FALSE, glm::value_ptr( modelMatrix ) );
	}

	void LabWork3::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glUseProgram( _programID );

		Mat4f mvpMatrix = _camera.getProjectionMatrix() * _camera.getViewMatrix() * modelMatrix;
		/*

		glUniformMatrix4fv( _modelMatrixLocation, 1, GL_FALSE, glm::value_ptr( modelMatrix ) );
		glUniformMatrix4fv( _viewMatrixLocation, 1, GL_FALSE, glm::value_ptr( _camera.getViewMatrix() ) );
		glUniformMatrix4fv( _projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr( _camera.getProjectionMatrix() ) );
		*/
		GLuint mvpLocation = glGetUniformLocation( _programID, "uMVPMatrix" );
		glUniformMatrix4fv( mvpLocation, 1, GL_FALSE, glm::value_ptr( mvpMatrix ) );

		glBindVertexArray( _cube.VAO );
		glDrawElements( GL_TRIANGLES, _cube.indices.size(), GL_UNSIGNED_INT, 0 );
		glBindVertexArray( 0 );
	}

	void LabWork3::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera.moveFront( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_S: // Back
				_camera.moveFront( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_A: // Left
				_camera.moveRight( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_D: // Right
				_camera.moveRight( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_R: // Up
				_camera.moveUp( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera.moveUp( -_cameraSpeed );
				_updateViewMatrix();
				break;
			default: break;
			}
		}

		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
			_updateViewMatrix();
		}
	}

	void LabWork3::displayUI()
	{
		ImGui::Begin( "Settings lab work 3" );
		ImGui::Text( "No setting available!" );

		if ( ImGui::SliderFloat( "FOV", &_cameraFovy, 10.0f, 120.0f ) )
		{
			_camera.setFovy( _cameraFovy );
			_updateProjectionMatrix();
		}

		ImGui::End();
	}
	void LabWork3::_createCube()
	{
		_cube.positions = {
			{ -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f }, // Back face
			{ -0.5f, -0.5f, 0.5f },	 { 0.5f, -0.5f, 0.5f },	 { 0.5f, 0.5f, 0.5f },	{ -0.5f, 0.5f, 0.5f } // Front face
		};

		for ( int i = 0; i < 8; ++i )
		{
			_cube.colors.push_back( getRandomVec3f() );
		}

		_cube.indices = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 0, 3, 7, 7, 4, 0,
						  1, 5, 6, 6, 2, 1, 3, 2, 6, 6, 7, 3, 0, 1, 5, 5, 4, 0 };

		_cube.transformation = glm::mat4( 1.0f );
		glProgramUniformMatrix4fv( _programID, _modelMatrixLocation, 1, GL_FALSE, glm::value_ptr( modelMatrix ) );
		modelMatrix = glm::scale( glm::mat4( 1.0f ), glm::vec3( 0.8f ) );
	}

	void LabWork3::_initBuffers()
	{
		glGenVertexArrays( 1, &_cube.VAO );
		glBindVertexArray( _cube.VAO );

		glGenBuffers( 1, &_cube.VBO );
		glBindBuffer( GL_ARRAY_BUFFER, _cube.VBO );

		std::vector<float> vertexData;
		for ( size_t i = 0; i < _cube.positions.size(); ++i )
		{
			vertexData.push_back( _cube.positions[ i ].x );
			vertexData.push_back( _cube.positions[ i ].y );
			vertexData.push_back( _cube.positions[ i ].z );
			vertexData.push_back( _cube.colors[ i ].x );
			vertexData.push_back( _cube.colors[ i ].y );
			vertexData.push_back( _cube.colors[ i ].z );
		}

		glBufferData( GL_ARRAY_BUFFER, vertexData.size() * sizeof( float ), vertexData.data(), GL_STATIC_DRAW );

		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void *)0 ); // Positions
		glEnableVertexAttribArray( 0 );

		glVertexAttribPointer(
			1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void *)( 3 * sizeof( float ) ) ); // Colors
		glEnableVertexAttribArray( 1 );

		glGenBuffers( 1, &_cube.EBO );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _cube.EBO );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER,
					  _cube.indices.size() * sizeof( unsigned int ),
					  _cube.indices.data(),
					  GL_STATIC_DRAW );

		glBindVertexArray( 0 );
	}

	void LabWork3::_updateViewMatrix()
	{
		GLuint viewMatrixLocation = glGetUniformLocation( _programID, "uViewMatrix" );
		glUniformMatrix4fv( viewMatrixLocation, 1, GL_FALSE, glm::value_ptr( _camera.getViewMatrix() ) );
	}

	void LabWork3::_updateProjectionMatrix()
	{
		GLuint projectionMatrixLocation = glGetUniformLocation( _programID, "uProjectionMatrix" );
		glUniformMatrix4fv( projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr( _camera.getProjectionMatrix() ) );
	}

	void LabWork3::_initCamera()
	{
		_camera.setPosition( glm::vec3( 0.f, 1.f, 3.f ) );
		_camera.setScreenSize( _windowWidth, _windowHeight );
	}

} // namespace M3D_ISICG
