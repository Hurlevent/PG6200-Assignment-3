#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif

#include <memory>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "Timer.h"
#include "GLUtils/Program.h"
#include "GLUtils/CubeMap.h"
#include "Model.h"
#include "VirtualTrackball.h"
#include "ShadowFBO.h"


/**
 * This class handles the game logic and display.
 * Uses SDL as the display manager, and glm for
 * vector and matrix computations
 */
class GameManager {
public:
	explicit GameManager();
	~GameManager();

	/**
	 * Initializes the game, including the OpenGL context
	 * and data required
	 */
	void init();

	/**
	 * The main loop of the game. Runs the SDL main loop
	 */
	void play();

	/**
	 * Quit function
	 */
	void quit();

	/**
	 * Function that handles rendering into the OpenGL context
	 */
	void render();

	/**
	  * Function that renders a shadow pass
	  */
	void renderShadowPass();

	/**
	  * Function that renders to screen
	  */
	void renderColorPass() const;

	/**
	  *	Function that renders the shadowmap fbo onto a section of the screen
	  */

	void renderFBO() const;

protected:
	/**
	 * Creates the OpenGL context using SDL
	 */
	void createOpenGLContext();

	static const unsigned int s_window_width = 800;
	static const unsigned int s_window_height = 600;

	static const unsigned int s_shadow_map_width = 1024;
	static const unsigned int s_shadow_map_height = 1024;

	static const unsigned int s_number_of_models = 20;

	static const float s_near_plane;
	static const float s_far_plane;
	static const float s_fovy;
	static const float s_cube_scale;

	static const float s_cube_vertices_data[];
	static const float s_cube_normals_data[];

private:
	void zoomIn();
	void zoomOut();

	/**
	*	A member function pointer that will point to either phong_rendering, wireframe_rendering and hiddenline_rendering depending on which rendering mode the user is using.
	*	The reason for doing this is so we don't have to make an ugly switch/case statement in our renderColorPass() function implementation.
	*/
	void (GameManager::*render_model)(const GLfloat * modelviewprojection, const GLfloat * modelview_inverse, const GLfloat * light_transform, const GLfloat * light_pos, int iteration) const;

	void phong_rendering(const GLfloat * modelviewprojection, const GLfloat * modelview_inverse, const GLfloat * light_transform, const GLfloat * light_pos, int iteration) const;
	void wireframe_rendering(const GLfloat * modelviewprojection, const GLfloat * modelview_inverse, const GLfloat * light_transform, const GLfloat * light_pos, int iteration) const;
	void hiddenline_rendering(const GLfloat * modelviewprojection, const GLfloat * modelview_inverse, const GLfloat * light_transform, const GLfloat * light_pos, int iteration) const;

	void init_fbo();


	GLuint m_fbo_vao;

	GLuint m_fbo_vertex_bo;

	bool m_display_shadow_map;

	GLuint m_vao[2]; //< Vertex array objects
	std::shared_ptr<GLUtils::Program> m_phong_program, m_wireframe_program, m_exploded_view_program, m_shadow_program, m_hiddenline_program, m_fbo_program; // exploded_view_program is not in use
	std::shared_ptr<GLUtils::CubeMap> m_diffuse_cubemap;
	std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER>> m_cube_vertices, m_cube_normals;

	std::shared_ptr<Model> m_model;
	std::shared_ptr<ShadowFBO> m_shadow_fbo;

	Timer m_my_timer; //< Timer for machine independent motion
	float m_zoom; //< Zoom factor

	struct {
		glm::vec3 position; //< Light position for shading etc
		glm::mat4 projection;
		glm::mat4 view;
	} m_light;

	struct {
		glm::mat4 projection;
		glm::mat4 view;
	} m_camera;

	std::vector<glm::mat4> m_model_matrices; //< OpenGL model transformation matrix
	std::vector<glm::vec3> m_model_colors;

	SDL_Window* m_main_window; //< Our window handle
	SDL_GLContext m_main_context; //< Our opengl context handle 

	VirtualTrackball m_cam_trackball;

};

#endif // _GAMEMANAGER_H_
