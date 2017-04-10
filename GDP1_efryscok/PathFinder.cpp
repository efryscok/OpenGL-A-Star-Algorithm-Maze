#include "global.h"

int CPathNode::getX() const {
	return mX;
}

int CPathNode::getY() const {
	return mY;
}

int CPathNode::getLevel() const {
	return mLevel;
}

int CPathNode::getPriority() const {
	return mPriority;
}

void CPathNode::updatePriority(const int & x, const int & y) {
	mPriority = mLevel + estimate(x, y) * 10;
}

void CPathNode::nextLevel(const int & i) {
	mLevel += mDiagonal ? (i % 2 == 0 ? 10 : 14) : 10;
}

int CPathNode::estimate(const int & x, const int & y) {
	int dx = x - mX;
	int dy = y - mY;
	double f = sqrt(2) - 1;

	switch (mHeuristic) {
	case MANHATTAN:
		return dx + dy;
	case EUCLIDIAN:
		return static_cast<int>(sqrt((dx * dx) + (dy * dy)));
	case OCTILE:
		return static_cast<int>((dx < dy) ? f * (dx + dy) : f * (dy + dx));
	case CHEBYSHEV:
		return std::max(dx, dy);
	default:
		return dx + dy;
		break;
	}
}

bool operator < (const CPathNode & left, const CPathNode & right) {
	return left.getPriority() > right.getPriority();
}

CPathFinder::CPathFinder(std::vector<std::vector<int>> map, EHeuristic heuristic, bool diagonal) {
	mWidth = mHeight = map.size();
	mHeuristic = heuristic;
	mMap = map;
	mClosedSet = mOpenSet = mDirMap = std::vector<std::vector<int>>(mWidth, std::vector<int>(mHeight, 0));

	if (diagonal) {
		mDirections = 8;
		mDirectionsX = { 1, 1, 0, -1, -1, -1, 0, 1 };
		mDirectionsY = { 0, 1, 1, 1, 0, -1, -1, -1 };
	}
	else {
		mDirections = 4;
		mDirectionsX = { 1, 0, -1, 0 };
		mDirectionsY = { 0, 1, 0, -1 };
	}
}

int CPathFinder::getDirX(const char & c) {
	return mDirectionsX[atoi(&c)];
}

int CPathFinder::getDirY(const char & c) {
	return mDirectionsY[atoi(&c)];
}

std::vector<std::pair<int, int>> CPathFinder::getPath(const int & startX, const int & startY, const int & goalX, const int & goalY) {
	std::priority_queue<CPathNode> pq[2]; // list of open (not-yet-tried) nodes
	int pqi; // pq index
	CPathNode* n0;
	CPathNode* m0;
	int i , j, x, y, xdx, ydy;
	i = j = x = y = xdx = ydy = pqi = 0;

	// reset the node maps
	for (y = 0; y < mWidth; ++y) {
		for (x = 0; x < mHeight; ++x) {
			mClosedSet[x][y] = 0;
			mOpenSet[x][y] = 0;
		}
	}

	// create the start node and push into list of open nodes
	n0 = new CPathNode(startX, startY, 0, 0, mHeuristic);
	n0->updatePriority(goalX, goalY);
	pq[pqi].push(*n0);
	x = n0->getX(); y = n0->getY();
	mOpenSet[x][y] = n0->getPriority(); // mark it on the open nodes map
	delete n0;

											  // A* search
	while (!pq[pqi].empty()) {
		// get the current node w/ the highest priority
		// from the list of open nodes
		n0 = new CPathNode(pq[pqi].top().getX(), pq[pqi].top().getY(), pq[pqi].top().getLevel(), pq[pqi].top().getPriority(), mHeuristic);

		x = n0->getX(); y = n0->getY();

		pq[pqi].pop(); // remove the node from the open list
		mOpenSet[x][y] = 0;
		// mark it on the closed nodes map
		mClosedSet[x][y] = 1;

		// quit searching when the goal state is reached
		//if((*n0).estimate(xFinish, yFinish) == 0)
		if (x == goalX && y == goalY) {
			// generate the path from finish to start
			// by following the directions
			std::vector<std::pair<int, int>> path;
			while (!(x == startX && y == startY)) {
				j = mDirMap[x][y];
				int pathDir = (j + mDirections / 2) % mDirections;
				x += mDirectionsX[j];
				y += mDirectionsY[j];
				path.push_back(std::make_pair(mDirectionsX[pathDir], mDirectionsY[pathDir]));
			}

			// garbage collection
			delete n0;
			// empty the leftover nodes
			while (!pq[pqi].empty()) pq[pqi].pop();
			return path;
		}

		// generate moves (child nodes) in all possible directions
		for (i = 0; i < mDirections; ++i) {
			xdx = x + mDirectionsX[i]; ydy = y + mDirectionsY[i];

			if (!(xdx < 0 || xdx > mWidth - 1 || ydy < 0 || ydy > mHeight - 1 || mMap[xdx][ydy] == 1 || mClosedSet[xdx][ydy] == 1)) {
				// generate a child node
				m0 = new CPathNode(xdx, ydy, n0->getLevel(), n0->getPriority(), mHeuristic);
				m0->nextLevel(i);
				m0->updatePriority(goalX, goalY);

				// if it is not in the open list then add into that
				if (mOpenSet[xdx][ydy] == 0) {
					mOpenSet[xdx][ydy] = m0->getPriority();
					pq[pqi].push(*m0);
					delete m0;
					// mark its parent node direction
					mDirMap[xdx][ydy] = (i + mDirections / 2) % mDirections;
				}
				else if (mOpenSet[xdx][ydy] > m0->getPriority()) {
					// update the priority info
					mOpenSet[xdx][ydy] = m0->getPriority();
					// update the parent direction info
					mDirMap[xdx][ydy] = (i + mDirections / 2) % mDirections;

					// replace the node
					// by emptying one pq to the other one
					// except the node to be replaced will be ignored
					// and the new node will be pushed in instead
					while (!(pq[pqi].top().getX() == xdx &&
						pq[pqi].top().getY() == ydy)) {
						pq[1 - pqi].push(pq[pqi].top());
						pq[pqi].pop();
					}
					pq[pqi].pop(); // remove the wanted node

								   // empty the larger size pq to the smaller one
					if (pq[pqi].size() > pq[1 - pqi].size()) pqi = 1 - pqi;
					while (!pq[pqi].empty()) {
						pq[1 - pqi].push(pq[pqi].top());
						pq[pqi].pop();
					}
					pqi = 1 - pqi;
					pq[pqi].push(*m0); // add the better node instead
					delete m0;
				}
				else delete m0; // garbage collection
			}
		}
		delete n0; // garbage collection
	}
	return std::vector<std::pair<int, int>>(); // no route found
}
