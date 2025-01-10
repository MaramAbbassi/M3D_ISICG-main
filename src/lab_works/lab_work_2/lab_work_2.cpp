#include "lab_work_2.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork2::_shaderFolder = "src/lab_works/lab_work_2/shaders/";

	LabWork2::~LabWork2()
	{
		if ( _vbo != GL_INVALID_INDEX )
		{
			glDeleteBuffers( 1, &_vbo );
			_vbo = GL_INVALID_INDEX;
		}
		if ( _vao != GL_INVALID_INDEX )
		{
			glDisableVertexArrayAttrib( _vao, 0 ); 
			glDeleteVertexArrays( 1, &_vao );	   
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

	bool LabWork2::init()
	{
		std::cout << "Initializing lab work 2..." << std::endl;
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );


		std::vector<glm::vec2> quadVertices = {
			glm::vec2( 0.5f, 0.5f ),
			glm::vec2( -0.5f, 0.5f ),
			glm::vec2( 0.5f, -0.5f ),
			glm::vec2( -0.5f, -0.5f ),
		};

		std::vector<GLuint> indices = { 0, 1, 2, 1, 3, 2 };

		std::vector<glm::vec3> vertexColors = { glm::vec3( 0.0f, 1.0f, 0.0f ),
												glm::vec3( 1.0f, 0.0f, 0.0f ),
												glm::vec3( 0.0f, 0.0f, 1.0f ),
												glm::vec3( 1.0f, 0.0f, 1.0f ) };

		glCreateBuffers( 1, &_vbo );
		glNamedBufferData( _vbo, quadVertices.size() * sizeof( glm::vec2 ), quadVertices.data(), GL_STATIC_DRAW );

		glCreateVertexArrays( 1, &_vao );

		glEnableVertexArrayAttrib( _vao, 0 );

		glVertexArrayAttribFormat( _vao, 0, 2, GL_FLOAT, GL_FALSE, 0 );

		glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( glm::vec2 ) );

		glVertexArrayAttribBinding( _vao, 0, 0 );

		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _ebo, indices.size() * sizeof( GLuint ), indices.data(), GL_STATIC_DRAW );
		glVertexArrayElementBuffer( _vao, _ebo );

		glCreateBuffers( 1, &_colorVBO );
		glNamedBufferData( _colorVBO, vertexColors.size() * sizeof( glm::vec3 ), vertexColors.data(), GL_STATIC_DRAW );

		glEnableVertexArrayAttrib( _vao, 1 );
		glVertexArrayAttribFormat( _vao, 1, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( _vao, 1, _colorVBO, 0, sizeof( glm::vec3 ) );
		glVertexArrayAttribBinding( _vao, 1, 1 );

		const std::string vertexShaderStr	= readFile( _shaderFolder + "lw2.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw2.frag" );

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

		_uTranslationXLocation = glGetUniformLocation( _programID, "uTranslationX" );
		glProgramUniform1f( _programID, _uTranslationXLocation, 0.0f );

		_uBrightnessLocation = glGetUniformLocation( _programID, "uBrightness" );
		glProgramUniform1f( _programID, _uBrightnessLocation, _brightness );


		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork2::animate( const float p_deltaTime )
	{
		_time += p_deltaTime;
		float translation = glm::sin( _time ) * 0.5f;
		glProgramUniform1f( _programID, _uTranslationXLocation, translation );
	}

	void LabWork2::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glUseProgram( _programID );
		glBindVertexArray( _vao );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
		glBindVertexArray( 0 );
	}

	void LabWork2::handleEvents( const SDL_Event & p_event ) {}

	void LabWork2::displayUI()
	{
		
		ImGui::Begin( "Settings lab work 2" );
		if ( ImGui::SliderFloat( "Brightness", &_brightness, 0.0f, 1.0f ) )
		{
			glProgramUniform1f( _programID, _uBrightnessLocation, _brightness );
		}
		if ( ImGui::ColorEdit3( "Background Color", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w ); //
		}

		ImGui::End();
	}

} // namespace M3D_ISICG
