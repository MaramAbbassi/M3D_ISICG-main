#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "lab_work_6.hpp"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork6::_shaderFolder = "src/lab_works/lab_work_6/shaders/";

	LabWork6::~LabWork6()
	{
		_bunnyModel.cleanGL();
		//glDeleteProgram( _programID );
		glDeleteProgram( _geometryPassProgram ); 
		glDeleteProgram( _shadingPassProgram );
	}

	bool LabWork6::init()
	{
		std::cout << "Initializing lab work 6..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		_initGeometryPassProgram();
		_initShadingPassProgram();
		glEnable( GL_DEPTH_TEST );
		_initCamera();

		//_bunnyModel.load( "Bunny", "data/models/bunny2/bunny_2.obj" );
		_bunnyModel.load( "Bunny", "data/models/sponza/sponza.obj" );

		_bunnyModel._transformation = glm::scale( _bunnyModel._transformation, Vec3f( 0.03f, 0.03f, 0.03f ) );

		_updateUniforms();
		_initGBuffuer();
		CreateCube();

		glUseProgram( _programID );
		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork6::animate( const float p_deltaTime ) {}

	void LabWork6::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // Clear the color buffer.
		// glEnable( GL_BLEND );
		// glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		 _updateUniforms();

		_uMVPMatrixLoc	  = glGetUniformLocation( _geometryPassProgram, "uMVPMatrix" );
		 _uNormalMatrixLoc = glGetUniformLocation( _geometryPassProgram, "uNormalMatrix" );
		_uViewMatrixLoc	  = glGetUniformLocation( _geometryPassProgram, "uViewMatrix" );
		 _uModelMatrixLoc  = glGetUniformLocation( _geometryPassProgram, "uModelMatrix" );
		Mat4f mvp		  = _camera.getProjectionMatrix() * _camera.getViewMatrix() * _bunnyModel._transformation;
		Mat3f NormalMatrix
			= Mat3f( glm::transpose( glm::inverse( _camera.getViewMatrix() * _bunnyModel._transformation ) ) );

		glProgramUniformMatrix3fv(
			_geometryPassProgram, _uNormalMatrixLoc, 1, GL_FALSE, glm::value_ptr( NormalMatrix ) );
		glProgramUniformMatrix4fv( _geometryPassProgram, _uMVPMatrixLoc, 1, GL_FALSE, glm::value_ptr( mvp ) );
		glProgramUniformMatrix4fv(
			_geometryPassProgram, _uViewMatrixLoc, 1, GL_FALSE, glm::value_ptr( _camera.getViewMatrix() ) );
		glProgramUniformMatrix4fv(
			_geometryPassProgram, _uModelMatrixLoc, 1, GL_FALSE, glm::value_ptr( _bunnyModel._transformation ) );
		glProgramUniformMatrix4fv(
			_geometryPassProgram, _uProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr( _camera.getProjectionMatrix() ) );

		//_bunnyModel.render( _programID );
		
		_geometryPass();
		_shadingPass();
		glBindFramebuffer( GL_READ_FRAMEBUFFER, FBO );
		glReadBuffer( outTexture );
		glBlitFramebuffer( 0,
						   0,
						   _windowWidth,
						   _windowHeight, 
						   0,
						   0,
						   _windowWidth,
						   _windowHeight, 
						   GL_COLOR_BUFFER_BIT,
						   GL_NEAREST ); 
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void LabWork6::handleEvents( const SDL_Event & p_event )
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

	void LabWork6::displayUI()
	{
		ImGui::Begin( "Settings lab work 6" );

		if ( ImGui::BeginListBox( "Debug G-Buffer Textures" ) )
		{
			if ( ImGui::Selectable( "PosFragments", outTexture == GL_COLOR_ATTACHMENT0 ) )
				outTexture = GL_COLOR_ATTACHMENT0;
			if ( ImGui::Selectable( "Normales", outTexture == GL_COLOR_ATTACHMENT1 ) )
				outTexture = GL_COLOR_ATTACHMENT1;
			
			if ( ImGui::Selectable( "Color_Diffuse", outTexture == GL_COLOR_ATTACHMENT3 ) )
				outTexture = GL_COLOR_ATTACHMENT3;
			if ( ImGui::Selectable( "Color_Spec", outTexture == GL_COLOR_ATTACHMENT4 ) )
				outTexture = GL_COLOR_ATTACHMENT4;

			ImGui::EndListBox();
		}

		if ( ImGui::ColorEdit3( "Background", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}

		if ( ImGui::SliderFloat( "fovy", &_fovy, 10.f, 160.f, "%01.f" ) )
		{
			_camera.setFovy( _fovy );
		}
		if ( ImGui::SliderFloat( "Speed", &_cameraSpeed, 0.1f, 10.f, "%01.1f" ) ) {}

		ImGui::End();
	}


	void LabWork6::resize( const int p_width, const int p_height )
	{
		BaseLabWork::resize( p_width, p_height );
		_camera.setScreenSize( p_width, p_height );
	}
	/*
	bool LabWork6::_initProgram()
	{
		const std::string vertexShaderSrc	= readFile( _shaderFolder + "mesh_texture.vert" );
		const std::string fragmentShaderSrc = readFile( _shaderFolder + "mesh_texture.frag" );

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
	}*/

	void LabWork6::_initCamera()

	{
		_camera.setScreenSize( _windowWidth, _windowHeight );
		//  Pour bunny
		/* _camera.setPosition( Vec3f( 0, 0, 3 ) );
		_camera.setLookAt( Vec3f( 0, 0, 1 ) );*/

		//  Pour Sponza
		_camera.setPosition( Vec3f( 4, 6, -2 ) );
		_camera.setLookAt( Vec3f( 5.f, 5.9f, -3.f ) );
	}

	void LabWork6::_updateUniforms()
	{
		glProgramUniform3fv( _geometryPassProgram,
							 glGetUniformLocation( _geometryPassProgram, "CameraPosition" ),
							 1,
							 glm::value_ptr( _camera.getPosition() ) );
		glProgramUniform3fv( _geometryPassProgram,
							 glGetUniformLocation( _geometryPassProgram, "LightPos" ),
							 1,
							 glm::value_ptr( lightPosition ) );
		glProgramUniform3fv( _geometryPassProgram,
							 glGetUniformLocation( _geometryPassProgram, "LightColor" ),
							 1,
							 glm::value_ptr( lightColor ) );
		glProgramUniform1i(
			_geometryPassProgram, glGetUniformLocation( _geometryPassProgram, "Blinn_Phong" ), Blinn_Phong );
		glProgramUniform1i(
			_geometryPassProgram, glGetUniformLocation( _geometryPassProgram, "Gauss_light" ), Gauss_light );
		glProgramUniform1i(
			_geometryPassProgram, glGetUniformLocation( _geometryPassProgram, "NormalMaps" ), NormalMaps );
	}

	GLuint LabWork6::_compileShader( GLenum type, const std::string & source )
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
			std::cerr << "Shader Compilation Error: " << log << std::endl;
			glDeleteShader( shader );
			return 0;
		}
		return shader;
	}
	void LabWork6::_initGBuffuer()
	{
		
		glCreateFramebuffers( 1, &FBO );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, FBO );
		

		glCreateTextures( GL_TEXTURE_2D, 1, &_gBufferTextures[ 0 ] );
		glCreateTextures( GL_TEXTURE_2D, 1, &_gBufferTextures[ 1 ] );
		glCreateTextures( GL_TEXTURE_2D, 1, &_gBufferTextures[ 2 ] );
		glCreateTextures( GL_TEXTURE_2D, 1, &_gBufferTextures[ 3 ] );
		glCreateTextures( GL_TEXTURE_2D, 1, &_gBufferTextures[ 4 ] );
		glCreateTextures( GL_TEXTURE_2D, 1, &_gBufferTextures[ 5 ] );

		GLenum drawBuffers[] = {
			GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4,
		};

		glNamedFramebufferTexture( FBO, GL_COLOR_ATTACHMENT0, _gBufferTextures[ 0 ], 0 );
		glNamedFramebufferTexture( FBO, GL_COLOR_ATTACHMENT1, _gBufferTextures[ 1 ], 0 );
		glNamedFramebufferTexture( FBO, GL_COLOR_ATTACHMENT2, _gBufferTextures[ 2 ], 0 );
		glNamedFramebufferTexture( FBO, GL_COLOR_ATTACHMENT3, _gBufferTextures[ 3 ], 0 );
		glNamedFramebufferTexture( FBO, GL_COLOR_ATTACHMENT4, _gBufferTextures[ 4 ], 0 );
		glNamedFramebufferTexture( FBO, GL_DEPTH_ATTACHMENT, _gBufferTextures[ 5 ], 0 );

		glTextureStorage2D( _gBufferTextures[ 0 ], 1, GL_RGB32F, _windowWidth, _windowHeight );
		glTextureStorage2D( _gBufferTextures[ 1 ], 1, GL_RGB32F, _windowWidth, _windowHeight );
		glTextureStorage2D( _gBufferTextures[ 2 ], 1, GL_RGB32F, _windowWidth, _windowHeight );
		glTextureStorage2D( _gBufferTextures[ 3 ], 1, GL_RGB32F, _windowWidth, _windowHeight );
		glTextureStorage2D( _gBufferTextures[ 4 ], 1, GL_RGB32F, _windowWidth, _windowHeight );
		glTextureStorage2D( _gBufferTextures[ 5 ], 1, GL_DEPTH_COMPONENT32F, _windowWidth, _windowHeight );

		glTextureParameteri( _gBufferTextures[ 0 ], GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( _gBufferTextures[ 0 ], GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		glTextureParameteri( _gBufferTextures[ 1 ], GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( _gBufferTextures[ 1 ], GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		glTextureParameteri( _gBufferTextures[ 2 ], GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( _gBufferTextures[ 2 ], GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		glTextureParameteri( _gBufferTextures[ 3 ], GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( _gBufferTextures[ 3 ], GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		glTextureParameteri( _gBufferTextures[ 4 ], GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( _gBufferTextures[ 4 ], GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		glTextureParameteri( _gBufferTextures[ 5 ], GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( _gBufferTextures[ 5 ], GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		glNamedFramebufferDrawBuffers( FBO, 5, drawBuffers ); 

		if ( glCheckNamedFramebufferStatus( FBO, GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		{
			std::cout << "Err: Gbuffer n'est pas complet" << std::endl;
		}
		
	}

	void LabWork6::_initGeometryPassProgram()
	{
		const std::string vertexShaderSrc	= readFile( _shaderFolder + "geometry_pass.vert" );
		const std::string fragmentShaderSrc = readFile( _shaderFolder + "geometry_pass.frag" );
		
		const GLuint vertexShader	= _compileShader( GL_VERTEX_SHADER, vertexShaderSrc );
		const GLuint fragmentShader = _compileShader( GL_FRAGMENT_SHADER, fragmentShaderSrc );

		if ( vertexShader == 0 || fragmentShader == 0 )
		{
			std::cerr << "Failed to compile geometry pass shaders." << std::endl;
			return;
		}

		_geometryPassProgram = glCreateProgram(); 
		glAttachShader( _geometryPassProgram, vertexShader );
		glAttachShader( _geometryPassProgram, fragmentShader );
		glLinkProgram( _geometryPassProgram );

		GLint linked;
		glGetProgramiv( _geometryPassProgram, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _geometryPassProgram, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return;
		}

		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );
	}
	void LabWork6::_geometryPass()
	{
		glUseProgram( _geometryPassProgram );
		glEnable( GL_DEPTH_TEST );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, FBO );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		_bunnyModel.render( _geometryPassProgram );
		
		glNamedFramebufferReadBuffer( FBO, outTexture ); // Sélection le buffer à lire
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, FBO );			 // Le FS va écrire dans les textures attachées au FBO
		for ( size_t i = 0; i < 5; i++ )
		{
			glBindTextureUnit( i, _gBufferTextures[ i ] );
		}

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ); // remettre le frame buffer par défaut

		glBlitNamedFramebuffer( FBO,0,0,0,_windowWidth,_windowHeight,0,0,_windowWidth,_windowHeight,GL_COLOR_BUFFER_BIT,GL_NEAREST );
	}

	void LabWork6::_initShadingPassProgram()
	{
		const std::string vertexShaderSrc	= readFile( _shaderFolder + "shading_pass.vert" );
		const std::string fragmentShaderSrc = readFile( _shaderFolder + "shading_pass.frag" );

		const GLuint vertexShader	= _compileShader( GL_VERTEX_SHADER, vertexShaderSrc );
		const GLuint fragmentShader = _compileShader( GL_FRAGMENT_SHADER, fragmentShaderSrc );

		if ( vertexShader == 0 || fragmentShader == 0 )
		{
			std::cerr << "Failed to compile geometry pass shaders." << std::endl;
			return;
		}


		_shadingPassProgram = glCreateProgram();
		glAttachShader( _shadingPassProgram, fragmentShader );
		glAttachShader( _shadingPassProgram, vertexShader );
		glLinkProgram( _shadingPassProgram );

		GLint linked;
		glGetProgramiv( _shadingPassProgram, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _shadingPassProgram, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return;
		}
		LightPOS = glGetUniformLocation( _shadingPassProgram, "LightPOS" );
		glDeleteShader( fragmentShader );
	}
	void LabWork6::CreateCube()
	{
		
		std::vector<glm::vec2> quadVertices = {
			glm::vec2( 0.5f, 0.5f ),
			glm::vec2( -0.5f, 0.5f ),
			glm::vec2( 0.5f, -0.5f ),
			glm::vec2( -0.5f, -0.5f ),
		};

		

		

		glCreateBuffers( 1, &_vbo );
		glNamedBufferData( _vbo, quadVertices.size() * sizeof( glm::vec2 ), quadVertices.data(), GL_STATIC_DRAW );
		glCreateVertexArrays( 1, &_vao );


		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _ebo, indices.size() * sizeof( int ), indices.data(), GL_STATIC_DRAW );
		glEnableVertexArrayAttrib( _vao, 0 );
		glVertexArrayAttribFormat( _vao, 0, 2, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( glm::vec2 ) );

		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _ebo, indices.size() * sizeof( GLuint ), indices.data(), GL_STATIC_DRAW );
		glVertexArrayElementBuffer( _vao, _ebo );
		glCreateBuffers( 1, &_colorVBO );
		glNamedBufferData( _colorVBO, vertexColors.size() * sizeof( glm::vec3 ), vertexColors.data(), GL_STATIC_DRAW );

		glEnableVertexArrayAttrib( _vao, 1 );
		glVertexArrayAttribFormat( _vao, 1, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( _vao, 1, _colorVBO, 0, sizeof( glm::vec3 ) );
		glVertexArrayAttribBinding( _vao, 1, 1 );

	}
	void LabWork6::_shadingPass()
	{
		glUseProgram( _shadingPassProgram );
		glDisable( GL_DEPTH_TEST );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glBindVertexArray( _vao );
		glProgramUniform3fv( _shadingPassProgram, LightPOS, 1, glm::value_ptr( _camera.getPosition() )   );
		
		glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0 );
		for ( int i = 0; i < 6; i++ )
		{
			glBindTextureUnit( i, _gBufferTextures[ i ] );
			glBindTextureUnit( i, 0 );
		}
		glBindVertexArray( 0 );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

} // namespace M3D_ISICG




