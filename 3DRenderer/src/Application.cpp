// Author  : Lewis Ward
// Program : 3D Renderer
// Date    : 09/10/2016
#include "Application.h"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\rotate_vector.hpp"
#include <string>
#define SHADOWMAP_SIZE TEXTURE_SUPER


Application::Application()
{
	m_scrennSize.x = RES_MEDIUM_X;
	m_scrennSize.y = RES_MEDIUM_Y;
	onResize((int)m_scrennSize.x, (int)m_scrennSize.y);
	glViewport(0, 0, (size_t)m_scrennSize.x, (size_t)m_scrennSize.y);
	windowRename("3D Renderer - Lewis Ward");

	m_lightPos = glm::vec3(-3.0f, 20.0f, 20.0f);

#if RELEASEINBUILD
	m_texture = std::make_shared<gl::Texture>("textures/wood.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_TEXTURE_REPEATS);
	m_textureSecondary = std::make_shared<gl::Texture>("textures/cobblestone.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_TEXTURE_REPEATS);
	m_sphereObject.push_back(std::make_shared<gl::ObjObject>("models/sphere.obj", "models/"));
	m_sphereObject.push_back(std::make_shared<gl::ObjObject>("models/FinalBaseMesh.obj", "models/"));
	m_planeObject = std::make_shared<gl::ObjObject>("models/plane.obj", "models/");

	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	m_sphereObject[1]->setMatrix(model);

	m_sphereObject[0]->setLinkedTexture(m_texture.get());
	m_sphereObject[1]->setLinkedTexture(m_texture.get());
	m_planeObject->setLinkedTexture(m_textureSecondary.get());
#else
	m_texture = std::make_shared<gl::Texture>("textures/wood.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_TEXTURE_REPEATS);
	m_textureSecondary = std::make_shared<gl::Texture>("textures/cobblestone.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_TEXTURE_REPEATS);
	m_sphereObject.push_back(std::make_shared<gl::ObjObject>("models/sphere.obj", "models/"));
	m_sphereObject.push_back(std::make_shared<gl::ObjObject>("models/cube.obj", "models/"));
	m_sphereObject.push_back(std::make_shared<gl::ObjObject>("models/sphere.obj", "models/"));
	m_sphereObject.push_back(std::make_shared<gl::ObjObject>("models/cube.obj", "models/"));
	m_planeObject = std::make_shared<gl::ObjObject>("models/plane.obj", "models/");

	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(4.5f, 2.75f, 1.5f));
	model = glm::rotate(model, 45.0f, glm::vec3(0, 1, 0));
	m_sphereObject[1]->setMatrix(model);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.65f, 3.0f, 4.5f));
	m_sphereObject[2]->setMatrix(model);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-5.0f, 1.0f, -4.0f));
	m_sphereObject[3]->setMatrix(model);

	m_sphereObject[0]->setLinkedTexture(m_texture.get());
	m_sphereObject[1]->setLinkedTexture(m_texture.get());
	m_sphereObject[2]->setLinkedTexture(m_texture.get());
	m_sphereObject[3]->setLinkedTexture(m_texture.get());
	m_planeObject->setLinkedTexture(m_textureSecondary.get());
#endif
	
	glEnable(GL_MULTISAMPLE);
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
	shadowMapDesc.internalFormat = GL_DEPTH_COMPONENT;
	shadowMapDesc.format = GL_DEPTH_COMPONENT;
	shadowMapDesc.width = SHADOWMAP_SIZE;
	shadowMapDesc.height = SHADOWMAP_SIZE;
	shadowMapDesc.type = GL_FLOAT;
	shadowMapDesc.texParamA = GL_LINEAR;
	shadowMapDesc.texParamB = GL_LINEAR;
	m_shadowMap = new gl::Texture(1, GL_TEXTURE_2D, shadowMapDesc, NULL);
	m_shadowMap->bind(1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// attach shadow map to framebuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowMap->getID(), 0);
	glDrawBuffer(GL_NONE);

	// check framebuffer is happy
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error\n");

	m_camera = glm::vec3(0.0f, 9.0f, 18.0f);
	m_camera = glm::rotateY(m_camera, 0.25f);

	m_polyOffset = 4.0f;
}

Application::~Application()
{
	glDeleteFramebuffers(1, &m_shadowFrameBuffer);
	if(m_shadowMap)
		delete m_shadowMap;
	m_shadowMap = 0;
	std::cout << "Application deleted\n";
}

void Application::update(float& dt)
{
	int cube = 1;

	if (isKeyPressed('w'))
	{
		float movement = 1.0f * dt;
		glm::mat4 m = glm::translate(m_sphereObject[cube]->getMatrix(), glm::vec3(0.0f, movement, 0.0f));
		m_sphereObject[cube]->setMatrix(m);
	}
	if (isKeyPressed('s'))
	{
		float movement = 1.0f * dt;
		glm::mat4 m = glm::translate(m_sphereObject[cube]->getMatrix(), glm::vec3(0.0f, -movement, 0.0f));
		m_sphereObject[cube]->setMatrix(m);
	}

	if (isKeyPressed('a'))
	{
		float movement = 1.0f * dt;
		glm::mat4 m = glm::translate(m_sphereObject[cube]->getMatrix(), glm::vec3(movement, 0.0f, 0.0f));
		m_sphereObject[cube]->setMatrix(m);
	}
	if (isKeyPressed('d'))
	{
		float movement = 1.0f * dt;
		glm::mat4 m = glm::translate(m_sphereObject[cube]->getMatrix(), glm::vec3(-movement, 0.0f, 0.0f));
		m_sphereObject[cube]->setMatrix(m);
	}

	if (isKeyPressed('q'))
	{
		float movement = 1.0f * dt;
		glm::mat4 m = glm::translate(m_sphereObject[cube]->getMatrix(), glm::vec3(0.0f, 0.0f, movement));
		m_sphereObject[cube]->setMatrix(m);
	}
	if (isKeyPressed('e'))
	{
		float movement = 1.0f * dt;
		glm::mat4 m = glm::translate(m_sphereObject[cube]->getMatrix(), glm::vec3(0.0f, 0.0f, -movement));
		m_sphereObject[cube]->setMatrix(m);
	}

	if (isKeyPressed('i'))
	{
		m_polyOffset += 1.0f * dt;
		std::cout << m_polyOffset << std::endl;
	}

	if (isKeyPressed('k'))
	{
		m_polyOffset -= 1.0f * dt;
		if (m_polyOffset <= 0.0f)
			m_polyOffset = 0.0f;
		std::cout << m_polyOffset << std::endl;
	}

	static bool rotateToggle = false;
	static bool ployToggle = false;

	if (isKeyPressed('x'))
		rotateToggle = true;
	else
		rotateToggle = false;

	float rotate = 1.0f * dt;

	if(rotateToggle)
		m_camera = glm::rotateY(m_camera, rotate);
}

void Application::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(28.0f / 255.0f, 28.0f / 255.0f, 28.0f / 255.0f, 1.0f);

	// MVP from camera position
	glm::mat4 model(1.0f);
	glm::mat4 view = glm::lookAt(m_camera, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(45.0f, m_scrennSize.x / m_scrennSize.y, 0.1f, 100.0f);
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));

	// MVP from light poisition
	glm::mat4 depthProjection = glm::perspective(45.0f, m_scrennSize.x / m_scrennSize.y, 0.1f, 100.0f);
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
		glViewport(0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(m_polyOffset, m_polyOffset);
		m_shadowProgram->bind();
		glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFrameBuffer);
		glClear(GL_DEPTH_BUFFER_BIT);
		m_shadowMap->bind(1);


		m_shadowProgram->uniformMatrix4("depthMVP", 1, depthMVP);
		objDrawCall(m_planeObject.get());
		for (size_t i = 0; i < m_sphereObject.size(); i++)
		{
			glm::mat4 depthM = m_sphereObject[i]->getMatrix();
			depthMVP = depthProjection * depthView * depthM;
			m_shadowProgram->uniformMatrix4("depthMVP", 1, depthMVP);
			objDrawCall(m_sphereObject[i].get());
		}

		m_shadowMap->unbind();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_shadowProgram->unbind();
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	
	// normal pass
	{
		glViewport(0, 0, (size_t)m_scrennSize.x, (size_t)m_scrennSize.y);
		// draw start
		m_shaderProgram->bind();
		m_planeObject->getLinkedTexture()->bind(1);
		m_shaderProgram->uniform1i("texture", 1);
		m_shadowMap->bind(2);

		m_shaderProgram->uniform1i("shadowMap", 2);
		m_shaderProgram->uniformMatrix4("depthBiasMVP", 1, depthBiasMVP);
		m_shaderProgram->uniformMatrix4("modelMatrix", 1, model);
		m_shaderProgram->uniformMatrix4("viewMatrix", 1, view);
		m_shaderProgram->uniformMatrix4("projectionlMatrix", 1, projection);
		m_shaderProgram->uniformMatrix3("normalMatrix", 1, normalMatrix);
		m_shaderProgram->uniform3f("gammaCorrectness", 1.0f / 2.2f, 1.0f / 2.2f,  1.0f / 2.2f);
		m_shaderProgram->uniform3f("eyePosition", m_camera.x, m_camera.y, m_camera.z);
		m_shaderProgram->uniform3f("ambientColour", 5.0f / 255.0f, 5.0f / 255.0f, 5.0f / 255.0f);
		m_shaderProgram->uniform3f("light.colour", 75.0f / 255.0f, 75.0f / 255.0f, 75.0f / 255.0f);
		m_shaderProgram->uniform3f("light.position", m_lightPos.x, m_lightPos.y, m_lightPos.z);
		m_shaderProgram->uniform1f("light.shininess", 90.0f);
		
		objDrawCall(m_planeObject.get());
		for (size_t i = 0; i < m_sphereObject.size(); i++)
		{
			m_sphereObject[i]->getLinkedTexture()->bind(1);
			m_shaderProgram->uniform1i("texture", 1);

			glm::mat4 depthM = m_sphereObject[i]->getMatrix();
			depthMVP = depthProjection * depthView * depthM;
			depthBiasMVP = biasMatrix * depthMVP;
			m_shaderProgram->uniformMatrix4("depthBiasMVP", 1, depthBiasMVP);
			m_shaderProgram->uniformMatrix4("modelMatrix", 1, m_sphereObject[i]->getMatrix());
			m_shaderProgram->uniformMatrix3("normalMatrix", 1, glm::transpose(glm::inverse(m_sphereObject[i]->getMatrix())));
			objDrawCall(m_sphereObject[i].get());
		}

		m_shadowMap->unbind();
		m_texture->unbind();
		m_shaderProgram->unbind();
	}


	// draw end
	glFlush();


	glutSwapBuffers();
}

