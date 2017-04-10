#include "global.h"

bool gCheating = false;
bool gWandering = false;
bool gGameOver = false;
bool gLeft = false;
bool gRight = false;
bool gCheatCode = false;
bool gOnWalls = false;
int gMazeWidth = 0;
int gMazeHeight = 0;
int gCheatStep = 0;
float gDeltaTime = 0.f;

GLFWwindow* gWindow = nullptr;
CMeshManager* gMeshManager = nullptr;
CShaderManager* gShaderManager = nullptr;
CTextureManager* gTextureManager = nullptr;
CPathFinder* gPathFinder = nullptr;
CCamera* gCamera = nullptr;
CModel* gPlayer = nullptr;
CModel* gEnemyFast = nullptr;
CModel* gEnemySlow = nullptr;
CHRTimer* gPlayerTimer = nullptr;
CHRTimer* gEnemyFastTimer = nullptr;
CHRTimer* gEnemySlowTimer = nullptr;

glm::vec4 gColourWhite(1.f, 1.f, 1.f, 1.f);
glm::vec4 gColourRed(1.f, 0.f, 0.f, 1.f);
glm::vec4 gColourGreen(0.f, 1.f, 0.f, 1.f);
glm::vec4 gColourBlue(0.f, 0.f, 1.f, 1.f);
glm::vec4 gColourDarkGrey(.45f, .45f, .45f, 1.f);
glm::vec4 gColourLightGrey(.55f, .55f, .55f, 1.f);
glm::vec4 gColourBlack(0.f, 0.f, 0.f, 1.f);

std::vector<glm::vec3> gWalkway;
std::vector<glm::vec3> gGoal;
std::vector<glm::vec3> gStart;
std::vector<CModel*> gModels;
std::vector<std::pair<int, int>> gEnemyFastPath;
std::vector<std::pair<int, int>> gEnemySlowPath;

bool operator < (const glm::vec4 & a, const glm::vec4 & b) {
	return a[0] < b[0] && a[1] < b[1] && a[2] < b[2];
}

bool operator > (const glm::vec4 & a, const glm::vec4 & b) {
	return a[0] > b[0] && a[1] > b[1] && a[2] > b[2];
}
