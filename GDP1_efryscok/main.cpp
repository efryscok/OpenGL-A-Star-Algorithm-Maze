#include "global.h"

// Methods
void initialize();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
float randf(float LO, float HI);
int randi(int LO, int HI);
void setupShaderManager();
void setupMeshManager();
void setupTextureManager();
void setupPathFinder();
void setupGlobals();
void shutdown(const std::string& msg);
void update();

int main() {
	// Memory Leak Check
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Initialize Random Number Generator
	srand(static_cast<unsigned int>(time(NULL)));

	// Setup Game
	initialize();
	setupShaderManager();
	setupMeshManager();
	setupTextureManager();
	setupPathFinder();
	setupGlobals();

	double lastTime = glfwGetTime();
	static const float kMaxDeltaTime = 0.1f;

	while (!glfwWindowShouldClose(gWindow)) {
		glViewport(0, 0, gCamera->width, gCamera->height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwGetFramebufferSize(gWindow, &gCamera->width, &gCamera->height);
		gCamera->aspectRatio = static_cast<float>(gCamera->width) / static_cast<float>(gCamera->height);

		gDeltaTime = float(glfwGetTime() - lastTime);
		lastTime = glfwGetTime();

		if (gDeltaTime > kMaxDeltaTime) {
			gDeltaTime = kMaxDeltaTime;
		}

		update();
		gCamera->lookAt = gPlayer->position + (gPlayer->orientation * gPlayer->offset);
		gCamera->position = gPlayer->position + (gPlayer->orientation * gCamera->offset);
		gCamera->update();
		gMeshManager->update();

		// Swap the glfw window buffers
		glfwSwapBuffers(gWindow);
		glfwPollEvents();
	}

	delete gMeshManager;
	delete gShaderManager;
	delete gTextureManager;
	delete gCamera;
	delete gPathFinder;
	delete gPlayerTimer;
	delete gEnemyFastTimer;
	delete gEnemySlowTimer;

	for (std::vector<CModel*>::iterator modelIT = gModels.begin(); modelIT != gModels.end(); ++modelIT) {
		delete *modelIT;
	}

	glfwDestroyWindow(gWindow);
	glfwTerminate();
}

void initialize() {
	if (!glfwInit()) {
		std::cout << "[ERROR] Failed to initilize glfw" << std::endl << "Press any key to continue . . . ";
		_getch();
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "[INFO] Initialized glfw" << std::endl;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	gWindow = glfwCreateWindow(1200, 800, "Erik Fryscok", NULL, NULL);

	if (!gWindow) {
		shutdown("Failed to open glfw window");
	}
	else {
		std::cout << "[INFO] Opened glfw window" << std::endl;
	}

	glfwSetKeyCallback(gWindow, key_callback);
	glfwSetInputMode(gWindow, GLFW_STICKY_KEYS, GL_TRUE);
	glfwMakeContextCurrent(gWindow);
	glfwSwapInterval(1);
	
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}

	if (gGameOver) {
		return;
	}

	if (key == GLFW_KEY_I && action == GLFW_PRESS && gCheatStep == 0) {
		gCheating = true;
		++gCheatStep;
		gPlayerTimer->Reset();
	}
	else if (key == GLFW_KEY_D && action == GLFW_PRESS && gCheatStep == 1 && gPlayerTimer->GetElapsedSeconds() < 3.f) {
		++gCheatStep;
	}
	else if (key == GLFW_KEY_K && action == GLFW_PRESS && gCheatStep == 2 && gPlayerTimer->GetElapsedSeconds() < 3.f) {
		++gCheatStep;
	}
	else if (key == GLFW_KEY_F && action == GLFW_PRESS && gCheatStep == 3 && gPlayerTimer->GetElapsedSeconds() < 3.f) {
		++gCheatStep;
	}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS && gCheatStep == 4 && gPlayerTimer->GetElapsedSeconds() < 3.f) {
		gCheating = false;
		gCheatStep = 0;
		gCheatCode = true;
		gPlayerTimer->Reset();
	}
	else if (action != GLFW_RELEASE) {
		gCheating = false;
		gCheatStep = 0;
	}
}

float randf(float LO, float HI) {
	return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
}

int randi(int LO, int HI) {
	return rand() % HI + LO;
}

void setupShaderManager() {
	gShaderManager = new CShaderManager("assets/shaders/");

	// Load, attach, and compile the vertex and fragment shaders
	if (!gShaderManager->loadShaders("vertexShader.glsl", "fragmentShader.glsl")) {
		shutdown("Failed to load shaders");
	}
	else {
		std::cout << "[INFO] Loaded shader files" << std::endl;
	}

	gShaderManager->getUniformLocations();
}

void setupMeshManager() {
	gMeshManager = new CMeshManager("assets/models/");

	// List all the models and load them from file
	std::vector<std::string> modelFiles{
		"box.obj",
	};

	for (std::vector<std::string>::iterator it = modelFiles.begin(); it != modelFiles.end(); ++it) {
		if (gMeshManager->loadModel(*it)) {
			std::cout << "[INFO] Loaded model \"" << *it << "\"" << std::endl;
		}
	}
}

void setupTextureManager() {
	gTextureManager = new CTextureManager("assets/textures/");

	// List all the textures and load them from file
	std::vector<std::string> textureFiles{};

	for (std::vector<std::string>::iterator it = textureFiles.begin(); it != textureFiles.end(); ++it) {
		if (gTextureManager->loadTexture(*it)) {
			std::cout << "[INFO] Loaded texture \"" << *it << "\"" << std::endl;
		}
		else {
			std::cout << "[ERROR] Could not load texture \"" << *it << "\"" << std::endl;
		}
	}
}

void setupPathFinder() {
	std::vector<S24BitBMPPixel> maze = gTextureManager->loadMaze("mazeFinal.bmp");
	gMazeWidth = gMazeHeight = static_cast<int>(sqrt(maze.size()));

	int pixelCount = 0;
	std::vector<std::vector<int>> map = std::vector<std::vector<int>>(gMazeWidth, std::vector<int>(gMazeHeight, 0));

	for (int col = 0; col < gMazeWidth; ++col) {
		for (int row = 0; row < gMazeHeight; ++row, ++pixelCount) {
			CModel* mazeSquare = new CModel();
			mazeSquare->mesh = "box.obj";
			mazeSquare->position = glm::vec3(col, 0.f, row);
			mazeSquare->colour = glm::vec4(maze[pixelCount].redPixel / 255.f, maze[pixelCount].greenPixel / 255.f, maze[pixelCount].bluePixel / 255.f, 1.f);

			if (mazeSquare->colour == gColourWhite) {
				gWalkway.push_back(glm::vec3(mazeSquare->position.x, 1.f, mazeSquare->position.z));
			}
			else if (mazeSquare->colour == gColourGreen) {
				gStart.push_back(glm::vec3(mazeSquare->position.x, 1.f, mazeSquare->position.z));
			}
			else if (mazeSquare->colour == gColourRed) {
				gGoal.push_back(glm::vec3(mazeSquare->position.x, 1.f, mazeSquare->position.z));
			}
			else if (mazeSquare->colour > gColourDarkGrey && mazeSquare->colour < gColourLightGrey) {
				mazeSquare->position.y = 1.f;
				map[col][row] = 1;
			}

			gModels.push_back(mazeSquare);
		}
	}

	gPathFinder = new CPathFinder(map);
}

void setupGlobals() {
	// CAMERA
	gCamera = new CCamera();
	gCamera->fieldOfView = 1.5f;

	// PLAYER
	gPlayerTimer = new CHRTimer();
	gPlayerTimer->Start();

	gPlayer = new CModel();
	gPlayer->mesh = "box.obj";
	gPlayer->position = gStart[randi(0, gStart.size() - 1)];
	gPlayer->colour = glm::vec4(0.f, 0.f, 1.f, 1.f);
	gPlayer->offset = glm::vec3(0.f, 0.f, -1.f);
	gPlayer->scale = 0.5f;
	gPlayer->movementSpeed = 0.1f;
	gModels.push_back(gPlayer);

	// ENEMIES
	gEnemyFastTimer = new CHRTimer();
	gEnemyFastTimer->Start();

	gEnemyFast = new CModel();
	gEnemyFast->mesh = "box.obj";
	gEnemyFast->position = gWalkway[randi(0, gWalkway.size() - 1)];
	gEnemyFast->colour = glm::vec4(1.f, 0.f, 0.f, 1.f);
	gEnemyFast->scale = 0.5f;
	gModels.push_back(gEnemyFast);

	gEnemySlowTimer = new CHRTimer();
	gEnemySlowTimer->Start();

	gEnemySlow = new CModel();
	gEnemySlow->mesh = "box.obj";
	gEnemySlow->position = gWalkway[randi(0, gWalkway.size() - 1)];
	gEnemySlow->colour = glm::vec4(0.f, 1.f, 0.f, 1.f);
	gEnemySlow->scale = 0.5f;
	gModels.push_back(gEnemySlow);
}

void shutdown(const std::string& msg) {
	std::cout << "[ERROR] " << msg << std::endl << "Press any key to continue . . . ";
	_getch();
	glfwTerminate();
	exit(EXIT_FAILURE);
}

void update() {
	if (gGameOver) {
		return;
	}

	// Get player map row and column
	int playerX = static_cast<int>(gPlayer->position.x + 0.5f);
	int playerY = static_cast<int>(gPlayer->position.z + 0.5f);
	int playerLastX = static_cast<int>(gPlayer->lastPosition.x + 0.5f);
	int playerLastY = static_cast<int>(gPlayer->lastPosition.z + 0.5f);

	// FAST ENEMY
	int enemyFastX = static_cast<int>(gEnemyFast->position.x + 0.5f);
	int enemyFastY = static_cast<int>(gEnemyFast->position.z + 0.5f);

	// Get the path to follow if the player is in plain sight head there or have a 20% chance of picking a random location to go to
	bool straightToPlayer = true;
	std::vector<std::pair<int, int>> tempPath = gPathFinder->getPath(enemyFastX, enemyFastY, playerX, playerY);

	if (!tempPath.empty()) {
		for (std::vector<std::pair<int, int>>::iterator pathIT = tempPath.begin(); pathIT != tempPath.end(); ++pathIT) {
			if (*pathIT != tempPath.front()) {
				straightToPlayer = false;
				break;
			}
		}

		if (straightToPlayer) {
			gEnemyFastPath = tempPath;
			gWandering = false;
		}
		else if (!gWandering || gEnemyFastPath.empty()) {
			glm::vec3 wanderTo = gWalkway[randi(0, gWalkway.size() - 1)];
			gEnemyFastPath = gPathFinder->getPath(enemyFastX, enemyFastY, static_cast<int>(wanderTo.x), static_cast<int>(wanderTo.z));
			gWandering = true;
		}
	}

	// Move the enemy along the current path based on if the player is in plain sight or wandering to a random position
	if (!gEnemyFastPath.empty() && (!gWandering && gEnemyFastTimer->GetElapsedSeconds() > .25f || gWandering && gEnemyFastTimer->GetElapsedSeconds() > .5f)) {
		enemyFastX += gEnemyFastPath.back().first;
		enemyFastY += gEnemyFastPath.back().second;
		gEnemyFast->position.x = static_cast<float>(enemyFastX);
		gEnemyFast->position.z = static_cast<float>(enemyFastY);
		gEnemyFastPath.pop_back();
		gEnemyFastTimer->Reset();
	}

	// SLOW ENEMY
	int enemySlowX = static_cast<int>(gEnemySlow->position.x + 0.5f);
	int enemySlowY = static_cast<int>(gEnemySlow->position.z + 0.5f);

	// Get the path to follow based on if the player has moved
	if (playerX != playerLastX || playerY != playerLastY) {
		gEnemySlowPath = gPathFinder->getPath(static_cast<int>(gEnemySlow->position.x + 0.5f), static_cast<int>(gEnemySlow->position.z + 0.5f), static_cast<int>(gPlayer->position.x + 0.5f), static_cast<int>(gPlayer->position.z + 0.5f));
	}

	// Move the enemy along the current path
	if (!gEnemySlowPath.empty() && gEnemySlowTimer->GetElapsedSeconds() > .5f) {
		enemySlowX += gEnemySlowPath.back().first;
		enemySlowY += gEnemySlowPath.back().second;
		gEnemySlow->position.x = static_cast<float>(enemySlowX);
		gEnemySlow->position.z = static_cast<float>(enemySlowY);
		gEnemySlowPath.pop_back();
		gEnemySlowTimer->Reset();
	}

	// Camera Movement
	if (glfwGetKey(gWindow, GLFW_KEY_ENTER) == GLFW_PRESS) {
		gCamera->offset.y = 0.f;
	}

	if (glfwGetKey(gWindow, GLFW_KEY_1) == GLFW_PRESS) {
		gCamera->offset.y -= gCamera->movementSpeed;
		if (gCamera->offset.y < 0.f) {
			gCamera->offset.y = 0.f;
			gCamera->fieldOfView = 1.5f;
		}
	}
	if (glfwGetKey(gWindow, GLFW_KEY_2) == GLFW_PRESS) {
		gCamera->offset.y += gCamera->movementSpeed;
		gCamera->fieldOfView = 1.f;
	}
	gPlayer->lastPosition = gPlayer->position;

	// Player Movement
	if (glfwGetKey(gWindow, GLFW_KEY_UP) == GLFW_PRESS) {
		gPlayer->position -= gPlayer->orientation * glm::vec3(0.f, 0.f, gPlayer->movementSpeed);
	}
	if (glfwGetKey(gWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
		gPlayer->position += gPlayer->orientation * glm::vec3(0.f, 0.f, gPlayer->movementSpeed);
	}
	if (glfwGetKey(gWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		gLeft = true;
	}
	if (glfwGetKey(gWindow, GLFW_KEY_RIGHT) == GLFW_RELEASE && gLeft) {
		gPlayer->orientation *= glm::quat(glm::vec3(0.f, glm::radians(-90.f), 0.f));
		gLeft = false;
	}
	if (glfwGetKey(gWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
		gRight = true;
	}
	if (glfwGetKey(gWindow, GLFW_KEY_LEFT) == GLFW_RELEASE && gRight) {
		gPlayer->orientation *= glm::quat(glm::vec3(0.f, glm::radians(90.f), 0.f));
		gRight = false;
	}

	playerX = static_cast<int>(gPlayer->position.x + 0.5f);
	playerY = static_cast<int>(gPlayer->position.z + 0.5f);

	int playerIndex = (playerX * gMazeWidth) + playerY;
	int enemyFastIndex = (enemyFastX * gMazeWidth) + enemyFastY;
	int enemySlowIndex = (enemySlowX * gMazeWidth) + enemySlowY;

	bool won = false;
	bool lost = false;
	bool start = false;
	bool fell = playerX < 0 || playerX > gMazeWidth - 1 || playerY < 0 || playerY > gMazeHeight - 1;
		
	if (!fell) {
		won = gModels[playerIndex]->colour == gColourRed;
		lost = playerIndex == enemyFastIndex || playerIndex == enemySlowIndex;
		start = gModels[playerIndex]->colour == gColourGreen;
	}

	if ((won || lost || fell) && !start) {
		gGameOver = true;

		std::string bmpFile;

		if (won) {
			bmpFile = "youWin.bmp";
		}
		else if (lost) {
			bmpFile = "youLose.bmp";
		}
		else {
			bmpFile = "youFell.bmp";
		}

		std::vector<S24BitBMPPixel> message = gTextureManager->loadMaze(bmpFile);

		int pixelCount = 0;
		gMazeWidth = gMazeHeight = static_cast<int>(sqrt(message.size()));

		for (std::vector<CModel*>::iterator modelIT = gModels.begin(); modelIT != gModels.end(); ++modelIT) {
			if (*modelIT != gPlayer && *modelIT != gEnemyFast && *modelIT != gEnemySlow) {
				delete *modelIT;
			}
		}

		gModels.clear();

		for (int col = 0; col < gMazeWidth; ++col) {
			for (int row = 0; row < gMazeHeight; ++row, ++pixelCount) {
				CModel* mazeSquare = new CModel();
				mazeSquare->mesh = "box.obj";
				mazeSquare->position = glm::vec3(col, 0.f, row);
				mazeSquare->colour = glm::vec4(message[pixelCount].redPixel / 255.f, message[pixelCount].greenPixel / 255.f, message[pixelCount].bluePixel / 255.f, 1.f);

				if (mazeSquare->colour == gColourBlack) {
					delete mazeSquare;
				}
				else {
					gModels.push_back(mazeSquare);
				}
			}
		}

		gPlayer->isVisible = false;
		gEnemyFast->isVisible = false;
		gEnemySlow->isVisible = false;

		gModels.push_back(gPlayer);
		gModels.push_back(gEnemyFast);
		gModels.push_back(gEnemySlow);

		gCamera->offset = glm::vec3(0.1f, 50.f, 16.f);
		gCamera->fieldOfView = 1.f;
		gPlayer->position = glm::vec3(16.f, 0.f, 1.f);
		gPlayer->offset = glm::vec3(0.1f, 0.f, 15.f);
		gPlayer->orientation = glm::quat(glm::vec3(0.f, 0.f, 0.f));
	}
	else if (gModels[playerIndex]->colour > gColourDarkGrey && gModels[playerIndex]->colour < gColourLightGrey) {
		if (gCheatCode && gPlayerTimer->GetElapsedSeconds() < 3.f) {
			gPlayer->position.y += 1.f;
			gCheatCode = false;
			gOnWalls = true;
		}
		else if (!gOnWalls) {
			gPlayer->position = gPlayer->lastPosition;
		}
	}
	else if (gModels[playerIndex]->colour == gColourWhite && gOnWalls) {
		gOnWalls = false;
		gPlayer->position.y -= 1.f;
	}
}