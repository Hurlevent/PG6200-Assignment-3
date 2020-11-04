#ifndef _PROGRAM_HPP__
#define _PROGRAM_HPP__

#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <fstream>

#include <GL/glew.h>

namespace GLUtils {

class Program {
public:
	explicit Program(const std::string & vs, const std::string & fs)
    : m_gl_program_id(glCreateProgram())
  {

		attachShader(readFile(vs), GL_VERTEX_SHADER);
		attachShader(readFile(fs), GL_FRAGMENT_SHADER);

    link();
	}

	explicit Program(const std::string & vs, const std::string & gs, const std::string & fs)
    : m_gl_program_id(glCreateProgram())
  {
		attachShader(readFile(vs), GL_VERTEX_SHADER);
		attachShader(readFile(gs), GL_GEOMETRY_SHADER);
		attachShader(readFile(fs), GL_FRAGMENT_SHADER);

		link();
	}

	void use() const {
		glUseProgram(m_gl_program_id);
	}

	static void disuse() {
		glUseProgram(0);
	}

  GLint getUniform(const std::string & var) const {
		GLint loc = glGetUniformLocation(m_gl_program_id, var.c_str());
		assert(loc >= 0);
		return loc;
	}

  void setAttributePointer(const std::string & var, unsigned int size, GLenum type=GL_FLOAT, GLboolean normalized=GL_FALSE, GLsizei stride=0, GLvoid* pointer=NULL) const {
		GLint loc = glGetAttribLocation(m_gl_program_id, var.c_str());
		assert(loc >= 0);
		glVertexAttribPointer(loc, size, type, normalized, stride, pointer);
		glEnableVertexAttribArray(loc);
	}

  GLuint get_program_id() const { return m_gl_program_id; }

private:
	void link() const {
		std::stringstream log;
		glLinkProgram(m_gl_program_id);

		// check for errors
		GLint linkstatus;
		glGetProgramiv(m_gl_program_id, GL_LINK_STATUS, &linkstatus);
		if (linkstatus != GL_TRUE) {
			log << "Linking failed!" << std::endl;

			GLint logsize;
			glGetProgramiv(m_gl_program_id, GL_INFO_LOG_LENGTH, &logsize);

			if (logsize > 0) {
				std::vector < GLchar > infolog(logsize + 1);
				glGetProgramInfoLog(m_gl_program_id, logsize, NULL, &infolog[0]);
				log << "--- error log ---" << std::endl;
				log << std::string(infolog.begin(), infolog.end()) << std::endl;
			} else {
				log << "--- empty log message ---" << std::endl;
			}
			throw std::runtime_error(log.str());
		}
	}

	void attachShader(const std::string & src, unsigned int type) const {
		std::stringstream log;
		// create shader object
		GLuint s = glCreateShader(type);
		if (s == 0) {
			log << "Failed to create shader of type " << type << std::endl;
			throw std::runtime_error(log.str());
		}

		// set source code and compile
		const GLchar* src_list[1] = { src.c_str() };
		glShaderSource(s, 1, src_list, NULL);
		glCompileShader(s);

		// check for errors
		GLint compile_status;
		glGetShaderiv(s, GL_COMPILE_STATUS, &compile_status);
		if (compile_status != GL_TRUE) {
			// compilation failed
			log << "Compilation failed!" << std::endl;
			log << "--- source code ---" << std::endl;
			std::istringstream src_ss(src);
			std::string line;
			unsigned int i=0;
			while (std::getline(src_ss, line))
				log << std::setw(4) << std::setfill('0') << ++i << line << std::endl;

			GLint logsize;
			glGetShaderiv(s, GL_INFO_LOG_LENGTH, &logsize);
			if (logsize > 0) {
				std::vector<GLchar> infolog(logsize + 1);
				glGetShaderInfoLog(s, logsize, NULL, &infolog[0]);

				log << "--- error log ---" << std::endl;
				log << std::string(infolog.begin(), infolog.end()) << std::endl;
			}
			else {
				log << "--- empty log message ---" << std::endl;
			}
			throw std::runtime_error(log.str());
		}

		glAttachShader(m_gl_program_id, s);
	}

  static std::string readFile(const std::string & filename) {
    int length;
    std::string buffer;
    std::string contents;

    std::ifstream is;

    is.open(filename.c_str());

    if (!is.good()) {
      std::string err = "Could not open ";
      err.append(filename);
      throw std::runtime_error(err);
    }

    // get length of file:
    is.seekg(0, std::ios::end);
    length = static_cast<int>(is.tellg());
    is.seekg(0, std::ios::beg);

    // reserve memory:
    contents.reserve(length);

    // read data
    while(getline(is,buffer)) {
      contents.append(buffer);
      contents.append("\n");
    }
    is.close();

    return contents;
  }

  GLuint m_gl_program_id; //< OpenGL shader program

};

} //Namespace GLUtils

#endif
