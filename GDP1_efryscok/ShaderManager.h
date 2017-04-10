#pragma once

class CModel;

class CShaderManager {
private:
	GLuint mProgramID;
	GLuint mUniformID_ModelMatrix;
	GLuint mUniformID_ModelColour;
	GLuint mUniformID_ViewMatrix;
	GLuint mUniformID_ProjectionMatrix;
	GLuint mUniformID_EyePosition;
	GLuint mUniformID_TexSampler;
	GLuint mUniformID_IsCubeMap;
	GLuint mUniformID_CubeMapSampler;
	GLuint mUniformID_HasTexture;
	std::string mRootDir;

public:
	CShaderManager(std::string rootDir) : mRootDir(rootDir) {}
	~CShaderManager() {}

	void getUniformLocations();
	bool loadShaders(std::string vertexFile, std::string fragmentFile);
	void update(CModel* model, GLuint textureID);
};

