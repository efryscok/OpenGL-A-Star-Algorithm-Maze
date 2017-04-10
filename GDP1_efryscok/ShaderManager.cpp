#include "global.h"

void CShaderManager::getUniformLocations() {
	mUniformID_ModelMatrix = glGetUniformLocation(mProgramID, "ModelMatrix");
	mUniformID_ModelColour = glGetUniformLocation(mProgramID, "ModelColour");
	mUniformID_ViewMatrix = glGetUniformLocation(mProgramID, "ViewMatrix");
	mUniformID_ProjectionMatrix = glGetUniformLocation(mProgramID, "ProjectionMatrix");
	mUniformID_EyePosition = glGetUniformLocation(mProgramID, "EyePosition");
	mUniformID_TexSampler = glGetUniformLocation(mProgramID, "texSampler");
	mUniformID_IsCubeMap = glGetUniformLocation(mProgramID, "isCubeMap");
	mUniformID_CubeMapSampler = glGetUniformLocation(mProgramID, "cubeMapSampler");
	mUniformID_HasTexture = glGetUniformLocation(mProgramID, "hasTexture");
}

bool CShaderManager::loadShaders(std::string vertexFile, std::string fragmentFile) {
	// Create the shaders
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the vertex shader code from the file
	std::string vertexShaderCode;
	std::ifstream vertexShaderStream(mRootDir + vertexFile, std::ios::in);

	if (vertexShaderStream.is_open()) {
		std::string line = "";
		
		while (getline(vertexShaderStream, line)) {
			vertexShaderCode += "\n" + line;
		}

		vertexShaderStream.close();
	}
	else {
		std::cout << "[ERROR] Could not open the vertex shader file " << mRootDir << vertexFile << std::endl;
		return 0;
	}

	// Read the fragment shader code from the file
	std::string fragmentShaderCode;
	std::ifstream fragmentShaderStream(mRootDir + fragmentFile, std::ios::in);

	if (fragmentShaderStream.is_open()) {
		std::string line = "";

		while (std::getline(fragmentShaderStream, line)) {
			fragmentShaderCode += "\n" + line;
		}

		fragmentShaderStream.close();
	}
	else {
		std::cout << "[ERROR] Could not open the fragment shader file " << mRootDir << fragmentFile << std::endl;
	}

	GLint result = GL_FALSE;
	int infoLogLength;

	// Compile vertex shader
	std::cout << "[INFO] Compiling vertex shader " << mRootDir << vertexFile << std::endl;
	const char* vertexSourcePointer = vertexShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, &vertexSourcePointer, NULL);
	glCompileShader(vertexShaderID);

	// Check vertex shader
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength > 0) {
		std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
		std::cout << &vertexShaderErrorMessage[0] << std::endl;
	}

	// Compile fragment shader
	std::cout << "[INFO] Compiling fragment shader " << mRootDir << fragmentFile << std::endl;
	const char* fragmentSourcePointer = fragmentShaderCode.c_str();
	glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer, NULL);
	glCompileShader(fragmentShaderID);

	// Check fragment shader
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength > 0) {
		std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
		std::cout << &fragmentShaderErrorMessage[0] << std::endl;
	}

	// Link the program
	std::cout << "[INFO] Linking shader program" << std::endl;
	mProgramID = glCreateProgram();

	glAttachShader(mProgramID, vertexShaderID);
	glAttachShader(mProgramID, fragmentShaderID);
	glLinkProgram(mProgramID);

	// Check the program
	glGetProgramiv(mProgramID, GL_LINK_STATUS, &result);
	glGetProgramiv(mProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength > 0) {
		std::vector<char> programErrorMessage(infoLogLength + 1);
		glGetProgramInfoLog(mProgramID, infoLogLength, NULL, &programErrorMessage[0]);
		std::cout << &programErrorMessage[0] << std::endl;
	}

	glDetachShader(mProgramID, vertexShaderID);
	glDetachShader(mProgramID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(mProgramID);

	return true;
}

void CShaderManager::update(CModel* model, GLuint textureID) {
	glUniformMatrix4fv(mUniformID_ProjectionMatrix, 1, GL_FALSE, (const GLfloat*)glm::value_ptr(gCamera->getProjection()));
	glUniformMatrix4fv(mUniformID_ViewMatrix, 1, GL_FALSE, (const GLfloat*)glm::value_ptr(gCamera->getView()));
	glUniform4fv(mUniformID_EyePosition, 1, (const GLfloat*)glm::value_ptr(gCamera->position));

	if (model->isCubeMap) {
		glUniform1i(mUniformID_IsCubeMap, TRUE);
		glUniform1i(mUniformID_CubeMapSampler, textureID);
		glUniformMatrix4fv(mUniformID_ModelMatrix, 1, GL_FALSE, (const GLfloat*)glm::value_ptr(model->modelMatrix));
		glUniform4fv(mUniformID_ModelColour, 1, (const GLfloat*)glm::value_ptr(model->colour));
	}
	else {
		glUniform1i(mUniformID_IsCubeMap, FALSE);
		glUniformMatrix4fv(mUniformID_ModelMatrix, 1, GL_FALSE, (const GLfloat*)glm::value_ptr(model->modelMatrix));
		glUniform4fv(mUniformID_ModelColour, 1, (const GLfloat*)glm::value_ptr(model->colour));
		glUniform1i(mUniformID_TexSampler, textureID);
	}

	if (model->texture == "N/A" || textureID == -1) {
		glUniform1i(mUniformID_HasTexture, FALSE);
	}
	else {
		glUniform1i(mUniformID_HasTexture, TRUE);
	}
}
