#pragma once

#include <cstdlib>
#include <crtdbg.h>
#include <conio.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <queue>
#include <stack>

#include <glad\glad.h>
#include <glfw\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "MeshManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "PathFinder.h"
#include "Camera.h"
#include "Model.h"
#include "CHRTimer.h"

#define _CRTDBG_MAP_ALLOC

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DBG_NEW
#endif // DBG_NEW
#endif // _DEBUG

extern bool gCheating;
extern bool gWandering;
extern bool gGameOver;
extern bool gLeft;
extern bool gRight;
extern bool gCheatCode;
extern bool gOnWalls;
extern int gMazeWidth;
extern int gMazeHeight;
extern int gCheatStep;
extern float gDeltaTime;

extern GLFWwindow* gWindow;
extern CMeshManager* gMeshManager;
extern CShaderManager* gShaderManager;
extern CTextureManager* gTextureManager;
extern CPathFinder* gPathFinder;
extern CCamera* gCamera;
extern CModel* gPlayer;
extern CModel* gEnemyFast;
extern CModel* gEnemySlow;
extern CHRTimer* gPlayerTimer;
extern CHRTimer* gEnemyFastTimer;
extern CHRTimer* gEnemySlowTimer;

extern glm::vec4 gColourWhite;
extern glm::vec4 gColourRed;
extern glm::vec4 gColourGreen;
extern glm::vec4 gColourBlue;
extern glm::vec4 gColourDarkGrey;
extern glm::vec4 gColourLightGrey;
extern glm::vec4 gColourBlack;

extern std::vector<glm::vec3> gWalkway;
extern std::vector<glm::vec3> gGoal;
extern std::vector<glm::vec3> gStart;
extern std::vector<CModel*> gModels;
extern std::vector<std::pair<int, int>> gEnemyFastPath;
extern std::vector<std::pair<int, int>> gEnemySlowPath;

extern bool operator < (const glm::vec4 & a, const glm::vec4 & b);
extern bool operator > (const glm::vec4 & a, const glm::vec4 & b);