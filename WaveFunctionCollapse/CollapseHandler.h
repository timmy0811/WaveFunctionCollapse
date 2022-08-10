#pragma once

#include <map>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>

enum class TILESET { CAVE, CITY };

class CollapseHandler
{
private:
	std::map<int, std::vector<int>> faces;
	std::map<int, std::vector<std::vector<int>>> connections;
	std::vector<std::vector<int>> indArr;
	std::vector<cv::Mat> tiles;

	uint16_t tileSetAmount;
	uint16_t tile_width;
	const uint16_t grid_dim;

	TILESET set;

	void initTiles();
	void initConnections();

	// Image processing
	cv::Mat rotateImage(cv::Mat image, cv::RotateFlags flag);
	void placeOnImage(cv::Mat canvas, cv::Mat image, uint16_t x, uint16_t y);

	// Collapse
	std::vector<int> getPotentials(uint16_t x, uint16_t y);

public:
	CollapseHandler(uint16_t tile_width, const uint16_t grid_dim);
	~CollapseHandler();

	void init(TILESET set);
	void generateConnections();

	void collapse(uint16_t initTile, bool generateDownwards = true);

	void draw(cv::Mat canvas);
};

