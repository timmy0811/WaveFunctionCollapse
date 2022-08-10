#include "CollapseHandler.h"

void CollapseHandler::initTiles()
{
	std::string path = "Assets/";
	switch (set) {
	case TILESET::CAVE: {
		// Tiles setup
		tiles.push_back(cv::imread(path + "CAVE/2.png"));
		tiles.push_back(rotateImage(tiles[0], cv::ROTATE_90_CLOCKWISE));
		tiles.push_back(rotateImage(tiles[0], cv::ROTATE_180));
		tiles.push_back(rotateImage(tiles[0], cv::ROTATE_90_COUNTERCLOCKWISE));

		tiles.push_back(cv::imread(path + "CAVE/3.png"));
		tiles.push_back(rotateImage(tiles[4], cv::ROTATE_90_CLOCKWISE));
		tiles.push_back(rotateImage(tiles[4], cv::ROTATE_180));
		tiles.push_back(rotateImage(tiles[4], cv::ROTATE_90_COUNTERCLOCKWISE));

		tiles.push_back(cv::imread(path + "CAVE/4.png"));
		tiles.push_back(rotateImage(tiles[8], cv::ROTATE_90_CLOCKWISE));
		tiles.push_back(rotateImage(tiles[8], cv::ROTATE_180));
		tiles.push_back(rotateImage(tiles[8], cv::ROTATE_90_COUNTERCLOCKWISE));

		tiles.push_back(cv::imread(path + "CAVE/0.png"));
		tiles.push_back(cv::imread(path + "CAVE/1.png"));

		tileSetAmount = tiles.size();
		break;
	}
	case TILESET::CITY: {
		// Tiles setup
		tiles.push_back(cv::imread(path + "CITY/0.png"));
		tiles.push_back(cv::imread(path + "CITY/1.png"));
		tiles.push_back(cv::imread(path + "CITY/2.png"));
		tiles.push_back(cv::imread(path + "CITY/3.png"));
		tiles.push_back(cv::imread(path + "CITY/5.png"));
		tiles.push_back(cv::imread(path + "CITY/4.png"));
		tiles.push_back(cv::imread(path + "CITY/6.png"));
		tiles.push_back(cv::imread(path + "CITY/7.png"));

		tileSetAmount = tiles.size();
		break;
	}
	}

	for (int i = 0; i < tiles.size(); i++) {
		cv::resize(tiles[i], tiles[i], cv::Size(tile_width, tile_width), 0, 0, cv::InterpolationFlags::INTER_AREA);
	}
}

void CollapseHandler::initConnections()
{
	// { UP, RIGTH, DOWN, LEFT }

	switch (set) {
	case TILESET::CAVE: {
		faces[0] = { 0, 1, 1, 0 };
		faces[1] = { 0, 0, 2, 1 };
		faces[2] = { 2, 0, 0, 2 };
		faces[3] = { 1, 2, 0, 0 };
		faces[4] = { 1, 3, 1, 0 };
		faces[5] = { 0, 1, 3, 1 };
		faces[6] = { 2, 0, 2, 3 };
		faces[7] = { 3, 2, 0, 2 };
		faces[8] = { 3, 3, 1, 2 };
		faces[9] = { 1, 3, 3, 1 };
		faces[10] = { 2, 1, 3, 3 };
		faces[11] = { 3, 2, 2, 3 };
		faces[12] = { 0, 0, 0, 0 };
		faces[13] = { 3, 3, 3, 3 };

		break;
	}
	case TILESET::CITY: {
		faces[0] = { 0, 3, 1, 0 };
		faces[1] = { 0, 0, 2, 3 };
		faces[2] = { 1, 4, 1, 0 };
		faces[3] = { 2, 0, 2, 4 };
		faces[4] = { 1, 6, 8, 5 };
		faces[5] = { 2, 5, 8, 6 };
		faces[6] = { 0, 0, 0, 0 };
		faces[7] = { 0, 5, 8, 5 };
		
		break;
	}
	default: {
		return;
	}
	}
}

cv::Mat CollapseHandler::rotateImage(cv::Mat image, cv::RotateFlags flag)
{
	cv::Mat out{};
	cv::rotate(image, out, flag);
	return out;
}

void CollapseHandler::placeOnImage(cv::Mat canvas, cv::Mat image, uint16_t x, uint16_t y)
{
	cv::Mat snippet = (canvas)(cv::Rect(x, y, image.cols, image.rows));
	image.copyTo(snippet);
}

std::vector<int> CollapseHandler::getPotentials(uint16_t x, uint16_t y)
{
	if (indArr[y][x] != -1 || x < 0 || x >= grid_dim || y < 0 || y >= grid_dim) return std::vector<int>{};

	std::vector<std::vector<int>> cutList = { {}, {}, {}, {} };

	for (int i = 0; i < 4; i++) {
		int xz, yz;

		// check four sides
		if (i == 0) {
			xz = x;
			yz = y - 1;
		}
		else if (i == 1) {
			xz = x + 1;
			yz = y;
		}
		else if (i == 2) {
			xz = x;
			yz = y + 1;
		}
		else {
			xz = x - 1;
			yz = y;
		}

		// Handle border cases
		if (xz < 0) {
			for (int j = 0; j < tileSetAmount; j++) {
				cutList[3].push_back(j);
			}
			continue;
		}

		else if (xz >= grid_dim) {
			for (int j = 0; j < tileSetAmount; j++) {
				cutList[1].push_back(j);
			}
			continue;
		}

		if (yz < 0) {
			for (int j = 0; j < tileSetAmount; j++) {
				cutList[0].push_back(j);
			}
			continue;
		}

		else if (yz >= grid_dim) {
			for (int j = 0; j < tileSetAmount; j++) {
				cutList[2].push_back(j);
			}
			continue;
		}

		// Get neighbor tiles
		if (indArr[yz][xz] == -1) {
			for (int j = 0; j < tileSetAmount; j++) {
				cutList[i].push_back(j);
			}
		}
		else {
			cutList[i] = connections[indArr[yz][xz]][(i + 2) % 4];
		}

		std::sort(cutList[i].begin(), cutList[i].end());
	}

	// Intersect edge potentials
	std::vector<int> inter0{};
	std::vector<int> inter1{};

	std::set_intersection(cutList[0].begin(), cutList[0].end(), cutList[1].begin(), cutList[1].end(), std::back_inserter(inter0));
	std::set_intersection(cutList[2].begin(), cutList[2].end(), cutList[3].begin(), cutList[3].end(), std::back_inserter(inter1));

	std::vector<int> out{};
	std::set_intersection(inter0.begin(), inter0.end(), inter1.begin(), inter1.end(), std::back_inserter(out));

	return out;
}

CollapseHandler::CollapseHandler(uint16_t tile_width, const uint16_t grid_dim)
	:tile_width(tile_width), grid_dim(grid_dim)
{
	for (int i = 0; i < grid_dim; i++) {
		indArr.push_back(std::vector<int>(grid_dim));
		std::fill(indArr[i].begin(), indArr[i].end(), -1);
	}
}

CollapseHandler::~CollapseHandler()
{
	faces.clear();
	connections.clear();

	indArr.clear();
	indArr.shrink_to_fit();

	tiles.clear();
	tiles.shrink_to_fit();
}

void CollapseHandler::init(TILESET set)
{
	this->set = set;

	initConnections();
	initTiles();
}

void CollapseHandler::generateConnections()
{
	for (int i = 0; i < faces.size(); i++) {
		std::vector<std::vector<int>> compList = { {} , {} , {} , {} };
		for (int k = 0; k < 4; k++) {
			for (int j = 0; j < faces.size(); j++) {
				if (faces[i][k] == faces[j][(static_cast<std::vector<int, std::allocator<int>>::size_type>(k) + 2) % 4]) {
					compList[k].push_back(j);
				}
			}
		}
		connections[i] = { compList[0], compList[1], compList[2], compList[3] };
	}
}

void CollapseHandler::collapse(uint16_t initTile, bool generateDownwards)
{
	srand(time(0));
	std::vector<int> potentials{};

	if (generateDownwards) {
		indArr[0][0] = initTile;

		for (int y = 0; y < grid_dim; y++) {
			for (int x = 0; x < grid_dim; x++) {
				if (indArr[y][x] != -1) continue;
				potentials = getPotentials(x, y);
				uint16_t pick = rand() % potentials.size();
				indArr[y][x] = potentials[pick];

				potentials.clear();
				potentials.shrink_to_fit();
			}
		}
	}
	else {
		indArr[grid_dim - 1][grid_dim - 1] = initTile;

		for (int y = grid_dim - 1; y >= 0; y--) {
			for (int x = grid_dim - 1; x >= 0; x--) {
				if (indArr[y][x] != -1) continue;
				potentials = getPotentials(x, y);
				uint16_t pick = rand() % potentials.size();
				indArr[y][x] = potentials[pick];

				potentials.clear();
				potentials.shrink_to_fit();
			}
		}
	}
}

void CollapseHandler::draw(cv::Mat canvas)
{
	for (int i = 0; i < grid_dim; i++) {
		for (int j = 0; j < grid_dim; j++) {
			placeOnImage(canvas, tiles[indArr[j][i]], i * tile_width, j * tile_width);
		}
	}
}
