// Author  : Lewis Ward
// Program : 3D Renderer
// Date    : 09/10/2016
#include "Window.h"
#include "ObjObject.h"
#include "Defines.h"
#include <memory>
#define shared std::shared_ptr

class Application
{
public:
	Application();
	~Application();

	void update(float& dt);

	void draw();

private:
	gl::Texture* m_shadowMap;
	uint32_t m_shadowFrameBuffer;
	shared<gl::Texture> m_texture;
	shared<gl::ObjObject> m_planeObject;
	std::vector<shared<gl::ObjObject>> m_sphereObject;
	shared<gl::Program> m_shaderProgram;
	shared<gl::Program> m_shadowProgram;
	glm::vec3 m_lightPos;
	glm::vec3 m_camera;
	glm::vec2 m_scrennSize;

	void objDrawCall(gl::ObjObject* objPtr)
	{
		const float* offset = 0;
		glBindBuffer(GL_ARRAY_BUFFER, objPtr->getVertexBuffer()->getVBO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objPtr->getVertexBuffer()->getIBO());
		glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertexNormalUV), offset);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, true, sizeof(vertexNormalUV), offset + 3);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, true, sizeof(vertexNormalUV), offset + 6);
		glEnableVertexAttribArray(2);
		glDrawElements(GL_TRIANGLES, objPtr->getIndicesCount(), GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};
