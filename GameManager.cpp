#include "GameManager.h"
#include "GameException.h"
#include "GLUtils/GLUtils.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <assert.h>
#include <stdexcept>
#include <algorithm>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform2.hpp>

using std::cerr;
using std::endl;
using GLUtils::BO;
using GLUtils::Program;

const float GameManager::s_near_plane = 0.5f;
const float GameManager::s_far_plane = 30.0f;
const float GameManager::s_fovy = 45.0f;
const float GameManager::s_cube_scale = GameManager::s_far_plane*0.75f;

const float GameManager::s_cube_vertices_data[] = {
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,

    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,

    0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
	
    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
	
    -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, -0.5f,

    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
};

const float GameManager::s_cube_normals_data[] = {
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,

    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,

    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,

    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
	
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,

    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
};


inline void checkSDLError(int line = -1) {
#ifndef NDEBUG
	const char *error = SDL_GetError();
	if (*error != '\0') {
		std::cout << "SDL Error";
		if (line != -1) {
			std::cout << ", line " << line;
		}
		std::cout << ": " << error << std::endl;
		SDL_ClearError();
	}
#endif
}

//			----	CONTRUCTOR	----
GameManager::GameManager() : m_display_shadow_map(false) {
	m_my_timer.restart();
	m_zoom = 1;
	m_light.position = glm::vec3(10, 0, 0);

	render_model = &GameManager::phong_rendering; // We'll start rendering our models with regular phong shading
}

GameManager::~GameManager() {
}

void GameManager::createOpenGLContext() {
	//Set OpenGL major an minor versions
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Set OpenGL attributes
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Use double buffering
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16); // Use framebuffer with 16 bit depth buffer
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); // Use framebuffer with 8 bit for red
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8); // Use framebuffer with 8 bit for green
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8); // Use framebuffer with 8 bit for blue
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // Use framebuffer with 8 bit for alpha
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	// Initalize video
	m_main_window = SDL_CreateWindow("Westerdals - PG6200 Assignment 3", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		s_window_width, s_window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!m_main_window) {
		THROW_EXCEPTION("SDL_CreateWindow failed");
	}
	checkSDLError(__LINE__);

	m_main_context = SDL_GL_CreateContext(m_main_window);
	SDL_GL_SetSwapInterval(1);
	checkSDLError(__LINE__);

	m_cam_trackball.setWindowSize(s_window_width, s_window_height);

	// Init glew
	// glewExperimental is required in openGL 3.3
	// to create forward compatible contexts 
	glewExperimental = GL_TRUE;
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		std::stringstream err;
		err << "Error initializing GLEW: " << glewGetErrorString(glewErr);
		THROW_EXCEPTION(err.str());
	}

	// Unfortunately glewInit generates an OpenGL error, but does what it's
	// supposed to (setting function pointers for core functionality).
	// Lets do the ugly thing of swallowing the error....
	glGetError();


	glViewport(0, 0, s_window_width, s_window_height);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	CHECK_GL_ERRORS();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GameManager::init() {
	//Create opengl context before we do anything OGL-stuff
	createOpenGLContext();

	//Initialize IL and ILU
	ilInit();
	iluInit();

	//////////////////////////////////////////
	///	INITIALIZING OBJECTS THAT WE NEED  ///
	//////////////////////////////////////////

	m_model.reset(new Model("models/bunny.obj", false));

	m_cube_vertices.reset(new BO<GL_ARRAY_BUFFER>(s_cube_vertices_data, sizeof(s_cube_vertices_data)));
	m_cube_normals.reset(new BO<GL_ARRAY_BUFFER>(s_cube_normals_data, sizeof(s_cube_normals_data)));

	m_shadow_fbo.reset(new ShadowFBO(s_shadow_map_width, s_shadow_map_height));

	m_diffuse_cubemap.reset(new GLUtils::CubeMap("cubemaps/diffuse/", "jpg"));

#ifdef _DEBUG
	CHECK_GL_ERRORS();
#endif

	//////////////////////////////////////////////////////
	////	INITIALIZING CAMERA AND LIGHT MATRICES	 /////
	//////////////////////////////////////////////////////


	//Set the matrices we will use
	m_camera.projection = glm::perspective(s_fovy/m_zoom,
			s_window_width / (float) s_window_height, s_near_plane, s_far_plane);
	m_camera.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));

	m_light.projection = glm::perspective(90.0f, 1.0f, s_near_plane, s_far_plane);
	m_light.view = glm::lookAt(m_light.position, glm::vec3(0), glm::vec3(0.0, 1.0, 0.0));

	////////////////////////////////////////////////////////////////////////////////////
	////		Createing the random transformations and colors for the bunnys		////
	////////////////////////////////////////////////////////////////////////////////////


	srand(static_cast<int>(time(NULL)));
	for (int i=0; i < s_number_of_models; ++i) {
		float tx = rand() / (float) RAND_MAX - 0.5f;
		float ty = rand() / (float) RAND_MAX - 0.5f;
		float tz = rand() / (float) RAND_MAX - 0.5f;

		glm::mat4 transformation = m_model->getTransform();
		transformation = glm::translate(transformation, glm::vec3(tx, ty, tz));

		m_model_matrices.push_back(transformation);
    m_model_colors.push_back(glm::vec3(tx+0.5, ty+0.5, tz+0.5));
	}


	//////////////////////////////////////////////////////
	//// START OF SHADER PROGRAM COMPILATION STAGE	  ////
	//////////////////////////////////////////////////////

#ifdef _DEBUG
	std::cout << "Attempting to compile phong-program..." << std::endl;
#endif

	m_phong_program.reset(new Program("shaders/phong.vert", "shaders/phong.geom", "shaders/phong.frag"));

#ifdef _DEBUG
	std::cout << "Checking for errors..." << std::endl;
	CHECK_GL_ERRORS();
	std::cout << "Attempting to compile shadow-program..." << std::endl;
#endif

	m_shadow_program.reset(new Program("shaders/shadow.vert", "shaders/shadow.frag"));

#ifdef _DEBUG
	std::cout << "Checking for errors..." << std::endl;
	CHECK_GL_ERRORS();
	std::cout << "Attempting to compile hiddenline-program..." << std::endl;
#endif

	m_hiddenline_program.reset(new Program("shaders/hiddenline.vert", "shaders/hiddenline.geom", "shaders/hiddenline.frag"));

#ifdef _DEBUG
	std::cout << "Checking for errors..." << std::endl;
	CHECK_GL_ERRORS();
	std::cout << "Attempting to compile fbo-program..." << std::endl;
#endif

	m_fbo_program.reset(new Program("shaders/fbo.vert", "shaders/fbo.frag"));

#ifdef _DEBUG
	std::cout << "Checking for errors..." << std::endl;
	CHECK_GL_ERRORS();
	std::cout << "Attempting to compile wireframe-program..." << std::endl;
#endif

	m_wireframe_program.reset(new Program("shaders/wireframe.vert", "shaders/wireframe.geom", "shaders/wireframe.frag"));

#ifdef _DEBUG
	std::cout << "Checking for errors..." << std::endl;
	CHECK_GL_ERRORS();
	std::cout << "All programs compiled successfully!" << std::endl;
#endif

	//////////////////////////////////////////////////////
	////    END OF SHADER PROGRAM COMPILATION STAGE	  ////
	//////////////////////////////////////////////////////


	//////////////////////////////////////////////
	////   START OF VAO INITIALIZING  STAGE	  ////
	//////////////////////////////////////////////

	// Generating 2 VAOS!
	glGenVertexArrays(2, &m_vao[0]);

	glBindVertexArray(m_vao[0]); 

	// Sending our mesh as input to our shaders
	m_model->getVertices()->bind();

	m_phong_program->setAttributePointer("position", 3); // Phong program needs access to the vertices

	m_shadow_program->setAttributePointer("position", 3); // Shadow program needs access to the vertices

	m_model->getNormals()->bind();

	m_phong_program->setAttributePointer("normal", 3); // Phong program needs access to the normals

	m_model->getVertices()->unbind(); //Unbinds both vertices and normals

	glBindVertexArray(m_vao[1]);

	// Sending out walls as input to our phong-shader

	m_cube_vertices->bind();
	m_phong_program->setAttributePointer("position", 3);

	m_cube_normals->bind();
	m_phong_program->setAttributePointer("normal", 3);

	m_model->getVertices()->unbind(); //Unbinds both vertices and normals

	glBindVertexArray(0);

#ifdef _DEBUG
	CHECK_GL_ERRORS();
#endif

	// Generating a third VAO for our fbo-program
	glGenVertexArrays(1, &m_fbo_vao);

	init_fbo(); // Initialize our fbo


	////////////////////////////////////////////////
	////     END OF VAO INITIALIZING STAGE	   ////
	///////////////////////////////////////////////

#ifdef _DEBUG
	std::cout << "All resources has been initialized..." << std::endl;
	CHECK_GL_ERRORS();
#endif
}

void GameManager::init_fbo()
{
		m_fbo_program->use();

		glUniform1i(m_fbo_program->getUniform("fbo_texture"), 0); 

		glBindVertexArray(m_fbo_vao);

		static float positions[8] = {
			-1.0, 1.0,
			-1.0, -1.0,
			1.0, 1.0,
			1.0, -1.0
		};

		glGenBuffers(1, &m_fbo_vertex_bo);
		glBindBuffer(GL_ARRAY_BUFFER, m_fbo_vertex_bo);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), &positions[0], GL_STATIC_DRAW);

		m_fbo_program->setAttributePointer("in_Position", 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

#ifdef _DEBUG
	CHECK_GL_ERRORS();
#endif
}


void GameManager::renderColorPass() const {

	glViewport(0, 0, s_window_width, s_window_height);

	// Makeing sure that we are rendering to screen
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

	//Create the new view matrix that takes the trackball view into account
	glm::mat4 view_matrix_new = m_camera.view * m_cam_trackball.getTransform();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_phong_program->use();

	// Binding all textures that we'll be using

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_shadow_fbo->getTexture());

	m_diffuse_cubemap->bindTexture(GL_TEXTURE1);

	/////////////////////////////////////////////////////////
	/////		START OF RENDERING STEP FOR THE WALLS	/////
	/////////////////////////////////////////////////////////

	glBindVertexArray(m_vao[1]);

	glm::mat4 model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(s_cube_scale));
	glm::mat4 model_matrix_inverse = glm::inverse(model_matrix);
	glm::mat4 modelview_matrix = view_matrix_new*model_matrix;
	glm::mat4 modelview_matrix_inverse = glm::inverse(modelview_matrix);
	glm::mat4 modelviewprojection_matrix = m_camera.projection * modelview_matrix;
	glm::vec3 light_pos = glm::mat3(model_matrix_inverse) * m_light.position/model_matrix_inverse[3].w;

	// setting uniforms for phong-program
	glUniform3fv(m_phong_program->getUniform("phong_light_pos"), 1, glm::value_ptr(light_pos));
	glUniform3fv(m_phong_program->getUniform("phong_color"), 1, glm::value_ptr(glm::vec3(1.0f, 0.8f, 0.8f)));
	glUniformMatrix4fv(m_phong_program->getUniform("phong_modelviewprojection_matrix"), 1, 0, glm::value_ptr(modelviewprojection_matrix));
	glUniformMatrix4fv(m_phong_program->getUniform("phong_modelview_matrix_inverse"), 1, 0, glm::value_ptr(modelview_matrix_inverse));

	glProgramUniform1i(m_phong_program->get_program_id(), m_phong_program->getUniform("phong_shadow_map"), 0);
	glProgramUniform1i(m_phong_program->get_program_id(), m_phong_program->getUniform("phong_cube_map"), 1);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	/////////////////////////////////////////////////////////
	/////		END OF RENDERING STEP FOR THE WALLS		/////
	/////////////////////////////////////////////////////////


#ifdef _DEBUG
	CHECK_GL_ERRORS();
#endif

	// Bias matrix for rendering shadows
	glm::mat4 T = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
		);

	/////////////////////////////////////////////////////////
	/////		START OF RENDERING STEP FOR THE MODELS	/////
	/////////////////////////////////////////////////////////

	glBindVertexArray(m_vao[0]);

	for (int i=0; i < s_number_of_models; ++i) {
		glm::mat4 model_matrix = m_model_matrices.at(i);
		glm::mat4 model_matrix_inverse = glm::inverse(model_matrix);
		glm::mat4 modelview_matrix = view_matrix_new * model_matrix;
		glm::mat4 modelview_matrix_inverse = glm::inverse(modelview_matrix);
		glm::mat4 modelviewprojection_matrix = m_camera.projection*modelview_matrix;
		glm::vec3 light_pos = glm::mat3(model_matrix_inverse) * m_light.position/model_matrix_inverse[3].w;

		glm::mat4 light_transform = T * m_light.projection * m_light.view * model_matrix;

		// This function will render the models with the shader program that the user has chosen to use.
		(this->*render_model)(glm::value_ptr(modelviewprojection_matrix), glm::value_ptr(modelview_matrix_inverse), glm::value_ptr(light_transform), glm::value_ptr(light_pos), i);

#ifdef _DEBUG
		CHECK_GL_ERRORS();
#endif

		glDrawArrays(GL_TRIANGLES, 0, m_model->getNumberOfVertices());
	}

	/////////////////////////////////////////////////////////
	/////		END OF RENDERING STEP FOR THE MODELS	/////
	/////////////////////////////////////////////////////////

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_diffuse_cubemap->unbindTexture();

	glUseProgram(0);

#ifdef _DEBUG
	CHECK_GL_ERRORS();
#endif
}

void GameManager::renderShadowPass() {
	//Render the scene from the light, with the lights projection, etc. into the shadow_fbo. Store only the depth values

	glViewport(0, 0, s_shadow_map_width, s_shadow_map_height);

	m_shadow_fbo->bind();
	m_shadow_program->use();

	glClear(GL_DEPTH_BUFFER_BIT);

	m_model->getVertices()->bind();

	m_shadow_program->setAttributePointer("position", 3);

	glm::mat4 transform;

  for(auto & model_matrix : m_model_matrices)
    {
    transform = m_light.projection * m_light.view * model_matrix; // This matrix will transform the models into the light's clip space
		glUniformMatrix4fv(m_shadow_program->getUniform("shadow_light_transform"), 1, 0, glm::value_ptr(transform));
		glDrawArrays(GL_TRIANGLES, 0, m_model->getNumberOfVertices());
  }

	m_shadow_program->disuse();
	m_shadow_fbo->unbind();

#ifdef _DEBUG
	CHECK_GL_ERRORS();
#endif

}

// This function displays the shadow_map in the lower left corner
void GameManager::renderFBO() const
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(m_fbo_vao);
	m_fbo_program->use();

	glm::mat3 fbo_transform = glm::mat3(glm::vec3(0.5, 0.0, 0.0), glm::vec3(0.0, 0.5, 0.0), glm::vec3(-0.5, -0.5, 0.5));

	glProgramUniformMatrix3fv(m_fbo_program->get_program_id(), m_fbo_program->getUniform("fbo_transform"), 1, 0, glm::value_ptr(fbo_transform));

	glBindTexture(GL_TEXTURE_2D, m_shadow_fbo->getTexture());
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	m_fbo_program->disuse();
	glBindVertexArray(0);

	glDisable(GL_BLEND);

#ifdef _DEBUG
	CHECK_GL_ERRORS();
#endif
}


void GameManager::render() {
	//Rotate the light a bit
	float elapsed = static_cast<float>(m_my_timer.elapsedAndRestart());
	glm::mat4 rotation = glm::rotate(elapsed, glm::vec3(0.0f, 1.0f, 0.0f));

	m_light.position = glm::mat3(rotation) * m_light.position;
	m_light.view = glm::lookAt(m_light.position,  glm::vec3(0), glm::vec3(0.0, 1.0, 0.0));

	// Create shadow-map first!
	renderShadowPass();

	// Render the screen!
	renderColorPass();

	if (m_display_shadow_map) {
		renderFBO();
	}
}

void GameManager::play() {
	bool doExit = false;

	//SDL main loop
	while (!doExit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {// poll for pending events
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				m_cam_trackball.rotateBegin(event.motion.x, event.motion.y);
				break;
			case SDL_MOUSEBUTTONUP:
				m_cam_trackball.rotateEnd(event.motion.x, event.motion.y);
				break;
			case SDL_MOUSEMOTION:
				m_cam_trackball.rotate(event.motion.x, event.motion.y, m_zoom);
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym) {
				case SDLK_ESCAPE: //Esc
					doExit = true;
					break;
				case SDLK_q: //Ctrl+q
					if (event.key.keysym.mod & KMOD_CTRL) doExit = true;
					break;
				case SDLK_PLUS:
					zoomIn();
					break;
				case SDLK_MINUS:
					zoomOut();
					break;
				case SDLK_1:
					render_model = &GameManager::phong_rendering;
					break;
				case SDLK_2:
					render_model = &GameManager::wireframe_rendering;
					break;
				case SDLK_3:
					render_model = &GameManager::hiddenline_rendering;
					break;
				case SDLK_t:
					m_display_shadow_map = !m_display_shadow_map;
					break;
				}
				break;
			case SDL_QUIT: //e.g., user clicks the upper right x
				doExit = true;
				break;
			}
		}

		//Render, and swap front and back buffers
		render();
		SDL_GL_SwapWindow(m_main_window);
	}
	quit();
}

void GameManager::zoomIn() {
	m_zoom *= 1.1f;
	m_camera.projection = glm::perspective(s_fovy/m_zoom,
                                         s_window_width / static_cast<float>(s_window_height), s_near_plane, s_far_plane);
}

void GameManager::zoomOut() {
	m_zoom = std::max(m_zoom*0.9f, 0.5f);
	m_camera.projection = glm::perspective(s_fovy / m_zoom,
                                         s_window_width / static_cast<float>(s_window_height), s_near_plane, s_far_plane);
}

void GameManager::phong_rendering(const GLfloat * modelviewprojection, const GLfloat * modelview_inverse, const GLfloat * light_transform, const GLfloat * light_pos, int iteration) const
{
	// Setting all uniforms for phong-program
	glProgramUniform1i(m_phong_program->get_program_id(), m_phong_program->getUniform("phong_shadow_map"), 0);

	glProgramUniform3fv(m_phong_program->get_program_id(), m_phong_program->getUniform("phong_light_pos"), 1, light_pos);
	glProgramUniform3fv(m_phong_program->get_program_id(), m_phong_program->getUniform("phong_color"), 1, glm::value_ptr(m_model_colors.at(iteration)));

	glProgramUniformMatrix4fv(m_phong_program->get_program_id(), m_phong_program->getUniform("phong_modelviewprojection_matrix"), 1, 0, modelviewprojection);
	glProgramUniformMatrix4fv(m_phong_program->get_program_id(), m_phong_program->getUniform("phong_modelview_matrix_inverse"), 1, 0, modelview_inverse);
	glProgramUniformMatrix4fv(m_phong_program->get_program_id(), m_phong_program->getUniform("phong_light_transform"), 1, 0, light_transform);

	glProgramUniform1i(m_phong_program->get_program_id(), m_phong_program->getUniform("phong_shadow_map"), 0);
	glProgramUniform1i(m_phong_program->get_program_id(), m_phong_program->getUniform("phong_cube_map"), 1);
}

void GameManager::wireframe_rendering(const GLfloat * modelviewprojection, const GLfloat * modelview_inverse, const GLfloat * light_transform, const GLfloat * light_pos, int iteration) const
{
	m_wireframe_program->use();

	phong_rendering(modelviewprojection, modelview_inverse, light_transform, light_pos, iteration);

	// setting all uniforms for wireframe-program
	glProgramUniformMatrix4fv(m_wireframe_program->get_program_id(), m_wireframe_program->getUniform("wireframe_projection_matrix"), 1, 0, modelviewprojection);
}

void GameManager::hiddenline_rendering(const GLfloat * modelviewprojection, const GLfloat * modelview_inverse, const GLfloat * light_transform, const GLfloat * light_pos, int iteration) const
{
	m_hiddenline_program->use();

	phong_rendering(modelviewprojection, modelview_inverse, light_transform, light_pos, iteration);

	// setting all uniforms for hiddenline-program
	glProgramUniformMatrix4fv(m_hiddenline_program->get_program_id(), m_hiddenline_program->getUniform("hiddenline_projection_matrix"), 1, 0, modelviewprojection);
	glProgramUniformMatrix4fv(m_hiddenline_program->get_program_id(), m_hiddenline_program->getUniform("hiddenline_modelview_matrix_inverse"), 1, 0, modelview_inverse);
	glProgramUniformMatrix4fv(m_hiddenline_program->get_program_id(), m_hiddenline_program->getUniform("hiddenline_light_transform"), 1, 0, light_transform);

	glProgramUniform3fv(m_hiddenline_program->get_program_id(), m_hiddenline_program->getUniform("hiddenline_light_pos"), 1, light_pos);
	glProgramUniform3fv(m_hiddenline_program->get_program_id(), m_hiddenline_program->getUniform("hiddenline_color"), 1, glm::value_ptr(m_model_colors.at(iteration)));

	glProgramUniform1i(m_hiddenline_program->get_program_id(), m_hiddenline_program->getUniform("hiddenline_shadow_map"), 0);
	glProgramUniform1i(m_hiddenline_program->get_program_id(), m_hiddenline_program->getUniform("hiddenline_cube_map"), 1);
}


void GameManager::quit() {
	std::cout << "Bye bye..." << std::endl;
}
