#include "global.h"

bool CMeshManager::loadModel(std::string modelFile) {
	Assimp::Importer importer;
	unsigned int flags = 0;
	const aiScene* scene = importer.ReadFile(mRootDir + modelFile, flags);

	if (!scene) {
		std::cout << "[ERROR] " << importer.GetErrorString() << std::endl;
		return false;
	}

	const aiMesh* mesh = scene->mMeshes[0];

	mVertices.clear();
	mIndices.clear();

	unsigned int vertexOffset = mVertices.size();
	unsigned int indexOffset = mIndices.size();
	unsigned int numVertices = mesh->mNumVertices;
	unsigned int numIndices = 3 * mesh->mNumFaces;

	mMeshInfo[modelFile].baseIndex = indexOffset;
	mMeshInfo[modelFile].baseVertex = vertexOffset;
	mMeshInfo[modelFile].numIndices = numIndices;

	// Fill vertices positions
	mVertices.resize(vertexOffset + numVertices);
	
	float minX, minY, minZ, maxX, maxY, maxZ, extentX, extentY, extentZ, maxExtent;

	// Set the initial values to the 1st vertex
	minX = maxX = mesh->mVertices[0].x;
	minY = maxY = mesh->mVertices[0].y;
	minZ = maxZ = mesh->mVertices[0].z;

	for (int index = 0; index != mesh->mNumVertices; index++) {
		if (mesh->mVertices[index].x < minX) { minX = mesh->mVertices[index].x; }
		if (mesh->mVertices[index].y < minY) { minY = mesh->mVertices[index].y; }
		if (mesh->mVertices[index].z < minZ) { minZ = mesh->mVertices[index].z; }

		if (mesh->mVertices[index].x > maxX) { maxX = mesh->mVertices[index].x; }
		if (mesh->mVertices[index].y > maxY) { maxY = mesh->mVertices[index].y; }
		if (mesh->mVertices[index].z > maxZ) { maxZ = mesh->mVertices[index].z; }
	}

	// What the max extent
	extentX = maxX - minX;
	extentY = maxY - minY;
	extentZ = maxZ - minZ;

	maxExtent = extentX;
	if (extentY > maxExtent) { maxExtent = extentY; }
	if (extentZ > maxExtent) { maxExtent = extentZ; }

	float scaleValue = 1.f / maxExtent;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		aiVector3D pos = mesh->mVertices[i];
		aiVector3D tc = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : aiVector3D(0.f);
		aiVector3D n = mesh->mNormals[i];

		SMeshVertex& vert = mVertices[vertexOffset + i];
		vert.position = glm::vec4(pos.x * scaleValue, pos.y * scaleValue, pos.z * scaleValue, 1.f);
		vert.texCoord = glm::vec4(tc.x, tc.y, tc.z, 0.f);
		vert.normal = glm::vec4(n.x, n.y, n.z, 1.f);
	}

	// Fill face indices
	mIndices.resize(indexOffset + numIndices);
	
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		unsigned int baseInd = 3 * i;

		// Assume the model has only triangles
		mIndices[indexOffset + baseInd] = mesh->mFaces[i].mIndices[0];
		mIndices[indexOffset + baseInd + 1] = mesh->mFaces[i].mIndices[1];
		mIndices[indexOffset + baseInd + 2] = mesh->mFaces[i].mIndices[2];
	}

	// Create the buffers
	glGenVertexArrays(1, &mMeshInfo[modelFile].vertexArrayID);
	glBindVertexArray(mMeshInfo[modelFile].vertexArrayID);

	glGenBuffers(1, &mTexCoordBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, mTexCoordBufferID);

	glGenBuffers(1, &mNormalBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, mNormalBufferID);

	int bytesInVertexArray = mVertices.size() * sizeof(SMeshVertex);
	glBufferData(GL_ARRAY_BUFFER, bytesInVertexArray, &mVertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &mIndexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferID);

	int bytesInIndexArray = mIndices.size() * sizeof(GLuint);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesInIndexArray, &mIndices[0], GL_STATIC_DRAW);

	// Bind the buffers
	glBindVertexArray(mMeshInfo[modelFile].vertexArrayID);
	glBindBuffer(GL_ARRAY_BUFFER, mTexCoordBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, mNormalBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferID);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(SMeshVertex), (GLvoid*)0);

	int normalOffset = sizeof(((SMeshVertex*)0)->position);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(SMeshVertex), (GLvoid*)normalOffset);

	int texCoordOffset = normalOffset + sizeof(((SMeshVertex*)0)->normal);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SMeshVertex), (GLvoid*)texCoordOffset);

	// The "scene" pointer will be deleted automatically by "importer"
	return true;
}

void CMeshManager::update() {
	for (std::vector<CModel*>::iterator modelIT = gModels.begin(); modelIT != gModels.end(); ++modelIT) {
		if ((*modelIT)->isVisible) {
			(*modelIT)->modelMatrix = glm::translate(glm::mat4(1.f), (*modelIT)->position) * glm::mat4((*modelIT)->orientation);
			(*modelIT)->modelMatrix *= glm::mat4((*modelIT)->orientation);
			(*modelIT)->modelMatrix = glm::scale((*modelIT)->modelMatrix, glm::vec3((*modelIT)->scale, (*modelIT)->scale, (*modelIT)->scale));

			gShaderManager->update(*modelIT, gTextureManager->update(*modelIT));

			if ((*modelIT)->isCubeMap) {
				glDisable(GL_DEPTH);
				glDepthMask(GL_FALSE);
			}

			glBindVertexArray(mMeshInfo[(*modelIT)->mesh].vertexArrayID);
			glDrawElements(GL_TRIANGLES, mMeshInfo[(*modelIT)->mesh].numIndices, GL_UNSIGNED_INT, (GLvoid*)0);

			if ((*modelIT)->isCubeMap) {
				glDepthMask(GL_TRUE);
				glEnable(GL_DEPTH);
			}
		}
	}
}
