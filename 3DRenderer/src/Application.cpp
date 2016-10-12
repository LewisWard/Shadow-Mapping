// Author  : Lewis Ward
// Program : 3D Renderer
// Date    : 09/10/2016
#include "Application.h"
#include "glm\gtc\matrix_transform.hpp"
#include <string>

Application::Application()
{
	m_scrennSize.x = RES_LOW_X;
	m_scrennSize.y = RES_LOW_Y;
	onResize(m_scrennSize.x, m_scrennSize.y);
	glViewport(0, 0, m_scrennSize.x, m_scrennSize.y);
	windowRename("3D Renderer - Lewis Ward");

	m_lightPos = glm::vec3(2.5f, 5.0f, -5.0f);

	m_texture = std::make_shared<gl::Texture>("textures/wood.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_TEXTURE_REPEATS);
	m_sphereObject = std::make_shared<gl::ObjObject>("models/sphere.obj", "models/");
	m_planeObject = std::make_shared<gl::ObjObject>("models/plane.obj", "models/");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glCullFace(GL_BACK);

	gl::Shader vertexShader("shaders/shader.vrt", kVertexShader);
	gl::Shader pixelShader("shaders/shader.pix", kPixelShader);
	m_shaderProgram = std::make_shared<gl::Program>(vertexShader, pixelShader);
	gl::Shader vertexShadow("shaders/shadow.vrt", kVertexShader);
	gl::Shader pixelShadow("shaders/shadow.pix", kPixelShader);
	m_shadowProgram = std::make_shared<gl::Program>(vertexShadow, pixelShadow);


	// create framebuffer for the shadow pass
	m_shadowFrameBuffer = 0;
	glGenFramebuffers(1, &m_shadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFrameBuffer);
	
	// create shadow map texture
	gl::textureDesc shadowMapDesc;
	shadowMapDesc.internalFormat = GL_DEPTH_COMPONENT16;
	shadowMapDesc.format = GL_DEPTH_COMPONENT;
	shadowMapDesc.width = RES_LOW_X;
	shadowMapDesc.height = RES_LOW_Y;
	shadowMapDesc.type = GL_FLOAT;
	shadowMapDesc.texParamA = GL_NEAREST;
	shadowMapDesc.texParamB = GL_NEAREST;
	m_shadowMap = new gl::Texture(1, GL_TEXTURE_2D, shadowMapDesc, NULL);
	m_shadowMap->bind(1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// attach shadow map to framebuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowMap->getID(), 0);
	glDrawBuffer(GL_NONE);

	// check framebuffer is happy
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error\n");

}

Application::~Application()
{
	glDeleteFramebuffers(1, &m_shadowFrameBuffer);
	delete m_shadowMap;
	m_shadowMap = 0;
	std::cout << "Application deleted\n";
}

void Application::update(float& dt)
{

}

void Application::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(28.0f / 255.0f, 28.0f / 255.0f, 28.0f / 255.0f, 1.0f);

	// MVP from camera position
	glm::mat4 model(1.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 5.0f, 25.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(45.0f, m_scrennSize.x / m_scrennSize.y, 0.1f, 100.0f);
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));

	// MVP from light poisition
	glm::mat4 depthProjection = glm::ortho<float>(-25.0f, 25.0f, -25.0f, 25.0f, 0.1f, 100);
	glm::mat4 depthView = glm::lookAt(m_lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 depthModel = glm::mat4(1.0);
	glm::mat4 depthMVP = depthProjection * depthView * depthModel;

	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
		);
	glm::mat4 depthBiasMVP = biasMatrix * depthMVP;


	// shadow pass
	{
		glCullFace(GL_FRONT);
		m_shadowProgram->bind();
		glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFrameBuffer);
		m_shadowMap->bind(1);


		m_shadowProgram->uniformMatrix4("depthMVP", 1, depthMVP);
		objDrawCall(m_planeObject.get());
		objDrawCall(m_sphereObject.get());

		m_shadowMap->unbind();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_shadowProgram->unbind();
		glCullFace(GL_BACK);
	}
	
	// normal pass
	{
		// draw start
		m_shaderProgram->bind();
		m_texture->bind(1);
		m_shadowMap->bind(2);

		// plane
		m_shaderProgram->uniform1i("texture", 1);
		m_shaderProgram->uniform1i("shadowMap", 2);
		m_shaderProgram->uniformMatrix4("depthBiasMVP", 1, depthBiasMVP);
		m_shaderProgram->uniformMatrix4("modelMatrix", 1, model);
		m_shaderProgram->uniformMatrix4("viewMatrix", 1, view);
		m_shaderProgram->uniformMatrix4("projectionlMatrix", 1, projection);
		m_shaderProgram->uniformMatrix3("normalMatrix", 1, normalMatrix);
		m_shaderProgram->uniform3f("light.position", 20.0f, 10.0f, 0.0f);
		m_shaderProgram->uniform3f("light.colour", 0.5f, 0.5f, 0.6f);
		objDrawCall(m_planeObject.get());
		objDrawCall(m_sphereObject.get());

		m_shadowMap->unbind();
		m_texture->unbind();
		m_shaderProgram->unbind();
	}

	// draw end
	glFlush();


	glutSwapBuffers();
}

