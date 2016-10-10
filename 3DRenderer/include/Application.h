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
	shared<gl::Texture> m_texture;
	shared<gl::ObjObject> m_planeObject;
	shared<gl::ObjObject> m_sphereObject;
	shared<gl::Program> m_shaderProgram;
	glm::vec2 m_scrennSize;
};
