/**
 * Base code
 * Draws two meshes and one ground plane, one mesh has textures, as well
 * as ground plane.
 * Must be fixed to load in mesh with multiple shapes (dummy.obj)
 */

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "WindowManager.h"
#include "GLTextureWriter.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "stb_image.h"
using namespace std;
using namespace glm;


class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
	std::shared_ptr<Program> texProg;
	std::shared_ptr<Program> cubeProg;

	// Shapes to be used (from obj file)
	std::vector<shared_ptr<Shape>> AllShapes;
	//meshes with just one shape
	shared_ptr<Shape> cactus;
	shared_ptr<Shape> cube;
	vector<shared_ptr<Shape>> deer;
	shared_ptr<Shape> shape;
	shared_ptr<Shape> cloud;
	shared_ptr<Shape> tree;
	shared_ptr<Shape> tree2;
	shared_ptr<Shape> butterfly;
	shared_ptr<Shape> grass;
	shared_ptr<Shape> poop;
	vector<shared_ptr<Shape>> bush;
	//ground plane info
	GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
	int gGiboLen;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	//geometry for texture render
	GLuint quad_VertexArrayID;
	GLuint quad_vertexbuffer;

	//three different textures
	shared_ptr<Texture> texture0;
 	shared_ptr<Texture> texture1;
 	shared_ptr<Texture> texture2;
	shared_ptr<Texture> texture3;


	unsigned int cubeMapTexture;

	int gMat = 0;

	//For each shape, now that they are not resized, they need to be
	//transformed appropriately to the origin and scaled
	//transforms for Nef
	vec3 gTrans = vec3(0);
	float gScale = 1.0;

	//transforms for the world
	vec3 gDTrans = vec3(0);
	float gDScale = 1.0;

	vec3 gCTrans = vec3(0);
	float gCScale = 1.0;

	vec3 treeTrans = vec3(0);
	float treeScale = 1.0;

	vec3 bTrans = vec3(0);
	float bScale = 1.0;

	vec3 bushTrans = vec3(0);
	float bushScale = 1.0;

	vec3 pTrans = vec3(0);

	float cTheta = 0;
	bool mouseDown = false;
	int drawMode = 0;

	mat4 view;
	vec3 eye = vec3(0, 0, 2);
	vec3 at = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);
	vec3 at2;

	float pitch;
	float yaw;
	float lastX;
	float lastY;
	float speed = 0.1;
	
	int firstClick = true;
	int right;
	int left;
	int forward;
	int backward;

	float theta;
	float theta1;
	float theta2;
	float theta3;
	float theta4;
	float delta;
	float gtheta;
	float gtheta2;
	float turn;
	int R = true;
	int L;
	int F;
	int B;

	int firstRB = true;
	int firstLF = true;
	int firstLB = true;
	int triggered = false;
	int time = 0;
	int x[10] = {rand()%25, rand() % 25, rand() % 25, rand()%25, rand() % 25, rand() % 25, rand() % 25, rand()%25, rand() % 25,rand() % 25};
	int z[10] = { rand() % 25, rand() % 25, rand() % 25, rand() % 25, rand() % 25, rand() % 25, rand() % 25, rand() % 25, rand() % 25,rand() % 25 };

	float displacement_x;
	float displacement_y;
	float velocity = 0.001;
	float t = 0;

	float bushX = 0;
	float bushZ = 0;
	float bushTurn = 0;


	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (key == GLFW_KEY_M && action == GLFW_PRESS)
		{
			gMat = (gMat + 1) % 4;
		}
		
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
 			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			drawMode = 1;
 		}
 		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
 			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			drawMode = 0;
 		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			left = true;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
			left = false;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			right = true;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
			right = false;
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			forward = true;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
			forward = false;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			backward = true;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
			backward = false;
		}
	}

	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
	{
		cTheta += (float) deltaX;
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			mouseDown = true;
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX << " Pos Y " << posY << endl;
		}

		if (action == GLFW_RELEASE)
		{
			mouseDown = false;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void cursorCallback(GLFWwindow *window, double xpos, double ypos) {
		if (mouseDown) {
			if (firstClick) {
				lastX = xpos;
				lastY = ypos;
				firstClick = false;
			}

			yaw += (lastX - xpos) * 0.01;
			pitch += (ypos - lastY) * 0.01;
			

		}

		if (!mouseDown) {
			firstClick = true;
		}

		if (pitch > 80.0f) {
			pitch = 80.0f;
		}
		if (pitch < -80.0f) {
			pitch = -80.0f;
		}

		at2.x = cos(radians(yaw)) * cos(radians(pitch));
		at2.y = sin(radians(pitch));
		at2.z = cos(radians(pitch)) * sin(radians(yaw));

	}

	unsigned int createSky(string dir, vector<string> faces) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(false);
		for(GLuint i = 0; i < faces.size(); i++) {
    		unsigned char *data = 
			stbi_load((dir+faces[i]).c_str(), &width, &height, &nrChannels, 0);
			if (data) {
    			glTexImage2D(
        			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
        			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			} else {
				std::cout << "failed to load: " << (dir+faces[i]).c_str() << std::endl;
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

		cout << " creating cube map any errors : " << glGetError() << endl;
		return textureID;
	}

	// Code to load in the three textures
	void initTex(const std::string& resourceDirectory)
	{
	 	texture0 = make_shared<Texture>();
		texture0->setFilename(resourceDirectory + "/crate.jpg");
		texture0->init();
		texture0->setUnit(0);
		texture0->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		texture1 = make_shared<Texture>();
		texture1->setFilename(resourceDirectory + "/world.jpg");
		texture1->init();
		texture1->setUnit(1);
		texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		texture2 = make_shared<Texture>();
		texture2->setFilename(resourceDirectory + "/grass.jpg");
		texture2->init();
		texture2->setUnit(2);
		texture2->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		
		texture3 = make_shared<Texture>();
		texture3->setFilename(resourceDirectory + "/butterfly.jpg");
		texture3->init();
		texture3->setUnit(3);
		texture3->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		

		vector<std::string> faces {
    	"iceflats_rt.tga",
    	"iceflats_lf.tga",
    	"iceflats_up.tga",
    	"iceflats_dn.tga",
    	"iceflats_ft.tga",
    	"iceflats_bk.tga"
		}; 
		cubeMapTexture = createSky(resourceDirectory + "/iceflats/",  faces);
	}

	//code to set up the two shaders - a diffuse shader and texture mapping
	void init(const std::string& resourceDirectory)
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		GLSL::checkVersion();

		cTheta = 0;
		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(
			resourceDirectory + "/simple_vert.glsl",
			resourceDirectory + "/simple_frag.glsl");
		if (! prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("MV");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addUniform("view");
		prog->addUniform("MatSpec");
		prog->addUniform("shine");
		prog->addAttribute("vertTex");

		//initialize the textures we might use
		initTex(resourceDirectory);

		texProg = make_shared<Program>();
		texProg->setVerbose(true);
		texProg->setShaderNames(
			resourceDirectory + "/tex_vert.glsl",
			resourceDirectory + "/tex_frag0.glsl");
		if (! texProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
 		texProg->addUniform("P");
		texProg->addUniform("MV");
		texProg->addAttribute("vertPos");
		texProg->addAttribute("vertNor");
		texProg->addAttribute("vertTex");
		texProg->addUniform("Texture0");
		texProg->addUniform("view");

		cubeProg = make_shared<Program>();
		cubeProg->setVerbose(true);
		cubeProg->setShaderNames(
			resourceDirectory + "/cube_vert.glsl",
			resourceDirectory + "/cube_frag.glsl");
		if (! cubeProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
 		cubeProg->addUniform("P");
		cubeProg->addUniform("M");
		cubeProg->addUniform("V");
		cubeProg->addAttribute("vertPos");
		cubeProg->addUniform("view");
	 }

	void initGeom(const std::string& resourceDirectory)
	{
		vector<tinyobj::shape_t> TOshapes;
		vector<tinyobj::material_t> objMaterials;

		string errStr;
		bool rc;

		//---------------------------read in tree-----------------------------------------------------
		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
			(resourceDirectory + "/tree.obj").c_str());

		tree = make_shared<Shape>();
		tree->createShape(TOshapes[0]);
		tree->measure();
		tree->init();

		tree2 = make_shared<Shape>();
		tree2->createShape(TOshapes[1]);
		tree2->init();

		// compute its transforms based on measuring it
		treeTrans = tree->min + 0.5f * (tree->max - tree->min);
		if (tree->max.x > tree->max.y && tree->max.x > tree->max.z)
		{
			treeScale = 2.0 / (tree->max.x - tree->min.x);
		}
		else if (tree->max.y > tree->max.x && tree->max.y > tree->max.z)
		{
			treeScale = 2.0 / (tree->max.y - tree->min.y);
		}
		else
		{
			treeScale = 2.0 / (tree->max.z - tree->min.z);
		}

		float max;
		float min;
		//-----------------------------read in bush----------------------------------------------------
		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
			(resourceDirectory + "/allGrass_001.obj").c_str());

		for (int i = 0; i < TOshapes.size(); i++) {
			grass = make_shared<Shape>();
			grass->createShape(TOshapes[i]);
			grass->measure();
			grass->init();

		    max = grass->max.x;
			min = grass->min.x;


			if (grass->max.y > max) {
				max = grass->max.y;
				min = grass->min.y;
			}
			if (grass->max.z > max) {
				max = grass->max.z;
				min = grass->min.z;
			}

			bushScale = 2.0 / (max - min) * 0.5;
			bushTrans = grass->min + 0.5f * (grass->max - grass->min);
			bush.push_back(grass);
		}
		

		//---------------------------read in butterfly---------------------------------------------------
		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
			(resourceDirectory + "/butterfly.obj").c_str());


		butterfly = make_shared<Shape>();
		butterfly->createShape(TOshapes[0]);
		butterfly->measure();
		butterfly->init();

		max = butterfly->max.x;
		min = butterfly->min.x;


		if (butterfly->max.y > max) {
			max = butterfly->max.y;
			min = butterfly->min.y;
		}
		if (butterfly->max.z > max) {
			max = butterfly->max.z;
			min = butterfly->min.z;
		}

		bScale = 2.0 / (max - min) * 0.5;
		bTrans = butterfly->min + 0.5f * (butterfly->max - butterfly->min);

		//--------------------------------read in cloud---------------------------------------------
		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
			(resourceDirectory + "/cloud.obj").c_str());


		cloud = make_shared<Shape>();
		cloud->createShape(TOshapes[0]);
		cloud->measure();
		cloud->init();

		max = cloud->max.x;
		min = cloud->min.x;


		if (cloud->max.y > max) {
			max = cloud->max.y;
			min = cloud->min.y;
		}
		if (cloud->max.z > max) {
			max = cloud->max.z;
			min = cloud->min.z;
		}

		gCScale = 2.0 / (max - min) * 0.5;
		gCTrans = cloud->min + 0.5f * (cloud->max - cloud->min);


		//----------------------------read in deer---------------------------------------------------

		//head
		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
			(resourceDirectory + "/deer.obj").c_str());


		shape = make_shared<Shape>();
		shape->createShape(TOshapes[0]);
		shape->measure();			
		
		

		max = shape->max.x;
	   min = shape->min.x;
			

		if (shape->max.y > max) {
			max = shape->max.y;
			min = shape->min.y;
		}
		if (shape->max.z > max) {
			max = shape->max.z;
			min = shape->min.z;
		}

		gDScale = 2.0 / (max - min) * 0.5;
		gDTrans = shape->min + 0.5f * (shape->max - shape->min);
		shape->resize(gDTrans, gDScale);
		shape->init();

		deer.push_back(shape);
		//body
		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
			(resourceDirectory + "/sphere.obj").c_str());

		shape = make_shared<Shape>();
		shape->createShape(TOshapes[0]);
		shape->measure();

		max = shape->max.x;
		min = shape->min.x;

		if (shape->max.y > max) {
			max = shape->max.y;
			min = shape->min.y;
		}
		if (shape->max.z > max) {
			max = shape->max.z;
			min = shape->min.z;
		}

		gDScale = 2.0 / (max - min) * 0.5;
		gDTrans = shape->min + 0.5f * (shape->max - shape->min);
		shape->resize(gDTrans, gDScale);
		shape->init();

		deer.push_back(shape);
		//legs
		shape = make_shared<Shape>();
		shape->setCylinder();
		shape->measure();

		max = shape->max.x;
		min = shape->min.x;

		if (shape->max.y > max) {
			max = shape->max.y;
			min = shape->min.y;
		}
		if (shape->max.z > max) {
			max = shape->max.z;
			min = shape->min.z;
		}

		gDScale = 2.0 / (max - min) * 0.5;
		gDTrans = shape->min + 0.5f * (shape->max - shape->min);

		shape->resize(gDTrans, gDScale * 0.25);
		shape->init();

	
		deer.push_back(shape);
		//---------------------------read in poop-------------------------------------------------------
		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
			(resourceDirectory + "/sphere.obj").c_str());

		poop = make_shared<Shape>();
		poop->createShape(TOshapes[0]);
		poop->measure();
		poop->init();

		max = poop->max.x;
		min = poop->min.x;


		pTrans = poop->min + 0.5f * (poop->max - poop->min);
		
		//---------------------------read in cube for cubemap---------------------------------------------
		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
			(resourceDirectory + "/cube.obj").c_str());

		cube = make_shared<Shape>();
		cube->createShape(TOshapes[0]);
		cube->measure();
		cube->init();

	}

	/**** geometry set up for ground plane *****/
	void initQuad()
	{
		float g_groundSize = 20;
		float g_groundY = -1.5;

		// A x-z plane at y = g_groundY of dim[-g_groundSize, g_groundSize]^2
		float GrndPos[] = {
			-g_groundSize, g_groundY, -g_groundSize,
			-g_groundSize, g_groundY,  g_groundSize,
			 g_groundSize, g_groundY,  g_groundSize,
			 g_groundSize, g_groundY, -g_groundSize
		};

		float GrndNorm[] = {
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0
		};

		float GrndTex[] = {
			0, 0, // back
			0, 1,
			1, 1,
			1, 0
		};

		unsigned short idx[] = {0, 1, 2, 0, 2, 3};

		GLuint VertexArrayID;
		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		gGiboLen = 6;
		glGenBuffers(1, &GrndBuffObj);
		glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

		glGenBuffers(1, &GrndNorBuffObj);
		glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

		glGenBuffers(1, &GrndTexBuffObj);
		glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

		glGenBuffers(1, &GIndxBuffObj);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
	}

	void render()
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Leave this code to just draw the meshes alone */
		float aspect = width/(float)height;

		at = eye + normalize(at2);
		vec3 forwardVec = normalize(at - eye);
		vec3 rightVec = normalize(cross(forwardVec, up));
		
		if (forward) {
			eye += forwardVec * speed;
			at += forwardVec * speed;
		}
		if (backward) {
			eye -= forwardVec * speed;
			at -= forwardVec * speed;
		}
		if (right) {
			eye += rightVec * speed;
			at += rightVec * speed;
		}
		if (left) {
			eye -= rightVec * speed;
			at -= rightVec * speed;
		}

		view = lookAt(eye, at, up);

		// Create the matrix stacks
		auto P = make_shared<MatrixStack>();
		auto MV = make_shared<MatrixStack>();
		// Apply perspective projection.
		P->pushMatrix();
		P->perspective(45, aspect, 0.01f, 300.0f);


		//------------------------------------draw tree------------------------------------//
			texProg->bind();
			glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));

			MV->pushMatrix();
			MV->loadIdentity();

			for (int i = 0; i < 10; i++) {
				MV->pushMatrix();			
				MV->translate(vec3(x[i]-10, 4, z[i]-10));				
				MV->scale(treeScale*5);
				MV->translate(-1.0f*treeTrans);
				glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
				glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
				texture0->bind(texProg->getUniform("Texture0"));
				tree->draw(texProg);
				texture2->bind(texProg->getUniform("Texture0"));
				tree2->draw(texProg);
				MV->popMatrix();
			}

			for (int i = 0; i < 10; i++) {
				MV->pushMatrix();
				MV->translate(vec3(x[i]+20, 4, z[i]+20));
				MV->scale(treeScale * 5);
				MV->translate(-1.0f*treeTrans);
				glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
				glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
			
				texture0->bind(texProg->getUniform("Texture0"));
				tree->draw(texProg);
				texture2->bind(texProg->getUniform("Texture0"));
				tree2->draw(texProg);
				MV->popMatrix();
			}

			for (int i = 0; i < 10; i++) {
				MV->pushMatrix();
				MV->translate(vec3(x[i] - 20, 4, z[i] + 20));
				MV->scale(treeScale * 5);
				MV->translate(-1.0f*treeTrans);
				glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
				glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);

				texture0->bind(texProg->getUniform("Texture0"));
				tree->draw(texProg);
				texture2->bind(texProg->getUniform("Texture0"));
				tree2->draw(texProg);
				MV->popMatrix();
			}
			MV->popMatrix();
			texProg->unbind();


		
		
		//-----------------------------------------draw butterfly----------------------------------------
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));

		MV->pushMatrix();
			MV->loadIdentity();

			
				MV->pushMatrix();
				MV->translate(vec3(1 + 3*cos(glfwGetTime()/2), 3 + 0.1*sin(glfwGetTime()*100), -5 + 3*sin(glfwGetTime()/2)));
				MV->scale(bScale);
				MV->translate(-1.0f*bTrans);
				glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
				glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
				SetMaterial(0);
				butterfly->draw(prog);
				MV->popMatrix();
			

				MV->pushMatrix();
				MV->translate(vec3(-3 + 5 * sin(glfwGetTime() / 2), 5 + 0.1*sin(glfwGetTime() * 100), -2 + 4 * cos(glfwGetTime() / 2)));
				MV->scale(bScale);
				MV->translate(-1.0f*bTrans);
				glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
				glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
				SetMaterial(0);
				butterfly->draw(prog);
				MV->popMatrix();
			
		

		MV->popMatrix();
		prog->unbind();

		//----------------------------------draw the deer--------------------------------------------------
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));

		// globle transforms for 'camera' (you will likely wantt to fix this)
		MV->pushMatrix();
		MV->loadIdentity();

			//global transform
			MV->translate(vec3(gtheta, 0.25, gtheta2 + 1));

			
			if (gtheta > 2.0f) {
				R = false;
				F = true;
				turn -= 1.57;
			}
			if (gtheta2 > 2.0f) {
				F = false;
				L = true;
				turn -= 1.57;
			}
			if (gtheta < -2.0f) {
				L = false;
				B = true;
				turn -= 1.57;
			}
			if (gtheta2 < -2.0f) {
				B = false;
				R = true;
				turn -= 1.57;
			}
		
			if (R) {
				gtheta += 0.005f;
				turn = 0;
			}
			if (L) {
				gtheta -= 0.005f;
				turn = -3.14;
			}
			if (F) {
				gtheta2 += 0.005f;
				turn = -1.57;
			}
			if (B) {
				gtheta2 -= 0.005f;
				turn = 1.57;
			}

			MV->rotate(turn, vec3(0, 1, 0));

			SetMaterial(6);
			

			MV->pushMatrix();
				MV->translate(vec3(-0.9, -0.12, 0));

				//--------------------------------the whole right front leg--------------------------------------
				MV->pushMatrix();
					
					MV->translate(vec3(0.5, -0.05, 0.1));
					MV->rotate(theta1, vec3(0, 0, 1));
					MV->translate(vec3(0, -0.5, 0));

					MV->pushMatrix();

						MV->translate(vec3(0, 0, 0));
						MV->rotate(theta1, vec3(0, 0, 1));
						MV->rotate(0.2, vec3(0, 0, 1));
						MV->translate(vec3(-0.05, -0.35, -0.01));


						//Draw right front lower leg
						MV->scale(vec3(0.5, 0.35, 0.5));
						glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
						glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
	
						deer[2]->draw(prog);

					MV->popMatrix();

					MV->pushMatrix();
					//draw right front uppper thigh
					MV->scale(vec3(0.5, 0.35, 0.5));
					glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
					glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
	
					deer[2]->draw(prog);
					MV->popMatrix();

				MV->popMatrix();
				//---------------------------------the whole right back leg--------------------------------------------
				
				MV->pushMatrix();

					MV->translate(vec3(-0.5, -0.04, 0.1));
					MV->rotate(theta2, vec3(0, 0, 1));
					
									
					MV->rotate(-0.2, vec3(0, 0, 1));
					MV->translate(vec3(0, -0.5, 0));

					MV->pushMatrix();

						MV->translate(vec3(0, 0, 0));
						MV->rotate(theta2, vec3(0, 0, 1));
						
						MV->rotate(0.2, vec3(0, 0, 1));
						MV->translate(vec3(-0.05, -0.35, -0.01));


						//Draw lower leg
						MV->scale(vec3(0.5, 0.35, 0.5));
						glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
						glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
	
						deer[2]->draw(prog);

					MV->popMatrix();

					MV->pushMatrix();
					//draw uppper thigh
					MV->scale(vec3(0.5, 0.35, 0.5));
					glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
					glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
	
					deer[2]->draw(prog);
					MV->popMatrix();

				MV->popMatrix();

				//--------------------------------the whole left front leg--------------------------------------
				MV->pushMatrix();

					MV->translate(vec3(0.5, -0.04, -0.1));
					MV->rotate(theta3, vec3(0, 0, 1));
					
					MV->rotate(0.2, vec3(0, 0, 1));
					MV->translate(vec3(0, -0.5, 0));

					MV->pushMatrix();

						MV->translate(vec3(0, 0, 0));
						
						MV->rotate(theta3, vec3(0, 0, 1));
						
						MV->rotate(0.2, vec3(0, 0, 1));
						MV->translate(vec3(-0.05, -0.35, -0.01));


						//Draw lower leg
						MV->scale(vec3(0.5, 0.35, 0.5));
						glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
						glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
	
						deer[2]->draw(prog);

					MV->popMatrix();

					MV->pushMatrix();
					//draw uppper thigh
					MV->scale(vec3(0.5, 0.35, 0.5));
					glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
					glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
		
					deer[2]->draw(prog);
					MV->popMatrix();

				MV->popMatrix();
               
				//--------------------------------the whole left back leg--------------------------------------
				MV->pushMatrix();

					MV->translate(vec3(-0.5, -0.04, -0.1));
					MV->rotate(theta4, vec3(0, 0, 1));

					MV->rotate(0.2, vec3(0, 0, 1));
					MV->translate(vec3(0, -0.5, 0));

					MV->pushMatrix();

						MV->translate(vec3(0, 0, 0));
						MV->rotate(theta4, vec3(0, 0, 1));
				
						MV->rotate(0.2, vec3(0, 0, 1));
						MV->translate(vec3(-0.05, -0.35, -0.01));


						//Draw lower leg
						MV->scale(vec3(0.5, 0.35, 0.5));
						glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
						glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
			
						deer[2]->draw(prog);

					MV->popMatrix();

					MV->pushMatrix();
					//draw uppper thigh
					MV->scale(vec3(0.5, 0.35, 0.5));
					glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
					glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
			
					deer[2]->draw(prog);
					MV->popMatrix();

				MV->popMatrix();
				//--------------------------------------------------------------------------------------
				//draw the body


				MV->scale(vec3(1.5, 1, 1));
				MV->rotate(0.2, vec3(0, 0, 1));
				glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
				glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
	
				deer[1]->draw(prog);


			MV->popMatrix();
			// draw the head			
			MV->scale(vec3(0.5, 0.5, 0.5));
			MV->rotate(theta, vec3(0, 1, 0));
			glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
			glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
	
			deer[0]->draw(prog);



		MV->popMatrix();
		prog->unbind();

		//spinning head
		theta = 0.04 * cos(delta += 0.1f);
		//moving legs
		theta1 = 0.04 *cos(0.2 * (delta += 0.1f) + 0.2);
		theta2 = 0.04 *cos(0.25 * (delta += 0.1f) + 0.03);
		theta3 = 0.04 *sin(0.1 * (delta += 0.1f) + 0.25);
		theta4 = 0.04 *sin(0.15 * (delta += 0.1f) + 0.05);
		//----------------------------------------draw poop------------------------------------------------------
		time++;

		if (time % 600 >= 150) {
			prog->bind();
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));

			MV->pushMatrix();
			MV->loadIdentity();

			MV->translate(vec3(gtheta, 0.25, gtheta2 + 1));
			MV->rotate(turn, vec3(0, 1, 0));

			SetMaterial(2);

			MV->pushMatrix();
			MV->translate(vec3(-0.9, -0.12, 0));

			MV->scale(vec3(1.5, 1, 1));
			MV->rotate(0.2, vec3(0, 0, 1));

			MV->pushMatrix();
			displacement_x = velocity*cos(1.57);
			if (displacement_y > -1.0f)
				displacement_y = velocity*sin(1.57) - 9.8*t;
			if (t < 0.5f)
				t += 0.0005;
			MV->translate(vec3(-0.5 - displacement_x, 0 + displacement_y, 0));

			MV->scale(vec3(0.1, 0.1, 0.1));
			SetMaterial(2);
			glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
			glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
		
			poop->draw(prog);

			MV->popMatrix();

			MV->popMatrix();

			MV->popMatrix();
			prog->unbind();
			triggered = false;
			if (displacement_y <= -0.82f) {
				bushX = gtheta;
				bushZ = gtheta2 + 1;
				bushTurn = turn;
				triggered = true;
				time = 0;
				t = 0;
			}
		}
			
		//--------------------------------------draw grass----------------------------------------------------
		if (triggered) {
			prog->bind();
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));

			MV->pushMatrix();
			MV->loadIdentity();

			MV->translate(vec3(bushX, 0.25, bushZ));
			MV->rotate(bushTurn, vec3(0, 1, 0));

			SetMaterial(5);

			MV->pushMatrix();
			MV->translate(vec3(-0.9, -0.12, 0));

			MV->scale(vec3(1.5, 1, 1));
			MV->rotate(0.2, vec3(0, 0, 1));

			MV->pushMatrix();
			MV->translate(vec3(-2 - displacement_x, displacement_y - 0.1, 0.1));

			MV->scale(vec3(0.1, 0.1, 0.1));
			glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
			glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
		
			for (int i = 0; i < 50; i++)
				bush[i]->draw(prog);

			MV->popMatrix();

			MV->popMatrix();

			MV->popMatrix();
			prog->unbind();
		}

		//-----------------------------draw cows that dont poop----------------------------------------------------
		
		generateCow(P, MV, gtheta + 2, gtheta2 - 2, view);
		generateCow(P, MV, gtheta + 2, gtheta2 + 2, view);
		generateCow(P, MV, gtheta - 2, gtheta2 + 1, view);
		generateCow(P, MV, gtheta - 1, gtheta2 - 1, view);


		//----------------------------------draw the sky box--------------------------------------------------
		cubeProg->bind();
		glUniformMatrix4fv(cubeProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		mat4 ident(1.0);
		glDepthFunc(GL_LEQUAL);
		MV->pushMatrix();
		MV->loadIdentity();
		MV->rotate(radians(cTheta), vec3(0, 1, 0));
		MV->translate(vec3(0, 99, 0));
		MV->scale(100);
		glUniformMatrix4fv(cubeProg->getUniform("V"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(cubeProg->getUniform("M"), 1, GL_FALSE, value_ptr(ident));
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
		glUniformMatrix4fv(cubeProg->getUniform("view"), 1, GL_FALSE, &view[0][0]);
		cube->draw(texProg);
		glDepthFunc(GL_LESS);
		MV->popMatrix();
		cubeProg->unbind();

		P->popMatrix();
	}

	void generateCow(shared_ptr<MatrixStack> P, shared_ptr<MatrixStack> MV, float gtheta, float gtheta2, mat4 view) {
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));

		// globle transforms for 'camera' (you will likely wantt to fix this)
		MV->pushMatrix();
		MV->loadIdentity();

		//global transform
		MV->translate(vec3(gtheta, 0.25, gtheta2));

		if (gtheta > 3.0f) {
			R = false;
			F = true;
			turn -= 1.57;
		}
		if (gtheta2 > 3.0f) {
			F = false;
			L = true;
			turn -= 1.57;
		}
		if (gtheta < -3.0f) {
			L = false;
			B = true;
			turn -= 1.57;
		}
		if (gtheta2 < -3.0f) {
			B = false;
			R = true;
			turn -= 1.57;
		}

		if (R) {
			gtheta += 0.005f;
			turn = 0;
		}
		if (L) {
			gtheta -= 0.005f;
			turn = -3.14;
		}
		if (F) {
			gtheta2 += 0.005f;
			turn = -1.57;
		}
		if (B) {
			gtheta2 -= 0.005f;
			turn = 1.57;
		}

		MV->rotate(turn, vec3(0, 1, 0));

		SetMaterial(6);


		MV->pushMatrix();
		MV->translate(vec3(-0.9, -0.12, 0));

		//--------------------------------the whole right front leg--------------------------------------
		MV->pushMatrix();

		MV->translate(vec3(0.5, -0.05, 0.1));
		MV->rotate(theta1, vec3(0, 0, 1));
		MV->translate(vec3(0, -0.5, 0));

		MV->pushMatrix();

		MV->translate(vec3(0, 0, 0));
		MV->rotate(theta1, vec3(0, 0, 1));
		MV->rotate(0.2, vec3(0, 0, 1));
		MV->translate(vec3(-0.05, -0.35, -0.01));


		//Draw right front lower leg
		MV->scale(vec3(0.5, 0.35, 0.5));
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
		
		deer[2]->draw(prog);

		MV->popMatrix();

		MV->pushMatrix();
		//draw right front uppper thigh
		MV->scale(vec3(0.5, 0.35, 0.5));
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
		
		deer[2]->draw(prog);
		MV->popMatrix();

		MV->popMatrix();
		//---------------------------------the whole right back leg--------------------------------------------

		MV->pushMatrix();

		MV->translate(vec3(-0.5, -0.04, 0.1));
		MV->rotate(theta2, vec3(0, 0, 1));


		MV->rotate(-0.2, vec3(0, 0, 1));
		MV->translate(vec3(0, -0.5, 0));

		MV->pushMatrix();

		MV->translate(vec3(0, 0, 0));
		MV->rotate(theta2, vec3(0, 0, 1));

		MV->rotate(0.2, vec3(0, 0, 1));
		MV->translate(vec3(-0.05, -0.35, -0.01));


		//Draw lower leg
		MV->scale(vec3(0.5, 0.35, 0.5));
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
		
		deer[2]->draw(prog);

		MV->popMatrix();

		MV->pushMatrix();
		//draw uppper thigh
		MV->scale(vec3(0.5, 0.35, 0.5));
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
	
		deer[2]->draw(prog);
		MV->popMatrix();

		MV->popMatrix();

		//--------------------------------the whole left front leg--------------------------------------
		MV->pushMatrix();

		MV->translate(vec3(0.5, -0.04, -0.1));
		MV->rotate(theta3, vec3(0, 0, 1));

		MV->rotate(0.2, vec3(0, 0, 1));
		MV->translate(vec3(0, -0.5, 0));

		MV->pushMatrix();

		MV->translate(vec3(0, 0, 0));

		MV->rotate(theta3, vec3(0, 0, 1));

		MV->rotate(0.2, vec3(0, 0, 1));
		MV->translate(vec3(-0.05, -0.35, -0.01));


		//Draw lower leg
		MV->scale(vec3(0.5, 0.35, 0.5));
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
		
		deer[2]->draw(prog);

		MV->popMatrix();

		MV->pushMatrix();
		//draw uppper thigh
		MV->scale(vec3(0.5, 0.35, 0.5));
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
	
		deer[2]->draw(prog);
		MV->popMatrix();

		MV->popMatrix();

		//--------------------------------the whole left back leg--------------------------------------
		MV->pushMatrix();

		MV->translate(vec3(-0.5, -0.04, -0.1));
		MV->rotate(theta4, vec3(0, 0, 1));

		MV->rotate(0.2, vec3(0, 0, 1));
		MV->translate(vec3(0, -0.5, 0));

		MV->pushMatrix();

		MV->translate(vec3(0, 0, 0));
		MV->rotate(theta4, vec3(0, 0, 1));

		MV->rotate(0.2, vec3(0, 0, 1));
		MV->translate(vec3(-0.05, -0.35, -0.01));


		//Draw lower leg
		MV->scale(vec3(0.5, 0.35, 0.5));
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
		
		deer[2]->draw(prog);

		MV->popMatrix();

		MV->pushMatrix();
		//draw uppper thigh
		MV->scale(vec3(0.5, 0.35, 0.5));
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
	
		deer[2]->draw(prog);
		MV->popMatrix();

		MV->popMatrix();
		//--------------------------------------------------------------------------------------
		//draw the body


		MV->scale(vec3(1.5, 1, 1));
		MV->rotate(0.2, vec3(0, 0, 1));
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);

		deer[1]->draw(prog);


		MV->popMatrix();
		// draw the head			
		MV->scale(vec3(0.5, 0.5, 0.5));
		MV->rotate(theta, vec3(0, 1, 0));
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("view"), 1, GL_FALSE, &view[0][0]);
	
		deer[0]->draw(prog);



		MV->popMatrix();
		prog->unbind();

		//spinning head
		theta = 0.04 * cos(delta += 0.1f);
		//moving legs
		theta1 = 0.04 *cos(0.2 * (delta += 0.1f) + 0.2);
		theta2 = 0.04 *cos(0.25 * (delta += 0.1f) + 0.03);
		theta3 = 0.04 *sin(0.1 * (delta += 0.1f) + 0.25);
		theta4 = 0.04 *sin(0.15 * (delta += 0.1f) + 0.05);

	}

	// helper function to set materials for shading
	void SetMaterial(int i)
	{
		switch (i)
		{
		case 0: //shiny blue plastic
			glUniform3f(prog->getUniform("MatAmb"), 0.02f, 0.04f, 0.2f);
			glUniform3f(prog->getUniform("MatDif"), 0.0f, 0.16f, 0.9f);
			break;
		case 1: // flat grey
			glUniform3f(prog->getUniform("MatAmb"), 0.13f, 0.13f, 0.14f);
			glUniform3f(prog->getUniform("MatDif"), 0.3f, 0.3f, 0.4f);
			break;
		case 2: //brass
			glUniform3f(prog->getUniform("MatAmb"), 0.3294f, 0.2235f, 0.02745f);
			glUniform3f(prog->getUniform("MatDif"), 0.7804f, 0.5686f, 0.11373f);
			break;
		case 3: //copper
			glUniform3f(prog->getUniform("MatAmb"), 0.1913f, 0.0735f, 0.0225f);
			glUniform3f(prog->getUniform("MatDif"), 0.7038f, 0.27048f, 0.0828f);
			break;
		case 4: // turquoise
			glUniform3f(prog->getUniform("MatAmb"), 0.1f, 0.18725f, 0.1745f);
			glUniform3f(prog->getUniform("MatDif"), 0.396f, 0.74151f, 0.69102f);
			glUniform3f(prog->getUniform("MatSpec"), 0.297f, 0.308f, 0.306678f);
			glUniform1f(prog->getUniform("shine"), 0.1f);
			break;
		case 5: // green rubber
			glUniform3f(prog->getUniform("MatAmb"), 0.0f, 0.05f, 0.0f);
			glUniform3f(prog->getUniform("MatDif"), 0.4f, 0.5f, 0.4f);
			glUniform3f(prog->getUniform("MatSpec"), 0.04f, 0.7f, 0.04f);
			glUniform1f(prog->getUniform("shine"), 0.78125f);
			break;
		case 6: // red plastic
			glUniform3f(prog->getUniform("MatAmb"), 0.0f, 0.0f, 0.0f);
			glUniform3f(prog->getUniform("MatDif"), 0.5f, 0.0f, 0.0f);
			glUniform3f(prog->getUniform("MatSpec"), 0.7f, 0.6f, 0.6f);
			glUniform1f(prog->getUniform("shine"), 0.25f);
			break;
		}
	}

};

int main(int argc, char **argv)
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
			resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(512, 512);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
