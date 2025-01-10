#include "lab_work_1.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork1::_shaderFolder = "src/lab_works/lab_work_1/shaders/";

	LabWork1::~LabWork1()
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
	}

	bool LabWork1::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// houni bdit

		std::vector<glm::vec2> triangleVertices
			= { glm::vec2( 0.5f, 0.5f ), glm::vec2( -0.5f, 0.5f ), glm::vec2( 0.5f, -0.5f ) };

		glCreateBuffers( 1, &_vbo );
		glNamedBufferData(
			_vbo, triangleVertices.size() * sizeof( glm::vec2 ), triangleVertices.data(), GL_STATIC_DRAW );

		glCreateVertexArrays( 1, &_vao );

		glEnableVertexArrayAttrib( _vao, 0 );

		glVertexArrayAttribFormat( _vao, 0, 2, GL_FLOAT, GL_FALSE, 0 );

		glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( glm::vec2 ) );

		glVertexArrayAttribBinding( _vao, 0, 0 );

		const std::string vertexShaderStr	= readFile( _shaderFolder + "lw1.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw1.frag" );

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

		// houni kamelt

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork1::animate( const float p_deltaTime ) {}

	void LabWork1::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
		//glClear( GL_COLOR_BUFFER_BIT );
		glUseProgram( _programID );
		glBindVertexArray( _vao );
		glDrawArrays( GL_TRIANGLES, 0, 3 );
		glBindVertexArray( 0 );
	}

	void LabWork1::handleEvents( const SDL_Event & p_event ) {}

	void LabWork1::displayUI()
	{
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

} // namespace M3D_ISICG
