#include <filesystem>
#include <vector>

#include "Camera.h"
#include "Shader.h"
#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace fs = std::filesystem;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(std::vector<std::string> faces);
glm::vec3 moveTrain(float& X, float& Y, float& Z, float& DegreesY, float& DegreesZ);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
float speed = 1.0f;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{

	std::cout << "<ENTER> Start the train movement\n<BACKSPACE> Stop the train movement\n<1> Driver Camera\n<2> Outside Camera\n<3> Free Camera\n<+> Increase train speed\n<-> Decrease train speed\n";

	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	float rotY = 0, rotZ = 0;

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bucharest to Brasov", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	// build and compile shaders
	Shader skyboxShader("skybox.vs", "skybox.fs");
	Shader trainShader("model.vs", "model.fs");
	Shader terrainShader("model.vs", "model.fs");
	Shader stationShader("model.vs", "model.fs");
	Shader ndStationShader("model.vs", "model.fs");
	Shader bvSignShader("model.vs", "model.fs");
	Shader bucSignShader("model.vs", "model.fs");
	Shader bearShader("model.vs", "model.fs");
	Shader grass1Shader("model.vs", "model.fs");
	Shader grass2Shader("model.vs", "model.fs");
	Shader grass3Shader("model.vs", "model.fs");
	Shader tree1Shader("model.vs", "model.fs");
	Shader tree2Shader("model.vs", "model.fs");

	Shader lightingShader("PhongLight.vs", "PhongLight.fs");
	Shader lightCubeShader("Lamp.vs", "Lamp.fs");
	Shader shader("ShadowMapping.vs", "ShadowMapping.fs");
	Shader simpleDepthShader("ShadowMappingDepth.vs", "ShadowMappingDepth.fs");

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// load textures
	fs::path localPath = fs::current_path();
	std::string textureFolder = localPath.string() + "/Resources/Textures";

	Model driverWagon(localPath.string() + "/Resources/train/tren/emd-gp40-2/textures/loco.obj");
	Model terrain(localPath.string() + "/Resources/terrain/terrain.obj");
	Model station(localPath.string() + "/Resources/station/source/god-knows.obj");
	Model secondStation(localPath.string() + "/Resources/station/source/god-knows.obj");
	Model bvSign(localPath.string() + "/Resources/station/bvSign/ExitSign_HiPoly.obj");
	Model bucSign(localPath.string() + "/Resources/station/bucSign/ExitSign_HiPoly.obj");
	Model bearModel(localPath.string() + "/Resources/other/bear/source/model.obj");
	Model grass1Model(localPath.string() + "/Resources/other/abandoned_building/Project Name.obj");
	Model grass2Model(localPath.string() + "/Resources/other/grass2/Project Name.obj");
	Model grass3Model(localPath.string() + "/Resources/other/grass3/Well-Handle_Finish.obj");
	Model tree1Model(localPath.string() + "/Resources/other/tree2/Project Name.obj");
	Model tree2Model(localPath.string() + "/Resources/other/tree1/Project Name.obj");


	// configure depth map FBO
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// shader configuration
	shader.Shader::use();
	shader.Shader::setInt("diffuseTexture", 0);
	shader.Shader::setInt("shadowMap", 1);

	std::vector<std::string> faces
	{
		textureFolder + "/_right.jpg",
		textureFolder + "/_left.jpg",
		textureFolder + "/_top.jpg",
		textureFolder + "/_bottom.jpg",
		textureFolder + "/_front.jpg",
		textureFolder + "/_back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	float startX = -265.0f;
	float startY = -17.0f;
	float startZ = 190.0f;

	float moveX = -10.0f;
	float moveY = 0.0f;
	float moveZ = 0.0f;

	float degreesY = 0.0f;
	float degreesZ = 0.0f;

	bool start = 0;

	unsigned int key = 0;

	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) // start train
			start = 1;
		if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) // stop train
			start = 0;

		processInput(window);

		lightingShader.use();
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("lightPos", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 3000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);

		// draw the lamp object
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(10.0f));
		lightCubeShader.setMat4("model", model);

		glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw scene as normal
		stationShader.use();
		trainShader.use();
		terrainShader.use();


		trainShader.setMat4("projection", projection);
		trainShader.setMat4("view", view);
		terrainShader.setMat4("projection", projection);
		terrainShader.setMat4("view", view);
		stationShader.setMat4("projection", projection);
		stationShader.setMat4("view", view);


		// render the loaded model

		glm::mat4 train = glm::mat4(1.0f);
		glm::mat4 _terrain = glm::mat4(1.0f);
		glm::mat4 _station = glm::mat4(1.0f);
		glm::mat4 _ndStation = glm::mat4(1.0f);
		glm::mat4 _bvSign = glm::mat4(1.0f);
		glm::mat4 _bucSign = glm::mat4(1.0f);
		glm::mat4 bearModelMatrix = glm::mat4(1.0f);
		glm::mat4 grass1Matrix = glm::mat4(1.0f);
		glm::mat4 grass2Matrix = glm::mat4(1.0f);
		glm::mat4 grass3Matrix = glm::mat4(1.0f);
		glm::mat4 tree1Matrix = glm::mat4(1.0f);
		glm::mat4 tree2Matrix = glm::mat4(1.0f);

	

		glm::vec3 trainPosition;
		if (start) {
			trainPosition = moveTrain(startX, startY, startZ, rotY, rotZ);
		}
		else {
			trainPosition = glm::vec3(startX, startY, startZ);
		}

		// update train's position and render
		train = glm::translate(train, trainPosition);
		train = glm::scale(train, glm::vec3(7.05f, 7.05f, 7.05f));
		train = glm::rotate(train, glm::radians(90.0f + rotY), glm::vec3(0, 1, 0));
		train = glm::rotate(train, glm::radians(0.0f + rotZ), glm::vec3(0, 0, 1));
		trainShader.setMat4("model", train);
		driverWagon.Draw(trainShader);



		// terrain

		_terrain = glm::translate(_terrain, glm::vec3(650.0f, -38.0f, -750.0f));
		_terrain = glm::scale(_terrain, glm::vec3(2500.0f, 2500.0f, 2500.0f));
		terrainShader.setMat4("model", _terrain);
		terrain.Draw(terrainShader);

		// start station
		_station = glm::translate(_station, glm::vec3(-390.0f, 40.0f, 276.0f));
		_station = glm::scale(_station, glm::vec3(0.12f, 0.12f, 0.12f));
		_station = glm::rotate(_station, glm::radians(90.0f), glm::vec3(0, 1, 0));
		stationShader.setMat4("model", _station);
		station.Draw(stationShader);

		// end station
		_ndStation = glm::translate(_ndStation, glm::vec3(-170.0f, 83.0f, -1890.0f));
		_ndStation = glm::scale(_ndStation, glm::vec3(0.12f, 0.12f, 0.12f));
		_ndStation = glm::rotate(_ndStation, glm::radians(3.0f), glm::vec3(0, 1, 0));
		ndStationShader.setMat4("model", _ndStation);
		secondStation.Draw(ndStationShader);

		// bucuresti sign
		_bucSign = glm::translate(_bucSign, glm::vec3(-291.0f, 50.0f, 188.0f));
		_bucSign = glm::scale(_bucSign, glm::vec3(9.0f, 10.0f, 15.0f));
		_bucSign = glm::rotate(_bucSign, glm::radians(90.0f), glm::vec3(0, 1, 0));
		bucSignShader.setMat4("model", _bucSign);
		bucSign.Draw(bvSignShader);

		// brasov sign
		_bvSign = glm::translate(_bvSign, glm::vec3(-77.0f, 93.5f, -1795.0f));
		_bvSign = glm::scale(_bvSign, glm::vec3(20.0f, 10.0f, 70.0f));
		_bvSign = glm::rotate(_bvSign, glm::radians(0.0f), glm::vec3(0, 1, 0));
		bvSignShader.setMat4("model", _bvSign);
		bvSign.Draw(bucSignShader);
		//bear
		float x = 60.5f;
		for (int i = 1; i <= 1; i++)
		{
			bearModelMatrix = glm::translate(bearModelMatrix, glm::vec3(50.0f, x-32, -1710.0f)); 
			bearModelMatrix = glm::scale(bearModelMatrix, glm::vec3(0.2f, 0.2f, 0.2f)); 
			bearModelMatrix = glm::rotate(bearModelMatrix, glm::radians(40.0f), glm::vec3(0, 1, 0)); 
			bearShader.setMat4("model", bearModelMatrix);
			bearModel.Draw(bearShader);
			x += i * 10;

		}

		
		//tree1
		float tree2x = -330.0f, tree2y = 35.5f, tree2z = -1900.0f;
		float tree2xsize = 0.40f, tree2ysize = 0.40f, tree2zsize = 0.40f;
		glm::mat4 originalTree2Matrix = tree2Matrix;
		tree2Matrix = glm::translate(tree2Matrix, glm::vec3(tree2x,tree2y,tree2z));
		tree2Matrix = glm::scale(tree2Matrix, glm::vec3(tree2zsize , tree2ysize , tree2zsize ));
		tree2Matrix = glm::rotate(tree2Matrix, glm::radians(40.0f), glm::vec3(0, 1, 0));
		tree2Shader.setMat4("model", tree2Matrix);
		tree2Model.Draw(tree2Shader);
		
		tree2x = -330.0f;
		tree2y = 40.5f;
		tree2z = -1700.0f;
		tree2Matrix = originalTree2Matrix;
		tree2Matrix = glm::translate(tree2Matrix, glm::vec3(tree2x, tree2y, tree2z));
		tree2Matrix = glm::scale(tree2Matrix, glm::vec3(tree2zsize, tree2ysize, tree2zsize));
		tree2Matrix = glm::rotate(tree2Matrix, glm::radians(130.0f), glm::vec3(0, 1, 0));
		tree2Shader.setMat4("model", tree2Matrix);
		tree2Model.Draw(tree2Shader);

		tree2x = 260.0f;
		tree2y = 25.0f;
		tree2z = -1590.0f;
		tree2Matrix = originalTree2Matrix;
		tree2Matrix = glm::translate(tree2Matrix, glm::vec3(tree2x, tree2y, tree2z));
		tree2Matrix = glm::scale(tree2Matrix, glm::vec3(tree2zsize, tree2ysize, tree2zsize));
		tree2Matrix = glm::rotate(tree2Matrix, glm::radians(220.0f), glm::vec3(0, 1, 0));
		tree2Shader.setMat4("model", tree2Matrix);
		tree2Model.Draw(tree2Shader);

		tree2x = -320.0f;
		tree2y = -20.0f;
		tree2z = 390.0f;
		tree2xsize = 0.35f;
		tree2ysize = 0.35f;
		tree2zsize = 0.35f;
		tree2Matrix = originalTree2Matrix;
		tree2Matrix = glm::translate(tree2Matrix, glm::vec3(tree2x, tree2y, tree2z));
		tree2Matrix = glm::scale(tree2Matrix, glm::vec3(tree2zsize, tree2ysize, tree2zsize));
		tree2Matrix = glm::rotate(tree2Matrix, glm::radians(310.0f), glm::vec3(0, 1, 0));
		tree2Shader.setMat4("model", tree2Matrix);
		tree2Model.Draw(tree2Shader);

	

		//tree2
		glm::mat4 originalTree1Matrix = tree1Matrix;
		float tree1x = 330.0f, tree1y = 17.0f, tree1z = -1850.0f;
		float tree1xsize = 0.4f, tree1ysize = 0.4f, tree1zsize = 0.4f;
		tree1Matrix = glm::translate(tree1Matrix, glm::vec3(tree1x, tree1y, tree1z));
		tree1Matrix = glm::scale(tree1Matrix, glm::vec3(tree1zsize , tree1ysize , tree1zsize ));
		tree1Matrix = glm::rotate(tree1Matrix, glm::radians(80.0f), glm::vec3(0, 1, 0)); //am marit gradele si fata s a invartit spre dreapta
		tree1Shader.setMat4("model", tree1Matrix);
		tree1Model.Draw(tree1Shader);

		tree1x = -210.0f;
		tree1y = -11.0f;
		tree1z = -270.0f;
		tree1Matrix = originalTree1Matrix;
		tree1Matrix = glm::translate(tree1Matrix, glm::vec3(tree1x, tree1y, tree1z));
		tree1Matrix = glm::scale(tree1Matrix, glm::vec3(tree1zsize, tree1ysize, tree1zsize));
		tree1Matrix = glm::rotate(tree1Matrix, glm::radians(315.0f), glm::vec3(0, 1, 0)); //am marit gradele si fata s a invartit spre dreapta
		tree1Shader.setMat4("model", tree1Matrix);
		tree1Model.Draw(tree1Shader);

		// // am micsorat z s a dat in dreapta // am micsorat x  s a dat in spate

		tree1x = -100.0f;
		tree1y = -18.0f;
		tree1z = 160.0f;
		tree1xsize = 0.35f;
		tree1ysize = 0.35f;
		tree1zsize = 0.35f;
		tree1Matrix = originalTree1Matrix;
		tree1Matrix = glm::translate(tree1Matrix, glm::vec3(tree1x, tree1y, tree1z));
		tree1Matrix = glm::scale(tree1Matrix, glm::vec3(tree1zsize, tree1ysize, tree1zsize));
		tree1Matrix = glm::rotate(tree1Matrix, glm::radians(-7.0f), glm::vec3(0, 1, 0)); //am marit gradele si fata s a invartit spre dreapta
		tree1Shader.setMat4("model", tree1Matrix);
		tree1Model.Draw(tree1Shader);



		// // am micsorat z s a dat in dreapta // am micsorat x  s a dat in spate

		// abandoned station
		
		float grass1x = 300.0f, grass1y = 31.5f, grass1z = -800.0f;
		float grass1xsize = 8.40f, grass1ysize = 8.40f, grass1zsize = 8.40f;
		glm::mat4 originalGrass1Matrix = grass1Matrix;
		grass1Matrix = glm::translate(grass1Matrix, glm::vec3(grass1x, grass1y, grass1z));
		grass1Matrix = glm::scale(grass1Matrix, glm::vec3(grass1zsize, grass1ysize, grass1zsize));
		grass1Matrix = glm::rotate(grass1Matrix, glm::radians(30.0f), glm::vec3(0, 1, 0));
		grass1Shader.setMat4("model", grass1Matrix);
		grass1Model.Draw(grass1Shader);

		//eagle
		float grass2x = 300.0f, grass2y = 231.5f, grass2z = -800.0f;
		float grass2xsize = 0.00240f, grass2ysize = 0.00240f, grass2zsize = 0.00240f;
		glm::mat4 originalGrass2Matrix = grass2Matrix;
		grass2Matrix = glm::translate(grass2Matrix, glm::vec3(grass2x, grass2y, grass2z));
		grass2Matrix = glm::scale(grass2Matrix, glm::vec3(grass2zsize, grass2ysize, grass2zsize));
		grass2Matrix = glm::rotate(grass2Matrix, glm::radians(30.0f), glm::vec3(0, 1, 0));
		grass2Shader.setMat4("model", grass2Matrix);
		grass2Model.Draw(grass2Shader);


		grass2Matrix = originalGrass2Matrix;
		grass2x = 400.0f;
		grass2y = 231.5f;
		grass2z = -1000.0f;
		grass2Matrix = glm::translate(grass2Matrix, glm::vec3(grass2x, grass2y, grass2z));
		grass2Matrix = glm::scale(grass2Matrix, glm::vec3(grass2zsize, grass2ysize, grass2zsize));
		grass2Matrix = glm::rotate(grass2Matrix, glm::radians(30.0f), glm::vec3(0, 1, 0));
		grass2Shader.setMat4("model", grass2Matrix);
		grass2Model.Draw(grass2Shader);
		// // am micsorat z s a dat in dreapta // am micsorat x  s a dat in spate

		grass2Matrix = originalGrass2Matrix;
		grass2x = -250.0f;
		grass2y = 100.5f;
		grass2z = -150.0f;
		grass2Matrix = glm::translate(grass2Matrix, glm::vec3(grass2x, grass2y, grass2z));
		grass2Matrix = glm::scale(grass2Matrix, glm::vec3(grass2zsize, grass2ysize, grass2zsize));
		grass2Matrix = glm::rotate(grass2Matrix, glm::radians(110.0f), glm::vec3(0, 1, 0));
		grass2Shader.setMat4("model", grass2Matrix);
		grass2Model.Draw(grass2Shader);

		//fountain
		float grass3x = 450.0f, grass3y =11.5f, grass3z = -400.0f;
		float grass3xsize = 20.240f, grass3ysize = 20.240f, grass3zsize = 20.240f;
		glm::mat4 originalGrass3Matrix = grass3Matrix;
		grass3Matrix = glm::translate(grass3Matrix, glm::vec3(grass3x, grass3y, grass3z));
		grass3Matrix = glm::scale(grass3Matrix, glm::vec3(grass3zsize, grass3ysize, grass3zsize));
		grass3Matrix = glm::rotate(grass3Matrix, glm::radians(90.0f), glm::vec3(0, 1, 0));
		grass3Shader.setMat4("model", grass3Matrix);
		grass3Model.Draw(grass3Shader);
		
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // driver camera
			key = 1;
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // 3rd person camera
			key = 2;
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) // free camera
			key = 3;

		switch (key)
		{

		case 1:
			camera.setViewMatrix(glm::vec3(startX +8, startY + 19, startZ -38));

			break;
		case 2:
			camera.setViewMatrix(glm::vec3(startX , startY + 50, startZ + 100));
			break;
		default:
			break;
		}

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); 
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); 
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);

	glfwTerminate();
	return 0;
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		camera.printPosition();
	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
	{
		if (speed <= 2.5)
			speed += 0.5;
		//std::cout << speed << "\n";
	}
	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
	{
		if (speed > 1.5)
			speed -= 0.5;
		//std::cout << speed << "\n";
	}
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

glm::vec3 moveTrain(float& X, float& Y, float& Z, float& DegreesY, float& DegreesZ)
{

	if (X == -265 && Z > 114)
	{
		Z -= 0.1 * speed;
	}
	else if (X < -248 && Z > -40)
	{
		if (DegreesY > -12.5)
			DegreesY -= 0.2;

		X += 0.02 * speed;
		Z -= 0.1 * speed;
		Y += 0.004 * speed;
	}
	else if (X < -214 && Z > -28)
	{
		if (DegreesY > -27)
			DegreesY -= 0.2;

		X += 0.05 * speed;
		Z -= 0.082 * speed;
		Y += 0.002 * speed;
	}
	else if (X < -170 && Z > -82)
	{
		if (DegreesY > -40)
			DegreesY -= 0.3;

		X += 0.06 * speed;
		Z -= 0.09 * speed;
		Y += 0.002 * speed;
	}
	else if (X < -111 && Z > -139)
	{
		if (DegreesY > -52)
			DegreesY -= 0.3;

		X += 0.07 * speed;
		Z -= 0.07 * speed;
		Y += 0.002 * speed;
	}
	else if (X < -54 && Z > -174)
	{
		if (DegreesY > -60)
			DegreesY -= 0.3;

		X += 0.09 * speed;
		Z -= 0.055 * speed;
		Y += 0.002 * speed;
	}
	else if (X < 159 && Z > -248)
	{

		if (DegreesY > -73)
			DegreesY -= 0.3;

		X += 0.09 * speed;
		Z -= 0.028 * speed;
		Y += 0.003 * speed;
	}
	else if (X < 270 && Z > -353)
	{
		if (DegreesY < -50)
			DegreesY += 0.3;

		X += 0.07 * speed;
		Z -= 0.07 * speed;
		Y += 0.003 * speed;
	}
	else if (X < 303 && Z > -419)
	{
		if (DegreesY < -25)
			DegreesY += 0.3;

		X += 0.04 * speed;
		Z -= 0.07 * speed;
		Y += 0.004 * speed;
	}
	else if (X < 329 && Z > -492)
	{
		if (DegreesY < -17)
			DegreesY += 0.3;

		X += 0.04 * speed;
		Z -= 0.11 * speed;
		Y += 0.006 * speed;
	}
	else if (X < 340 && Z > -566)
	{
		if (DegreesY < 0)
			DegreesY += 0.3;
		if (DegreesZ < 5)
			DegreesZ += 0.3;

		X += 0.02 * speed;
		Z -= 0.09 * speed;
		Y += 0.01 * speed;
	}
	else if (X < 349 && Z > -657)
	{
		X += 0.008 * speed;
		Z -= 0.1 * speed;
		Y += 0.012 * speed;
	}
	else if (X < 359 && Z > -766)
	{
		X += 0.014 * speed;
		Z -= 0.13 * speed;
		Y += 0.012 * speed;
	}
	else if (X < 365 && Z > -838)
	{
		X += 0.01 * speed;
		Z -= 0.1 * speed;
		Y += 0.006 * speed;
	}
	else if (X< 369 && Z > -901)
	{

		X += 0.006 * speed;
		Z -= 0.1 * speed;
		Y += 0.013 * speed;
	}
	else if (X < 371 && Z > -965)
	{
		X += 0.003 * speed;
		Z -= 0.1 * speed;
		Y += 0.017 * speed;
	}
	else if (X < 372 && Z > -1059)
	{
		Z -= 0.12 * speed;
		Y += 0.016 * speed;
	}
	else if (X < 372 && Z > -1137)
	{
		Z -= 0.12 * speed;
		Y += 0.025 * speed;
	}
	else if (X < 376 && Z > -1240)
	{
		if (DegreesZ > -10)
			DegreesZ -= 0.3;
		if (X > 390)
			Y -= 0.015;

		X += 0.003 * speed;
		Z -= 0.12 * speed;
		Y -= 0.024 * speed;
	}
	else if (X < 404 && Z > -1435)
	{
		if (DegreesZ > -15)
			DegreesZ -= 0.3;
		if (DegreesY < -5)
			DegreesY += 0.3;

		X += 0.02 * speed;
		Z -= 0.13 * speed;
		Y -= 0.03 * speed;
	}
	else if (X < 406 && Z > -1507)
	{
		if (DegreesZ < 0)
			DegreesZ += 0.3;

		X += 0.003 * speed;
		Z -= 0.13 * speed;
		Y -= 0.01 * speed;
	}
	else if (X > 390 && Z > -1582)
	{
		if (DegreesY < 15)
			DegreesY += 0.3;

		X -= 0.01 * speed;
		Z -= 0.1 * speed;
		Y -= 0.01 * speed;
	}
	else if (X > 351 && Z > -1648)
	{
		if (DegreesY < 30)
			DegreesY += 0.3;

		X -= 0.064 * speed;
		Z -= 0.1 * speed;
		Y -= 0.004 * speed;
	}
	else if (X > 294 && Z > -1705)
	{
		if (DegreesY < 50)
			DegreesY += 0.3;

		X -= 0.1 * speed;
		Z -= 0.1 * speed;
	}
	else if (X > 240 && Z > -1735)
	{
		if (DegreesY < 60)
			DegreesY += 0.3;

		X -= 0.12 * speed;
		Z -= 0.08 * speed;
	}
	else if (X > 167 && Z > -1761)
	{
		if (DegreesY < 70)
			DegreesY += 0.3;

		X -= 0.15 * speed;
		Z -= 0.06 * speed;
		Y -= 0.004 * speed;
	}
	else if (X > -128 && Z > -1763)
	{
		if (DegreesY < 90)
			DegreesY += 0.3;

		X -= 0.16 * speed;
		Z -= 0.01 * speed;
		Y += 0.007;
	}
	else if (X > -136 && Z > -1765)
	{
		X -= 0.13 * speed;
		Z -= 0.0013 * speed;
		Y += 0.007 * speed;
	}

	return glm::vec3(X, Y, Z);
}