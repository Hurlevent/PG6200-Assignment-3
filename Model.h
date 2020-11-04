#ifndef _MODEL_H__
#define _MODEL_H__

#include <memory>
#include <string>
#include <vector>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLUtils/BO.h"

class Model {
public:
	explicit Model(const std::string & filename, bool invert=0);
	~Model();

  unsigned int getNumberOfVertices() const {return m_number_of_vertices;}
  glm::mat4 getTransform() const {return m_transform;}

	std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER> > getVertices() const { return m_vertices; }
	std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER> > getNormals() const { return m_normals; }
	std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER> > getColors() const { return m_colors; }

private:
	static void loadRecursive(bool invert,
			std::vector<float>& vertex_data, std::vector<float>& normal_data,
			std::vector<float>& color_data,
			const aiScene* scene, const aiNode* node, aiMatrix4x4 modelview_matrix);

	const aiScene* scene;

	std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER>> m_normals;
	std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER>> m_vertices;
	std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER>> m_colors;

	glm::vec3 m_min_dim;
	glm::vec3 m_max_dim;
	glm::mat4 m_transform;

	unsigned int m_number_of_vertices;
};

#endif
