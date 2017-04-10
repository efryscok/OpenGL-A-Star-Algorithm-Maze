#include "global.h"


bool CTextureManager::loadCubeMap(std::string name, std::string right, std::string left, std::string top, std::string bottom, std::string front, std::string back) {
	glGenTextures(1, &textures[name]);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textures[name]);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE /*GL_REPEAT*/);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE /*GL_REPEAT*/);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE /*GL_REPEAT*/);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glTexStorage2D(GL_TEXTURE_CUBE_MAP, 10, GL_RGBA8, mNumberOfColumns, mNumberOfRows);

	if (loadBMP(right)) {
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, mNumberOfColumns, mNumberOfRows, GL_RGB, GL_UNSIGNED_BYTE, mImages);
		clearBMP();	
	}
	else {
		return false;
	}

	if (loadBMP(left)) {
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0, 0, mNumberOfColumns, mNumberOfRows, GL_RGB, GL_UNSIGNED_BYTE, mImages);
		clearBMP();
	}
	else {
		return false;
	}

	if (loadBMP(top)) {
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0, 0, mNumberOfColumns, mNumberOfRows, GL_RGB, GL_UNSIGNED_BYTE, mImages);
		clearBMP();
	}
	else {
		return false;
	}

	if (loadBMP(bottom)) {
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 0, 0, mNumberOfColumns, mNumberOfRows, GL_RGB, GL_UNSIGNED_BYTE, mImages);
		clearBMP();
	}
	else {
		return false;
	}

	if (loadBMP(front)) {
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0, 0, mNumberOfColumns, mNumberOfRows, GL_RGB, GL_UNSIGNED_BYTE, mImages);
		clearBMP();
	}
	else {
		return false;
	}

	if (loadBMP(back)) {
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0, 0, mNumberOfColumns, mNumberOfRows, GL_RGB, GL_UNSIGNED_BYTE, mImages);
		clearBMP();
	}
	else {
		return false;
	}

	return true;
}

bool CTextureManager::loadTexture(std::string textureFile) {
	glGenTextures(1, &textures[textureFile]);

	if (loadBMP(textureFile)) {
		glBindTexture(GL_TEXTURE_2D, textures[textureFile]);
		glTexStorage2D(GL_TEXTURE_2D, textures[textureFile], GL_RGBA8, mNumberOfColumns, mNumberOfRows);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mNumberOfColumns, mNumberOfRows, GL_RGB, GL_UNSIGNED_BYTE, mImages);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		clearBMP();
		return true;
	}

	return false;
}

std::vector<S24BitBMPPixel> CTextureManager::loadMaze(std::string mazeFile) {
	if (loadBMP(mazeFile)) {
		std::vector<S24BitBMPPixel> returnVec;

		for (unsigned long i = 0; i < mPixelCount; ++i) {
			returnVec.push_back(mImages[i]);
		}

		return returnVec;
	}

	return std::vector<S24BitBMPPixel>();
}

GLuint CTextureManager::update(CModel* model) {
	if (model->isCubeMap) {
		glActiveTexture(80 + GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[model->texture]);
		return textures[model->texture];
	}
	else {
		if (model->texture == "N/A") {
			return -1;
		}
		else {
			glActiveTexture(textures[model->texture] + GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[model->texture]);
			return textures[model->texture];
		}
	}

	return -1;
}

unsigned short CTextureManager::bitshiftAddToUShort(unsigned short uShort, char byte, int shift) {
	if (shift > 1) {
		shift = 0;
	}

	shift *= 8;

	return uShort | (static_cast<unsigned long>(byte) << shift);
}

unsigned long CTextureManager::bitshiftAddToULong(unsigned long uLong, char byte, int shift) {
	if (shift > 3) {
		shift = 0;
	}

	shift *= 8;
	
	return uLong | (static_cast<unsigned long>(byte) << shift);
}

bool CTextureManager::clearBMP() {
	delete[] mImages;
	mImages = nullptr;
	return true;
}

bool CTextureManager::loadBMP(std::string bmpFile) {
	std::ifstream textureStream(mRootDir + bmpFile, std::ios::binary);

	if (textureStream.is_open()) {
		// Scan to the end to get the size
		textureStream.seekg(0, std::ios::end);
		std::ios::pos_type fileSize = textureStream.tellg();

		// Return to start
		textureStream.seekg(0, std::ios::beg);

		char* rawData = new char[static_cast<unsigned int>(fileSize)];

		textureStream.read(rawData, fileSize);
		textureStream.close();

		// Now go through and decode the file
		unsigned long index = 0;
		char letter1 = readNextChar(rawData, index);
		char letter2 = readNextChar(rawData, index);

		if (letter1 != 'B' && letter2 != 'M') {
			return false;
		}

		// File is BMP
		mFileSize = readNextULong(rawData, index);
		mReserved1 = readNextUShort(rawData, index);
		mReserved2 = readNextUShort(rawData, index);
		mOffsetInBits = readNextULong(rawData, index);
		mHeaderSize = readNextULong(rawData, index);
		mNumberOfRows = readNextULong(rawData, index);
		mHeight = mOriginalHeight = mNumberOfRows;
		mNumberOfColumns = readNextULong(rawData, index);
		mWidth = mOriginalWidth = mNumberOfColumns;
		mNumberOfPlanes = readNextUShort(rawData, index);
		mBitPerPixel = readNextUShort(rawData, index);

		if (mBitPerPixel != 24) {
			delete[] rawData;
			return false;
		}

		mCompressionMode = readNextULong(rawData, index);
		mImageSizeInBytes = readNextULong(rawData, index);
		mPixelsPerMeterX = readNextULong(rawData, index);
		mPixelsPerMeterY = readNextULong(rawData, index);
		mNumberOfLookUpTableEntries = readNextULong(rawData, index);
		mNumberOfImportantColours = readNextULong(rawData, index);

		long bytesPerRow = ((3 * mNumberOfRows + 3) / 4) * 4;
		long numberOfPaddingBytes = bytesPerRow - 3 * mNumberOfColumns;

		// Allocate enough space
		if (mImages != nullptr) {
			clearBMP();
		}
		mImages = new S24BitBMPPixel[mNumberOfRows * mNumberOfColumns];

		// Ran out of memory
		if (!mImages) {
			delete[] rawData;
			return false;
		}

		// Read the bitmap into memory
		unsigned long bytesRead = 0;
		unsigned long totalBytesInFile = mNumberOfRows * mNumberOfColumns;
		unsigned long pixelCount = 0;

		for (unsigned long row = 0; row < mNumberOfRows; ++row) {
			for (unsigned long col = 0; col < mNumberOfColumns; ++col) {
				char gColourBlue = readNextChar(rawData, index);
				char gColourGreen = readNextChar(rawData, index);
				char gColourRed = readNextChar(rawData, index);

				mImages[pixelCount].redPixel = static_cast<unsigned char>(gColourRed);
				mImages[pixelCount].greenPixel = static_cast<unsigned char>(gColourGreen);
				mImages[pixelCount].bluePixel = static_cast<unsigned char>(gColourBlue);

				++pixelCount;
				bytesRead += 3;
			}

			for (int discardedBytes = 0; discardedBytes < numberOfPaddingBytes; ++discardedBytes) {
				char tempChar = readNextChar(rawData, index);
			}
		}

		mPixelCount = pixelCount;
		delete[] rawData;
		return true;
	}

	return false;
}

char CTextureManager::readNextChar(char* data, unsigned long& index) {
	return static_cast<unsigned char>(data[index++]);
}

unsigned short CTextureManager::readNextUShort(char* data, unsigned long& index) {
	unsigned short value = 0;
	value = bitshiftAddToUShort(value, static_cast<unsigned char>(data[index++]), 0);
	value = bitshiftAddToUShort(value, static_cast<unsigned char>(data[index++]), 1);
	return value;
}

unsigned long CTextureManager::readNextULong(char* data, unsigned long& index) {
	unsigned long value = 0;
	value = bitshiftAddToULong(value, static_cast<unsigned char>(data[index++]), 0);
	value = bitshiftAddToULong(value, static_cast<unsigned char>(data[index++]), 1);
	value = bitshiftAddToULong(value, static_cast<unsigned char>(data[index++]), 2);
	value = bitshiftAddToULong(value, static_cast<unsigned char>(data[index++]), 3);
	return value;
}
