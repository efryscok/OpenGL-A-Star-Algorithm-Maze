#pragma once

enum EHeuristic {
	MANHATTAN,
	EUCLIDIAN,
	OCTILE,
	CHEBYSHEV
};

class CPathNode {
private:
	bool mDiagonal;
	int mX;
	int mY;
	int mLevel;
	int mPriority;
	EHeuristic mHeuristic;

public:
	CPathNode(int x, int y, int level, int priority, EHeuristic heuristic, bool diagonal = false) : 
		mX(x), mY(y), mLevel(level), mPriority(priority), mHeuristic(heuristic), mDiagonal(diagonal) 
	{}

	int getX() const;
	int getY() const;
	int getLevel() const;
	int getPriority() const;

	void updatePriority(const int & x, const int & y);
	void nextLevel(const int & i);
	int estimate(const int & x, const int & y);
};

class CPathFinder {
private:
	int mWidth;
	int mHeight;
	int mDirections;
	EHeuristic mHeuristic;

	std::vector<int> mDirectionsX;
	std::vector<int> mDirectionsY;
	std::vector<std::vector<int>> mMap;
	std::vector<std::vector<int>> mClosedSet;
	std::vector<std::vector<int>> mOpenSet;
	std::vector<std::vector<int>> mDirMap;

public:
	CPathFinder(std::vector<std::vector<int>> map, EHeuristic heuristic = MANHATTAN, bool diagonal = false);

	int getDirX(const char & c);
	int getDirY(const char & c);

	std::vector<std::pair<int, int>> getPath(const int & startX, const int & startY, const int & goalX, const int & goalY);
};