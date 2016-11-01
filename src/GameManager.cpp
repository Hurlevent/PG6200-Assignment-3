#include "GameManager.h"
#include "GameException.h"
#include "GLUtils/GLUtils.hpp"
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
using GLUtils::readFile;

const float GameManager::near_plane = 0.5f;
const float GameManager::far_plane = 30.0f;
const float GameManager::fovy = 45.0f;
const float GameManager::cube_scale = GameManager::far_plane*0.75f;

const float GameManager::cube_vertices_data[] = {
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

const float GameManager::cube_normals_data[] = {
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


GameManager::GameManager() : m_rendering_mode(RENDERING_MODE_PHONG) {
	my_timer.restart();
	zoom = 1;
	light.position = glm::vec3(10, 0, 0);

	shadow_map_model.vertices = {
		-0.5f, -0.5f, 0.5f,
		-0.5f, 0, 0.5f,
		0, -0.5f, 0.5f,
		0, -0.5f, 0.5f,
		0, 0, 0.5f,
		-0.5f, 0, 0.5f
	};
	shadow_map_model.normal = glm::vec3(0,0,1.0f);
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
	main_window = SDL_CreateWindow("Westerdals - PG6200 Assignment 3", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!main_window) {
		THROW_EXCEPTION("SDL_CreateWindow failed");
	}
	checkSDLError(__LINE__);

	main_context = SDL_GL_CreateContext(main_window);
	SDL_GL_SetSwapInterval(1);
	checkSDLError(__LINE__);
	
	cam_trackball.setWindowSize(window_width, window_height);

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


	glViewport(0, 0, window_width, window_height);
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

	//Initialize the different stuff we need
	model.reset(new Model("models/bunny.obj", false));
	cube_vertices.reset(new BO<GL_ARRAY_BUFFER>(cube_vertices_data, sizeof(cube_vertices_data)));
	cube_normals.reset(new BO<GL_ARRAY_BUFFER>(cube_normals_data, sizeof(cube_normals_data)));

	shadow_map_model.vbo.reset(new BO<GL_ARRAY_BUFFER>(shadow_map_model.vertices.data(), sizeof(shadow_map_model.vertices.data())));
	
	//Set the matrices we will use
	camera.projection = glm::perspective(fovy/zoom,
			window_width / (float) window_height, near_plane, far_plane);
	camera.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));

	light.projection = glm::perspective(90.0f, 1.0f, near_plane, far_plane);
	light.view = glm::lookAt(light.position, glm::vec3(0), glm::vec3(0.0, 1.0, 0.0));

	//Create the random transformations and colors for the bunnys
	srand(static_cast<int>(time(NULL)));
	for (int i=0; i<n_models; ++i) {
		float tx = rand() / (float) RAND_MAX - 0.5f;
		float ty = rand() / (float) RAND_MAX - 0.5f;
		float tz = rand() / (float) RAND_MAX - 0.5f;

		glm::mat4 transformation = model->getTransform();
		transformation = glm::translate(transformation, glm::vec3(tx, ty, tz));

		model_matrices.push_back(transformation);
		model_colors.push_back(glm::vec3(tx+0.5, ty+0.5, tz+0.5));
	}

	std::cout << "INIT before phong-compile and linking" << std::endl;

	//Create the programs we will use

	phong_program.reset(new Program("shaders/phong.vert", "shaders/phong.geom", "shaders/phong.frag"));

	CHECK_GL_ERRORS();
	std::cout << "INIT after phong-compile and linking" << std::endl;
	std::cout << "INIT before shadow-compile and linking" << std::endl;

	shadow_program.reset(new Program("shaders/shadow.vert", "shaders/shadow.frag"));
	CHECK_GL_ERRORS();

	std::cout << "INIT after shadow-compile and linking" << std::endl;
	try
	{
		hiddenline_program.reset(new Program("shaders/phong.vert", "shaders/hiddenline.geom", "shaders/hiddenline.frag"));
	} catch(std::runtime_error &e)
	{
		std::cerr << "Failed to compile+link shader program: " << e.what() << std::endl;
	}
	
	std::cout << "INIT after hiddenline-compile and linking" << std::endl;
	CHECK_GL_ERRORS();

	//Set uniforms for the programs
	//Typically diffuse_cubemap and shadowmap
	phong_program->use();

	/*
		WE WILL HAVE TO WRITE CODE HERE!!!!
	*/

	phong_program->disuse();
	CHECK_GL_ERRORS();
	
	// Initialize shadow_fbo
	shadow_fbo.reset(new ShadowFBO(shadow_map_width, shadow_map_height));

	//Set up VAOs and set as input to shaders
	glGenVertexArrays(3, &vao[0]);
	glBindVertexArray(vao[0]);
	model->getVertices()->bind();
	phong_program->setAttributePointer("position", 3);
	std::cout << "INIT Before setAttribPointer position for shadow program" << std::endl;
	shadow_program->setAttributePointer("position", 3);
	std::cout << "INIT After setAttribPointer position for shadow program" << std::endl;

	model->getNormals()->bind();
	phong_program->setAttributePointer("normal", 3);
	//std::cout << "INIT Before setAttribPointer normals for shadow program" << std::endl;
	//shadow_program->setAttributePointer("normal", 3);
	//std::cout << "INIT After setAttribPointer normals for shadow program" << std::endl;
	model->getVertices()->unbind(); //Unbinds both vertices and normals
	glBindVertexArray(0);
	
	glBindVertexArray(vao[1]);
	cube_vertices->bind();
	phong_program->setAttributePointer("position", 3);
	//shadow_program->setAttributePointer("position", 3);
	cube_normals->bind();
	phong_program->setAttributePointer("normal", 3);
	//shadow_program->setAttributePointer("normal", 3);
	model->getVertices()->unbind(); //Unbinds both vertices and normals
	glBindVertexArray(0);

	/*
	glBindVertexArray(vao[2]);
	shadow_map_model.vbo->bind();
	phong_program->setAttributePointer("position", 3);
	// We probably don't need the normal, since we can probably just use the camera.lookAt vector.
	shadow_map_model.vbo->unbind();
	glBindVertexArray(0);
	*/
	CHECK_GL_ERRORS();
	std::cout << "END OF INIT " << std::endl;

	initFBOProgramAndVAO();
}

void GameManager::initFBOProgramAndVAO()
{
	std::cout << "init FBO PRogram and VAO" << std::endl;
	try {
		fbo_program.reset(new Program("shaders/fbo.vert", "shaders/fbo.frag"));
		fbo_program->use();
		std::cout << "About to get uniform" << std::endl;
		glUniform1i(fbo_program->getUniform("fbo_texture"), 0); // Yes, 0 is correct
 		std::cout << "Done setting uniform" << std::endl;
		glGenVertexArrays(1, &fbo_vao);

		glBindVertexArray(fbo_vao);

		static float positions[8] = {
			-1.0, 1.0,
			-1.0, -1.0,
			1.0, 1.0,
			1.0, -1.0
		};

		glGenBuffers(1, &fbo_vertex_bo);
		glBindBuffer(GL_ARRAY_BUFFER, fbo_vertex_bo);
		glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), &positions[0], GL_STATIC_DRAW);

		std::cout << "about to set in_Positions" << std::endl;
		fbo_program->setAttributePointer("in_Position", 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		std::cout << "done setting in_Positions" << std::endl;
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	} catch(std::runtime_error &error)
	{
		std::cerr << "Exception in initing FBOProgram and VAO: \n" << error.what() << std::endl;
	}
	CHECK_GL_ERRORS();
}


void GameManager::renderColorPass() {
	glViewport(0, 0, window_width, window_height); // WHAT THE HELL IS A VIEWPORT?
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0); // Make sure that we are rendering to screen

	//Create the new view matrix that takes the trackball view into account
	glm::mat4 view_matrix_new = camera.view*cam_trackball.getTransform();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//phong_program->use();
	phong_program->use();
	//Bind shadow map and diffuse cube map

	/**
	  * Render cube
	  */ 
	{
		glBindVertexArray(vao[1]);

		glm::mat4 model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(cube_scale)); // Ok, så vi lager en identitetsmatrise som er skalert med 22.5
		glm::mat4 model_matrix_inverse = glm::inverse(model_matrix); // WTF???
		glm::mat4 modelview_matrix = view_matrix_new*model_matrix;
		glm::mat4 modelview_matrix_inverse = glm::inverse(modelview_matrix);
		glm::mat4 modelviewprojection_matrix = camera.projection*modelview_matrix;
		glm::vec3 light_pos = glm::mat3(model_matrix_inverse)*light.position/model_matrix_inverse[3].w; // HVA GJØR DENNE FORMELEN??

		glUniform3fv(phong_program->getUniform("light_pos"), 1, glm::value_ptr(light_pos));
		glUniform3fv(phong_program->getUniform("color"), 1, glm::value_ptr(glm::vec3(1.0f, 0.8f, 0.8f)));
		glUniformMatrix4fv(phong_program->getUniform("modelviewprojection_matrix"), 1, 0, glm::value_ptr(modelviewprojection_matrix));
		glUniformMatrix4fv(phong_program->getUniform("modelview_matrix_inverse"), 1, 0, glm::value_ptr(modelview_matrix_inverse));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	/**
	  * Render model
	  * Create modelview matrix and normal matrix and set as input
	  */

	if (m_rendering_mode)
	{
		if (m_rendering_mode == RENDERING_MODE_WIREFRAME)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			// Change to hidden line here!
			phong_program->disuse();
			hiddenline_program->use();

		}
	}
	
	glBindVertexArray(vao[0]);

	for (int i=0; i<n_models; ++i) {
		glm::mat4 model_matrix = model_matrices.at(i);
		glm::mat4 model_matrix_inverse = glm::inverse(model_matrix);
		glm::mat4 modelview_matrix = view_matrix_new*model_matrix;
		glm::mat4 modelview_matrix_inverse = glm::inverse(modelview_matrix);
		glm::mat4 modelviewprojection_matrix = camera.projection*modelview_matrix;
		glm::vec3 light_pos = glm::mat3(model_matrix_inverse)*light.position/model_matrix_inverse[3].w;
	

		glUniform3fv((m_rendering_mode != RENDERING_MODE_HIDDENLINE) ? phong_program->getUniform("light_pos") : hiddenline_program->getUniform("light_pos"), 1, glm::value_ptr(light_pos));
		glUniform3fv((m_rendering_mode != RENDERING_MODE_HIDDENLINE) ? phong_program->getUniform("color") : hiddenline_program->getUniform("color"), 1, glm::value_ptr(model_colors.at(i)));
		glUniformMatrix4fv((m_rendering_mode != RENDERING_MODE_HIDDENLINE) ? phong_program->getUniform("modelviewprojection_matrix") : hiddenline_program->getUniform("modelviewprojection_matrix"), 1, 0, glm::value_ptr(modelviewprojection_matrix));
		glUniformMatrix4fv((m_rendering_mode != RENDERING_MODE_HIDDENLINE) ? phong_program->getUniform("modelview_matrix_inverse") : hiddenline_program->getUniform("modelview_matrix_inverse"), 1, 0, glm::value_ptr(modelview_matrix_inverse));
		
		/*if (m_rendering_mode == RENDERING_MODE_HIDDENLINE) {
			std::cout << GLUtils::mat4ToString(cam_trackball.getTransform()).c_str() << std::endl;
			glProgramUniformMatrix4fv(hiddenline_program->getUniform("rotation_matrix"), hiddenline_program->getUniform("rotation_matrix"), 1, 0, glm::value_ptr(cam_trackball.getTransform()));
		}*/
		
		glDrawArrays(GL_TRIANGLES, 0, model->getNVertices());
	}

	glBindVertexArray(0);
	
	if(m_rendering_mode)
	{
		if (m_rendering_mode == RENDERING_MODE_WIREFRAME)
		{
			glPolygonMode(GL_FRONT, GL_FILL);
		}
		else
		{
			hiddenline_program->disuse();
			phong_program->use();
		}
	}

	phong_program->disuse();
}

void GameManager::renderShadowPass() {
	//Render the scene from the light, with the lights projection, etc. into the shadow_fbo. Store only the depth values
	//Remember to set the viewport, clearing the depth buffer, etc.

	glViewport(0, 0, shadow_map_width, shadow_map_height); // What the hell is a viewport???

	shadow_fbo->bind();

	shadow_program->use();

	// Implement rendering to textures here, maybe even create different shader programs and switch to them, in order to render the shadow maps
	// Everything that is rendered when shadow_fbo is binded, should be rendered to the fbo, rather than the screen.

	glClear(GL_DEPTH_BUFFER_BIT);

	model->getVertices()->bind();

	shadow_program->setAttributePointer("position", 3);

	glm::mat4 transform;

	for (int i = 0; i < n_models; ++i)
	{
		transform = light.projection * light.view * model_matrices.at(i); // Nå transformerer vi alle kaninene inn i lyset's clip space
		glUniformMatrix4fv(shadow_program->getUniform("light_transform"), 1, 0, glm::value_ptr(transform));
		glDrawArrays(GL_TRIANGLES, 0, model->getNVertices());
	}

	shadow_program->disuse();
	shadow_fbo->unbind();
	

}

void GameManager::renderFBO()
{
	glBindVertexArray(fbo_vao);
	fbo_program->use();

	glBindTexture(GL_TEXTURE_2D, shadow_fbo->getTexture());
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	fbo_program->disuse();
	glBindVertexArray(0);
}


void GameManager::render() {
	//Rotate the light a bit
	float elapsed = static_cast<float>(my_timer.elapsedAndRestart());
	glm::mat4 rotation = glm::rotate(elapsed*10.f, 0.0f, 1.0f, 0.0f);
	light.position = glm::mat3(rotation)*light.position;
	light.view = glm::lookAt(light.position,  glm::vec3(0), glm::vec3(0.0, 1.0, 0.0));

	renderShadowPass();
	//renderColorPass();  // REMEMBER TO UNCOMMENT
	
	renderFBO();
	try
	{
		CHECK_GL_ERRORS();
	} catch(std::runtime_error &e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
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
				cam_trackball.rotateBegin(event.motion.x, event.motion.y);
				break;
			case SDL_MOUSEBUTTONUP:
				cam_trackball.rotateEnd(event.motion.x, event.motion.y);
				break;
			case SDL_MOUSEMOTION:
				cam_trackball.rotate(event.motion.x, event.motion.y, zoom);
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
					phong_rendering();
					break;
				case SDLK_2:
					wireframe_rendering();
					break;
				case SDLK_3:
					hiddenline_rendering();
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
		SDL_GL_SwapWindow(main_window);
	}
	quit();
}

void GameManager::zoomIn() {
	zoom *= 1.1f;
	camera.projection = glm::perspective(fovy/zoom,
			window_width / (float) window_height, near_plane, far_plane);
}

void GameManager::zoomOut() {
	zoom = std::max(zoom*0.9f, 0.5f);
	camera.projection = glm::perspective(fovy/zoom,
			window_width / (float) window_height, near_plane, far_plane);
}

void GameManager::phong_rendering()
{
	m_rendering_mode = RENDERING_MODE_PHONG;

}

void GameManager::wireframe_rendering()
{
	// I might want to disable phong shading here!
	m_rendering_mode = RENDERING_MODE_WIREFRAME;
}

void GameManager::hiddenline_rendering()
{
	m_rendering_mode = RENDERING_MODE_HIDDENLINE;
}


void GameManager::quit() {
	std::cout << "Bye bye..." << std::endl;
}
