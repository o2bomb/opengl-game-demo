#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "Shader.h"
#include "Camera.h"

// INITIALIZE OBJECTS IN WORLD SPACE
void createGameOver(unsigned int VAO, unsigned int gameScreenTex, Shader lightingShader);
void createGround(unsigned int VAO, unsigned int dirtTex, unsigned int dirtTex2, Shader lightingShader);
void createSky(unsigned int VAO, unsigned int skyTex, Shader lightingShader);
void createSven(unsigned int VAO, unsigned int svenBodyTex, unsigned int svenHeadTex, Shader lightingShader);
void createWatersheep(unsigned int VAO, unsigned int waterSheepBodyTex, unsigned int waterSheepHeadTex, unsigned int waterSheepHeadTex2, Shader lightingShader);
void createTree(unsigned int VAO, unsigned int woodTex, unsigned int woodTex2, unsigned int leafTex, unsigned int leafTex2, Shader lightingShader);
void createZombie(unsigned int VAO, unsigned int zombieBodyTex, unsigned int zombieHeadTex, unsigned int zombieLegTex, unsigned int podzolTex, Shader lightingShader);
void createLamp(unsigned int lightVAO, unsigned int VAO, unsigned int handleTex, Shader lightingShader, Shader lampShader, glm::mat4 projection, glm::mat4 orthographic, glm::mat4 view);
void createPortal(unsigned int VAO, unsigned int structureTex, unsigned int portalTex, Shader lightingShader);

// INTERACTION TOGGLES
void resetGame();
void checkGameStatus();
void checkLampDistance();
void checkSvenDistance();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// HELPER METHODS
float* getRatio(float value1, float value2);
void moveObject();
void updateInput();
unsigned int loadTexture(char const * path);

// INITIAL POSITION OF CAMERA IN WORLD SPACE
Camera camera(glm::vec3(0.0f, 0.6f, 9.0f));

// INITIAL POSITION OF LAMP IN WORLD SPACE
glm::vec3 lightPos(0.0f, 0.4f, 8.0f);

// INITIAL POSITION OF SVEN IN WORLD SPACE
glm::vec3 svenPos(-3.5f, 0.35f, -1.0f);

// INITIAL POSITION OF WATER SHEEP IN WORLD SPACE
glm::vec3 waterSheepPos(-2.5f, 0.35f, -7.0f);

// INITIAL POSITION OF ZOMBIE IN WORLD SPACE
glm::vec3 zombiePos(1.0f, 0.0f, 8.0f);

// INITIAL POSITION OF PORTAL IN WORLD SPACE
glm::vec3 portalPos(9.0f, 0.0f, 0.0f);

// PI VALUE
const float PI = 3.14159;

// WIDTH & HEIGHT OF VIEWPORT
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

bool firstMouse = false;

// is the player holding the object?
bool holdLamp = false;
bool holdSven = false;

// toggle for orthographic/perspective projection
bool orthographicToggle = false;

// toggles for object interaction
bool lampIsClose;
bool svenIsClose;
bool gameIsHorror;
bool gameIsLost;
bool gameIsWon;

// is the scene dark?
bool isDark = false;

// is the player in the air?
bool isJump;
// position of camera during jump
float POSITION_JUMP;
// jump time of player
float TIME_JUMP;

// initial values for lighting
float quadraticAtten = 0.25;
float linearAtten = 0.7;

float deltaTime = 0.0f;	
float lastFrame = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

// used in updateInput()
int LIGHT_DELAY = 0;
int TOGGLE_DELAY = 0;
int PICKUP_DELAY = 0;
int ORTHO_DELAY = 0;

int main()
{
	// INITIALIZE WINDOW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Save Sven From Water Sheep!", NULL, NULL);
	if(window == NULL)
	{
		std::cout << "Error creating the window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);
    	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Error in initialising and loading in GLAD" << std::endl;
		return -1;
	}

	// LOAD WORLD AND LAMP SHADERS
	Shader lightingShader("lightingVS.vs", "lightingFS.fs");
	Shader lampShader("lampVS.vs", "lampFS.fs");

	// DEFINE BOX VERTEX POSITIONS
	float vertices[] = {
		// position           // colour             // texture
		// NORTH
	    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,

		// SOUTH
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,

		// EAST
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,


		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

		// WEST
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,

		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

		// BOTTOM
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,

		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		// TOP
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	// LOAD OBJECT TEXTURES
	unsigned int handleTex = loadTexture(FileSystem::getPath("resources/textures/handle.jpg").c_str());
	unsigned int skyTex = loadTexture(FileSystem::getPath("resources/textures/sky.jpg").c_str());
	unsigned int skyTex2 = loadTexture(FileSystem::getPath("resources/textures/bars.png").c_str());
	unsigned int dirtTex = loadTexture(FileSystem::getPath("resources/textures/dirt.jpg").c_str());
	unsigned int dirtTex2 = loadTexture(FileSystem::getPath("resources/textures/bloody.png").c_str());
	unsigned int svenBodyTex = loadTexture(FileSystem::getPath("resources/textures/svenBody.jpg").c_str());
	unsigned int svenHeadTex = loadTexture(FileSystem::getPath("resources/textures/svenHead.png").c_str());
	unsigned int waterSheepBodyTex = loadTexture(FileSystem::getPath("resources/textures/waterSheepBody.jpg").c_str());
	unsigned int waterSheepHeadTex = loadTexture(FileSystem::getPath("resources/textures/sheepHead.jpg").c_str());
	unsigned int waterSheepHeadTex2 = loadTexture(FileSystem::getPath("resources/textures/waterSheepFace.png").c_str());
	unsigned int woodTex = loadTexture(FileSystem::getPath("resources/textures/wood.jpg").c_str());
	unsigned int woodTex2 = loadTexture(FileSystem::getPath("resources/textures/obsidian.png").c_str());
	unsigned int leafTex = loadTexture(FileSystem::getPath("resources/textures/leaf.png").c_str());	
	unsigned int leafTex2 = loadTexture(FileSystem::getPath("resources/textures/netherack.png").c_str());
	unsigned int zombieBodyTex = loadTexture(FileSystem::getPath("resources/textures/zombieBody.jpg").c_str());
	unsigned int zombieHeadTex = loadTexture(FileSystem::getPath("resources/textures/zombieHead.jpg").c_str());
	unsigned int zombieLegTex = loadTexture(FileSystem::getPath("resources/textures/denim.jpeg").c_str());
	unsigned int podzolTex = loadTexture(FileSystem::getPath("resources/textures/podzol.jpg").c_str());
	unsigned int gameOverTex = loadTexture(FileSystem::getPath("resources/textures/gameover.png").c_str());
	unsigned int gameWinTex = loadTexture(FileSystem::getPath("resources/textures/gamewin.jpg").c_str());
	unsigned int structureTex = loadTexture(FileSystem::getPath("resources/textures/glowstone.jpeg").c_str());
	unsigned int portalTex = loadTexture(FileSystem::getPath("resources/textures/portal.jpeg").c_str());



	// CREATE VERTEX ARRAY OBJECT AND VERTEXT BUFFER OBJECT FOR WORLD OBJECTS
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(VAO);

	// POSITION INDEX
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// COLOUR INDEX
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// TEXTURE INDEX
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// CREATE VERTEX ARRAY OBJECT FOR LAMP OBJECT
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// ACTIVATE WORLD SHADER
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);

	while (!glfwWindowShouldClose(window))
	{
    
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        // set background colour to black
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glEnable(GL_DEPTH_TEST);
        
        // prevent overlapping of front-facing sides
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // prevent flickering
		updateInput();

		lightingShader.use();
		// set initial position of light
		lightingShader.setVec3("light.position", lightPos);

        // set initial camera position
		lightingShader.setVec3("viewPos", camera.Position);
		
		// check game status
		checkGameStatus();

        // toggle the darkness/brightness of the scene
		if(isDark)
		{
        	// increase ambient, diffuse, and specular values
			lightingShader.setVec3("light.ambient", 2.1f, 2.1f, 2.1f);
			lightingShader.setVec3("light.diffuse", 0.3f, 0.3f, 0.3f);
			lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
			
            // increase attenuation value
			lightingShader.setFloat("light.constant", 1.0f);
			lightingShader.setFloat("light.linear", 1.0f);
			lightingShader.setFloat("light.quadratic", 1.0f);
		}
		else
		{
			// decrease ambient, diffuse, and specular values
			lightingShader.setVec3("light.ambient", 0.4f, 0.4f, 0.4f);
			lightingShader.setVec3("light.diffuse", 0.3f, 0.3f, 0.3f);
			lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

			// decrease attenuation value to defaults
			lightingShader.setFloat("light.constant", 0.01f);
			lightingShader.setFloat("light.linear", linearAtten);
			lightingShader.setFloat("light.quadratic", quadraticAtten);
		}	

		// default lighting values
        lightingShader.setFloat("light.constant", 1.0f);
        lightingShader.setFloat("light.linear", linearAtten);
		lightingShader.setFloat("light.quadratic", quadraticAtten);


        // the lamp is being held
		if(holdLamp)
		{
			glm::vec3 tempPosition = camera.Position;
			glm::vec3 tempFront = camera.Front;

			// floating animation of lamp and light source
			tempPosition.y = tempPosition.y + sin(glfwGetTime()) / 60;
			tempFront.y = tempFront.y +sin(glfwGetTime() * 3) / 40;

            // position lamp in front of player
			lightPos = tempPosition + tempFront;

            // prevent lamp from going below the ground
			if(lightPos.y <= 0.2)
			{
				lightPos.y = 0.2;
			}
		}
		else
		{
			// slower floating animation
			lightPos.y = lightPos.y + sin(glfwGetTime() * 10) / 250;
			if(lightPos.y <= 0.2)
			{
				lightPos.y = 0.2;
			}
		}

		// Sven is being held
		if(holdSven)
		{
			glm::vec3 tempPosition = camera.Position;
			glm::vec3 tempFront = camera.Front;
			
			svenPos = tempPosition + tempFront;
			if(svenPos.y <= 0.35)
			{
				svenPos.y = 0.35;
			}

			// turn the game into horror mode
			gameIsHorror = true;
		}

		lightingShader.setFloat("material.shininess", 32.0f);
        
		// INITIALIZE PERSPECTIVE PROJECTION MATRIX
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        
		// INITIALIZE ORTHOGRAPHIC PROJECTION MATRIX
		glm::mat4 orthographic = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.0f, 20.0f);
		glm::mat4 view = camera.GetViewMatrix();

		// TOGGLE BETWEEN PERSPECTIVE/ORTHOGRAPHIC PROJECTION
		if(orthographicToggle)
		{
			lightingShader.setMat4("projection", orthographic);
		}

		else
		{
			lightingShader.setMat4("projection", projection);

		}
		lightingShader.setMat4("view", view);
		
		// GAME OVER SCREEN
		if(gameIsLost)
		{
			// display game over screen
			camera.Front = glm::vec3(0.0f, 0.0f, -1.0f);
			camera.Position.x = 0.0f;
			camera.Position.z = 0.0f;
			camera.Pitch = 0.0f;
			camera.Yaw = -90.0f;
			orthographicToggle = true;
			isDark = true;
			createGameOver(VAO, gameOverTex, lightingShader);
		}
		else if(gameIsWon)
		{
			// display game won screen
			camera.Front = glm::vec3(0.0f, 0.0f, -1.0f);
			camera.Position.x = 0.0f;
			camera.Position.z = 0.0f;
			camera.Pitch = 0.0f;
			camera.Yaw = -90.0f;
			isDark = true;
			orthographicToggle = true;
			createGameOver(VAO, gameWinTex, lightingShader);
		}
		else
		{
			// CREATE OBJECTS IN WORLD SPACE
			if(!gameIsHorror)
				createSky(VAO, skyTex, lightingShader);
			createSky(VAO, skyTex2, lightingShader);
			createTree(VAO, woodTex, woodTex2, leafTex, leafTex2, lightingShader);
			createPortal(VAO, structureTex, portalTex, lightingShader);
			createGround(VAO, dirtTex, dirtTex2, lightingShader);
			createSven(VAO, svenBodyTex, svenHeadTex, lightingShader);
			createWatersheep(VAO, waterSheepBodyTex, waterSheepHeadTex, waterSheepHeadTex2, lightingShader);
			createZombie(VAO, zombieBodyTex, zombieHeadTex, zombieLegTex, podzolTex, lightingShader);
			createLamp(lightVAO, VAO, handleTex, lightingShader, lampShader, projection, orthographic, view);
		}

		/*
			SOURCE(S):
				https://www.gamedev.net/forums/topic/490713-jump-formula/
		*/
		// if the player is jumping, perform gravity calculations
		if(isJump)
		{
			float gravity = -6.0;
			camera.Position.y = POSITION_JUMP + 6 * (2 * TIME_JUMP + gravity * pow(TIME_JUMP, 2));
            
            // the player is falling
			if((camera.Position.y < POSITION_JUMP) && TIME_JUMP > 0)
			{
				isJump = false;
				camera.Position.y = POSITION_JUMP;
			}
		}
		processInput(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();

	return 0;
}

// CAPTURE KEYBOARD INPUT
void processInput(GLFWwindow *window)
{
	// exit the game
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	// exit the game
	if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		resetGame();
	}

	// jump upwards
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !isJump)
	{
		TIME_JUMP = 0;
		isJump = true;
		POSITION_JUMP = camera.Position.y;
	}

	// move forwards
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, deltaTime, isJump);
		moveObject();
	}
	// move backwards
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime, isJump);
		moveObject();
	}

	// move to the left
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, deltaTime, isJump);
		moveObject();
	}
	
	// move to the right
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, deltaTime, isJump);
		moveObject();
	}
    
	// interact with lamp 
	if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && PICKUP_DELAY == 0)
	{
		PICKUP_DELAY = 20;
		checkLampDistance();
	
		if(lampIsClose && !holdLamp)
			holdLamp = true;
		else if(holdLamp)
			holdLamp = false;
	}

	// interact with Sven 
	if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && PICKUP_DELAY == 0)
	{
		PICKUP_DELAY = 20;
		checkSvenDistance();
	
		if(svenIsClose && !holdSven)
			holdSven = true;
		else if(holdSven)
			holdSven = false;
	}

	// toggle type of projection
	if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && ORTHO_DELAY == 0)
	{
		ORTHO_DELAY = 20;
		if(!orthographicToggle)
			orthographicToggle = true;
		else
			orthographicToggle = false;
	}

    	// toggle dark or bright scenery 
	if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && LIGHT_DELAY == 0)
	{
		LIGHT_DELAY = 20;
		if(!isDark)
			isDark = true;
		else
			isDark = false;
	}

	// increase attenuation of light source
	if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		linearAtten /= 1.2;
		quadraticAtten /= 1.2*1.2;
        
        if(linearAtten <= 0.05f)
			linearAtten = 0.05f;

		if(quadraticAtten <= 0.0075)
			quadraticAtten = 0.0075;
	}

	// decrease attenuation of light source
	if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		linearAtten *= 1.2;
		quadraticAtten *= 1.2*1.2;

		if(linearAtten >= 0.8)
			linearAtten = 0.8;

		if(quadraticAtten >= 2.0)
			quadraticAtten = 2.0;
	}
}

// CREATE GAME OVER SCREEN
void createGameOver(unsigned int VAO, unsigned int gameScreenTex, Shader lightingShader)
{
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gameScreenTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gameScreenTex);
	glm::mat4 model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.1f));
	model = glm::scale(model, glm::vec3(30.0f, 20.0f, 5.0f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

// CREATE LAMP OBJECT IN WORLD SPACE
void createLamp(unsigned int lightVAO, unsigned int VAO, unsigned int handleTex, Shader lightingShader, Shader lampShader, glm::mat4 projection, glm::mat4 orthographic, glm::mat4 view)
{
	// the handle
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, handleTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glm::mat4 model;
	model = glm::mat4();
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.1f, 0.3f, 0.1f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// the light source
	glBindVertexArray(lightVAO);
	lampShader.use();
	if(orthographicToggle)
		lampShader.setMat4("projection", orthographic);
	else
		lampShader.setMat4("projection", projection);
	model = glm::mat4();
	model = glm::translate(model, lightPos);
	model = glm::translate(model, glm::vec3(0.0f, 0.2, 0.0f));
	if(holdLamp)
	{
		model = glm::rotate(model, (float)glfwGetTime() * 10, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f)); // a smaller cube
	lampShader.setMat4("view", view);
	lampShader.setFloat("intensity", 1.0);
	lampShader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

// CREATE PORTAL OBJECT IN WORLD SPACE
void createPortal(unsigned int VAO, unsigned int structureTex, unsigned int portalTex, Shader lightingShader)
{
	// the portal consists of 14 blocks
	glm::vec3 blockPositions[] = 
	{
		// first half of portal
		glm::vec3(0.0f, 0.2f, 0.2f),
		glm::vec3(0.0f, 0.2f, 0.6f),
		glm::vec3(0.0f, 0.6f, 0.6f),
		glm::vec3(0.0f, 1.0f, 0.6f),
		glm::vec3(0.0f, 1.4f, 0.6f),
		glm::vec3(0.0f, 1.8f, 0.6f),
		glm::vec3(0.0f, 1.8f, 0.2f),
		// second half of portal
		glm::vec3(0.0f, 0.2f, -0.2f),
		glm::vec3(0.0f, 0.2f, -0.6f),
		glm::vec3(0.0f, 0.6f, -0.6f),
		glm::vec3(0.0f, 1.0f, -0.6f),
		glm::vec3(0.0f, 1.4f, -0.6f),
		glm::vec3(0.0f, 1.8f, -0.6f),
		glm::vec3(0.0f, 1.8f, -0.2f),
	};

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, structureTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, structureTex);
	int numberOfBlocks = sizeof(blockPositions)/sizeof(blockPositions[0]);
	// render all 14 blocks of the portal
	for(int i = 0; i < numberOfBlocks; i++)
	{	
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, portalPos);
		model = glm::translate(model, blockPositions[i]);
		model = glm::scale(model, glm::vec3(0.4f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// the portal's insides consists of 6 blocks
	glm::vec3 portalPositions[] = 
	{
		glm::vec3(0.0f, 0.6f, 0.2f),
		glm::vec3(0.0f, 1.0f, 0.2f),
		glm::vec3(0.0f, 1.4f, 0.2f),
		glm::vec3(0.0f, 0.6f, -0.2f),
		glm::vec3(0.0f, 1.0f, -0.2f),
		glm::vec3(0.0f, 1.4f, -0.2f),
	};

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, portalTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, portalTex);
	int numberOfPortals = sizeof(portalPositions)/sizeof(portalPositions[0]);
	// render the insides of the portal
	for(int i = 0; i < numberOfPortals; i++)
	{	
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, portalPos);
		model = glm::translate(model, portalPositions[i]);
		model = glm::scale(model, glm::vec3(0.01f, 0.4f, 0.4f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

// CREATE SKY WALL OBJECTS IN WORLD SPACE
void createSky(unsigned int VAO, unsigned int skyTex, Shader lightingShader)
{
	glm::vec3 skyPositions[] = 
	{
		// NORTH
		glm::vec3(0.0f, 5.0f, 10.0f),
		// SOUTH
		glm::vec3(0.0f, 5.0f, -10.0f),
		// EAST
		glm::vec3(10.0f, 5.0f, 0.0f),
		// WEST
		glm::vec3(-10.0f, 5.0f, 0.0f),
		// ROOF
		glm::vec3(0.0f, 10.0f, 0.0f)
	};

	glm::vec3 skyScales[] = 
	{
		// NORTH
		glm::vec3(20.0f, 20.0f, 0.01f),
		// SOUTH
		glm::vec3(20.0f, 20.0f, 0.01f),
		// EAST
		glm::vec3(0.01f, 20.0f, 20.0f),
		// WEST
		glm::vec3(0.01f, 20.0f, 20.0f),
		// ROOF
		glm::vec3(20.0f, 0.01f, 20.0f)
	};

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skyTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, skyTex);
	// render all 5 planes of sky
	for(int i = 0; i < 5; i++)
	{	
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, skyPositions[i]);
		model = glm::scale(model, skyScales[i]);
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

// CREATE GROUND PLANE OBJECT IN WORLD SPACE
void createGround(unsigned int VAO, unsigned int dirtTex, unsigned int dirtTex2, Shader lightingShader)
{
	if(gameIsHorror)
		dirtTex = dirtTex2;

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dirtTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, dirtTex);
	glm::mat4 model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(20.0f, 0.01f, 20.0f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

// CREATE WATER SHEEP OBJECT IN WORLD SPACE
void createSven(unsigned int VAO, unsigned int svenBodyTex, unsigned int svenHeadTex, Shader lightingShader)
{
	glm::mat4 model;

	// Sven's head
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, svenHeadTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, svenHeadTex);
	model = glm::mat4();
	model = glm::translate(model, svenPos);
	model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Sven's body
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, svenBodyTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, svenBodyTex);
	model = glm::mat4();
	model = glm::translate(model, svenPos);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.25f));
	model = glm::scale(model, glm::vec3(0.275f, 0.275f, 0.5f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Sven's legs
	glm::vec3 legPositions[] = {
		// front left leg
		glm::vec3(-0.075f, -0.2f, 0.05f),
		// front right leg 
		glm::vec3(0.075f, -0.2f, 0.05f),
		// back left leg 
		glm::vec3(-0.075f, -0.2f, 0.4f),
		// back right leg
		glm::vec3(0.075f, -0.2f, 0.4f),
	};
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, svenBodyTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, svenBodyTex);
	for(int i = 0; i < 4; i++)
	{
		model = glm::mat4();
		model = glm::translate(model, svenPos);
		model = glm::translate(model, legPositions[i]);
		model = glm::scale(model, glm::vec3(0.075f, 0.3f, 0.075f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// Sven's tail
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, svenBodyTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, svenBodyTex);
	model = glm::mat4();
	model = glm::translate(model, svenPos);
	if(holdSven)
		model = glm::rotate(model, sin((float)glfwGetTime() * 5), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.525f));
	model = glm::scale(model, glm::vec3(0.075f, 0.3f, 0.075f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

// CREATE WATER SHEEP OBJECT IN WORLD SPACE
void createWatersheep(unsigned int VAO, unsigned int waterSheepBodyTex, unsigned int waterSheepHeadTex, unsigned int waterSheepHeadTex2, Shader lightingShader)
{
	glm::mat4 model;
	float sheepRotation = ((180 * PI) / 180);
	float* xzRatio;
	xzRatio = getRatio(camera.Position.x, camera.Position.z);
	float xVelocity = 0.25f * xzRatio[0] * deltaTime;
	float zVelocity = 0.25f * xzRatio[1] * deltaTime;

	glm::vec3 playerPosition = camera.Position - waterSheepPos;
	glm::vec3 rotationAxis = glm::vec3(0.0f, 0.0f, -0.5f);

	if(gameIsHorror)
	{
		waterSheepHeadTex = waterSheepHeadTex2;
		// DEFINE ZOMBIE AI
		// if zombie isn't in the players range, then chase the player
		if(playerPosition.x > 0.0f && playerPosition.z > 0.0f)
		{
			sheepRotation = ((180 * PI) / 180) + atan(playerPosition.x / playerPosition.z);
			// if water sheep isn't in the players range, then chase the player
			if((glm::length(camera.Position - waterSheepPos)) > 0.5f)
			{
				waterSheepPos.x += xVelocity;
				waterSheepPos.z += zVelocity;
			}
		}
		else if(playerPosition.x > 0.0f && playerPosition.z < 0.0f)
		{
			sheepRotation = atan(playerPosition.x / playerPosition.z);
			// if water sheep isn't in the players range, then chase the player
			if((glm::length(camera.Position - waterSheepPos)) > 0.5f)
			{
				waterSheepPos.x += xVelocity;
				waterSheepPos.z -= zVelocity;
			}
		}
		else if(playerPosition.x < 0.0f && playerPosition.z < 0.0f)
		{
			sheepRotation = atan(playerPosition.x / playerPosition.z);
			// if water sheep isn't in the players range, then chase the player
			if((glm::length(camera.Position - waterSheepPos)) > 0.5f)
			{
				waterSheepPos.x -= xVelocity;
				waterSheepPos.z -= zVelocity;
			}
		}
		else if(playerPosition.x < 0.0f && playerPosition.z > 0.0f)
		{
			sheepRotation = ((180 * PI) / 180) + atan(playerPosition.x / playerPosition.z);
			if((glm::length(camera.Position - waterSheepPos)) > 0.5f)
			{
				waterSheepPos.x -= xVelocity;
				waterSheepPos.z += zVelocity;
			}
		}
	}

	// Water sheep's head
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterSheepHeadTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterSheepHeadTex);
	model = glm::mat4();
	model = glm::translate(model, waterSheepPos);
	model = glm::rotate(model, sheepRotation, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, rotationAxis);
	model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Water sheep's body
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterSheepBodyTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterSheepBodyTex);
	model = glm::mat4();
	model = glm::translate(model, waterSheepPos);
	model = glm::rotate(model, sheepRotation, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, rotationAxis);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.25f));
	model = glm::scale(model, glm::vec3(0.275f, 0.275f, 0.5f));
	lightingShader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Water sheeps's legs
	glm::vec3 legPositions[] = {
		// front left leg
		glm::vec3(-0.075f, -0.2f, 0.05f),
		// front right leg 
		glm::vec3(0.075f, -0.2f, 0.05f),
		// back left leg 
		glm::vec3(-0.075f, -0.2f, 0.4f),
		// back right leg
		glm::vec3(0.075f, -0.2f, 0.4f),
	};
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterSheepBodyTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterSheepBodyTex);
	for(int i = 0; i < 4; i++)
	{
		model = glm::mat4();
		model = glm::translate(model, waterSheepPos);
		model = glm::rotate(model, sheepRotation, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, rotationAxis);
		model = glm::translate(model, legPositions[i]);
		model = glm::scale(model, glm::vec3(0.075f, 0.3f, 0.075f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

// CREATE ZOMBIE OBJECT IN WORLD SPACE
void createZombie(unsigned int VAO, unsigned int zombieBodyTex, unsigned int zombieHeadTex, unsigned int zombieLegTex, unsigned int podzolTex, Shader lightingShader)
{
	glm::mat4 model;
	float zombieRotation = ((180 * PI) / 180);
	float* xzRatio;
	xzRatio = getRatio(camera.Position.x, camera.Position.z);
	float xVelocity = 0.25f * xzRatio[0] * deltaTime;
	float zVelocity = 0.25f * xzRatio[1] * deltaTime;

	glm::vec3 playerPosition = camera.Position - zombiePos;
	glm::vec3 rotationAxis = glm::vec3(0.0f, 0.0f, -0.5f);

	if(gameIsHorror)
	{
		// DEFINE ZOMBIE AI
		// if zombie isn't in the players range, then chase the player
		if(playerPosition.x > 0.0f && playerPosition.z > 0.0f)
		{
			zombieRotation = ((180 * PI) / 180) + atan(playerPosition.x / playerPosition.z);
			if((glm::length(camera.Position - zombiePos)) > 0.5f)
			{
				zombiePos.x += xVelocity;
				zombiePos.z += zVelocity;
			}
		}
		else if(playerPosition.x > 0.0f && playerPosition.z < 0.0f)
		{
			zombieRotation = atan(playerPosition.x / playerPosition.z);
			if((glm::length(camera.Position - zombiePos)) > 0.5f)
			{
				zombiePos.x += xVelocity;
				zombiePos.z -= zVelocity;
			}
		}
		else if(playerPosition.x < 0.0f && playerPosition.z < 0.0f)
		{
			zombieRotation = atan(playerPosition.x / playerPosition.z);
			if((glm::length(camera.Position - zombiePos)) > 0.5f)
			{
				zombiePos.x -= xVelocity;
				zombiePos.z -= zVelocity;
			}
		}
		else if(playerPosition.x < 0.0f && playerPosition.z > 0.0f)
		{
			zombieRotation = ((180 * PI) / 180) + atan(playerPosition.x / playerPosition.z);
			if((glm::length(camera.Position - zombiePos)) > 0.5f)
			{
				zombiePos.x -= xVelocity;
				zombiePos.z += zVelocity;
			}
		}

		// zombie head
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, zombieHeadTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, zombieHeadTex);
		model = glm::mat4();
		model = glm::translate(model, zombiePos);
		model = glm::rotate(model, zombieRotation, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, rotationAxis);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// zombie torso
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, zombieBodyTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, zombieBodyTex);
		model = glm::mat4();
		model = glm::translate(model, zombiePos);
		model = glm::rotate(model, zombieRotation, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, rotationAxis);
		model = glm::translate(model, glm::vec3(0.0f, 0.7f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.4f, 0.15f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// zombie left arm
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, zombieBodyTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, zombieBodyTex);
		model = glm::mat4();
		model = glm::translate(model, zombiePos);
		model = glm::rotate(model, zombieRotation, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, rotationAxis);
		model = glm::translate(model, glm::vec3(-0.225f, 0.8f, -0.15f));
		model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.45f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// zombie right arm
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, zombieBodyTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, zombieBodyTex);
		model = glm::mat4();
		model = glm::translate(model, zombiePos);
		model = glm::rotate(model, zombieRotation, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, rotationAxis);
		model = glm::translate(model, glm::vec3(0.225f, 0.8f, -0.15f));
		model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.45f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// zombie left leg
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, zombieLegTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, zombieLegTex);
		model = glm::mat4();
		model = glm::translate(model, zombiePos);
		model = glm::rotate(model, zombieRotation, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, rotationAxis);
		model = glm::translate(model, glm::vec3(0.075f, 0.3f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, 0.0f));
		model = glm::rotate(model, cos((float)glfwGetTime() * 5), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.125f, 0.5f, 0.125f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// zombie right leg
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, zombieLegTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, zombieLegTex);
		model = glm::mat4();
		model = glm::translate(model, zombiePos);
		model = glm::rotate(model, zombieRotation, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, rotationAxis);
		model = glm::translate(model, glm::vec3(-0.075f, 0.3f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, 0.0f));
		model = glm::rotate(model, sin((float)glfwGetTime() * 5), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.125f, 0.5f, 0.125f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	else
	{
		// zombie head
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, zombieHeadTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, zombieHeadTex);
		model = glm::mat4();
		model = glm::translate(model, zombiePos);
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// podzol
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, podzolTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, podzolTex);
		model = glm::mat4();
		model = glm::translate(model, zombiePos);
		model = glm::scale(model, glm::vec3(0.5f, 0.1f, 0.5f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// zombie arm
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, zombieBodyTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, zombieBodyTex);
		model = glm::mat4();
		model = glm::translate(model, zombiePos);
		model = glm::translate(model, glm::vec3(0.3f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.4f, 0.15f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

// CREATE TREE OBJECTS IN WORLD SPACE
void createTree(unsigned int VAO, unsigned int woodTex, unsigned int woodTex2, unsigned int leafTex, unsigned int leafTex2, Shader lightingShader)
{
	// define positions of 4 trees
	glm::vec3 treePositions[] =
	{
		glm::vec3(1.0f,0.0f,1.0f),
		glm::vec3(1.0f,0.0f,-1.0f),
		glm::vec3(-1.0f,0.0f,1.0f),
		glm::vec3(-1.0f,0.0f,-1.0f),
		glm::vec3(2.0f,0.0f,2.0f),
		glm::vec3(2.0f,0.0f,-2.0f),
		glm::vec3(-2.0f,0.0f,2.0f),
		glm::vec3(-2.0f,0.0f,-2.0f),
		glm::vec3(4.0f,0.0f,4.0f),
		glm::vec3(4.0f,0.0f,-4.0f),
		glm::vec3(-4.0f,0.0f,4.0f),
		glm::vec3(-4.0f,0.0f,-4.0f),
		glm::vec3(2.0f,0.0f,4.0f),
		glm::vec3(2.0f,0.0f,-4.0f),
		glm::vec3(-2.0f,0.0f,4.0f),
		glm::vec3(-2.0f,0.0f,-4.0f),
		glm::vec3(4.0f,0.0f,2.0f),
		glm::vec3(4.0f,0.0f,-2.0f),
		glm::vec3(-4.0f,0.0f,2.0f),
		glm::vec3(-4.0f,0.0f,-2.0f),
		glm::vec3(8.0f,0.0f,8.0f),
		glm::vec3(8.0f,0.0f,-8.0f),
		glm::vec3(-8.0f,0.0f,8.0f),
		glm::vec3(-8.0f,0.0f,-8.0f),
		glm::vec3(6.0f,0.0f,8.0f),
		glm::vec3(6.0f,0.0f,-8.0f),
		glm::vec3(-6.0f,0.0f,8.0f),
		glm::vec3(-6.0f,0.0f,-8.0f),
		glm::vec3(4.0f,0.0f,8.0f),
		glm::vec3(4.0f,0.0f,-8.0f),
		glm::vec3(-4.0f,0.0f,8.0f),
		glm::vec3(-4.0f,0.0f,-8.0f),
		glm::vec3(2.0f,0.0f,8.0f),
		glm::vec3(2.0f,0.0f,-8.0f),
		glm::vec3(-2.0f,0.0f,8.0f),
		glm::vec3(-2.0f,0.0f,-8.0f)
	};

	glm::vec3 woodPositions[] =
	{
		glm::vec3(0.0f,0.1f,0.0f),
		glm::vec3(0.0f,0.55f,0.0f),
		glm::vec3(0.0f,1.0f,0.0f),
		glm::vec3(0.0f,1.45f,0.0f)
	};
	
	int numberOfTrees = sizeof(treePositions)/sizeof(treePositions[0]);
	glm::mat4 model;
	if(gameIsHorror)
	{
		woodTex = woodTex2;
		leafTex = leafTex2;
	}

	for(int i = 0; i < numberOfTrees; i++)
	{
		// trunk
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, woodTex);
		model = glm::mat4();
		for(int j = 0; j < 4; j++)
		{
			model = glm::mat4();
			model = glm::translate(model, treePositions[i]);
			model = glm::translate(model, woodPositions[j]);
			model = glm::scale(model, glm::vec3(0.45f));
			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// leaves
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, leafTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, leafTex);
		model = glm::mat4();
		model = glm::translate(model, treePositions[i]);
		if(gameIsHorror)
		{
			model = glm::translate(model, glm::vec3(0.0f, sin((float)glfwGetTime() * 10) / 10, 0.0f));
		}
		model = glm::translate(model, glm::vec3(0.0f,2.5f,0.0f));
		model = glm::scale(model, glm::vec3(1.70f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

}

// RESET THE GAME
void resetGame()
{
	// RESET CORE GAME TOGGLES
	holdSven = false;
	holdLamp = false;
	gameIsLost = false;
	gameIsWon = false;
	gameIsHorror = false;
	lampIsClose = false;
	svenIsClose = false;
	orthographicToggle = false;
	isDark = false;

	// RESET POSITION OF CAMERA IN WORLD SPACE
	camera.Position = glm::vec3(0.0f, 0.6f, 9.0f);
	camera.WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	camera.Front = glm::vec3(0.0f, 0.0f, -1.0f);

	// RESET POSITION OF LAMP IN WORLD SPACE
	lightPos = glm::vec3(0.0f, 0.4f, 8.0f);

	// RESET POSITION OF SVEN IN WORLD SPACE
	svenPos = glm::vec3(-3.5f, 0.35f, -1.0f);

	// RESET POSITION OF WATER SHEEP IN WORLD SPACE
	waterSheepPos = glm::vec3(-2.5f, 0.35f, -7.0f);

	// RESET POSITION OF ZOMBIE IN WORLD SPACE
	zombiePos = glm::vec3(1.0f, 0.0f, 8.0f);

	// RESET POSITION OF PORTAL IN WORLD SPACE
	portalPos = glm::vec3(9.0f, 0.0f, 0.0f);
}

// CHECK IF GAME IS LOST OR WON
void checkGameStatus()
{
	// has the zombie or water sheep caught the player?
	if(gameIsHorror)
	{
		if((glm::length(camera.Position - zombiePos)) <= 1.0f || (glm::length(camera.Position - waterSheepPos)) <= 0.5f)
		{
			gameIsLost = true;
		}

		// has the player saved Sven?
		if((glm::length(svenPos - portalPos)) <= 1.0f)
		{
			gameIsWon = true;
			gameIsLost = false;
		}
	}
}

// CHECK DISTANCE OF LAMP TO PLAYER
void checkLampDistance()
{
	// is the lamp close enough to be picked up?
	if((glm::length(camera.Position - lightPos)) <= 1.2f)
	{
		lampIsClose = true;
	}
	else
	{
		lampIsClose = false;
	}
}

// CHECK DISTANCE OF SVEN TO PLAYER
void checkSvenDistance()
{
	// is Sven close enough to be picked up?
	if((glm::length(camera.Position - svenPos)) <= 1.2f)
	{
		svenIsClose = true;
	}
	else
	{
		svenIsClose = false;
	}
}

// TRACK LAMP WITH CAMERA MOVEMENT
void moveObject()
{
	if(holdLamp)
	{
		glm::vec3 tempPosition = camera.Position;
		glm::vec3 tempFront = camera.Front;

		tempPosition.y = tempPosition.y + +sin(glfwGetTime())/10;
		tempFront.y = tempFront.y + +sin(glfwGetTime()+glfwGetTime()+ glfwGetTime() + glfwGetTime())/10;
		lightPos = tempPosition + tempFront;		
	}

	if(holdSven)
	{
		glm::vec3 tempPosition = camera.Position;
		glm::vec3 tempFront = camera.Front;
			
		svenPos = tempPosition + tempFront;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	
	glViewport(0, 0, width, height);
} 

/*
    SOURCE(s):
	https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/7.3.camera_mouse_zoom/camera_mouse_zoom.cpp
*/
// PERFORM CAMERA MOVEMENT THROUGH MOUSE INPUT
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if(firstMouse)
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


/*
    SOURCE(s):
	https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/4.2.lighting_maps_specular_map/lighting_maps_specular.cpp
*/
// LOAD A 2D TEXTURE FROM A FILE
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if(data)
	{
		GLenum format;
		if(nrComponents == 1)
			format = GL_RED;
		else if(nrComponents == 3)
			format = GL_RGB;
		else if(nrComponents == 4)
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

/*
    SOURCE(s):
	Yong Yang Tan, Curtin University
*/
// FINDS THE RATIO OF TWO GIVEN FLOATS
// RETURNS THE RATIO IN THE FORM OF AN ARRAY (POINTER)
// WILL ALWAYS RETURN POSITIVE RATIOS
float* getRatio(float value1, float value2)
{
	// get absolute value of both values
	float number1 = fabs(value1);
	float number2 = fabs(value2);

	static float arr[2];
	if(number1 > number2)
	{
        arr[0] = number1 / number2;
		arr[1] = 1.0f;
	}
    else if(number1 < number2)
	{
        arr[0] = 1.0f;
		arr[1] = number2 / number1;
	}
	else
	{
		arr[0] = 1.0f;
		arr[1] = 1.0f;
	}

	return arr;
}

/*
    SOURCE(s):
	Antoni Liang, Curtin University
*/
// PREVENT FLICKERING WITH INPUT
void updateInput()
{
	if(LIGHT_DELAY > 0)
	{
		LIGHT_DELAY -= 1.1;
	}

	if(TIME_JUMP < 0.5f)
	{
		TIME_JUMP += 0.01;
	}

	if(TOGGLE_DELAY > 0)
	{
		TOGGLE_DELAY -= 1;
	}
	
	if(PICKUP_DELAY > 0)
	{
		PICKUP_DELAY -= 1;
	}
	
	if(ORTHO_DELAY > 0)
	{
		ORTHO_DELAY -= 1;
	}
}

