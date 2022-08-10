
#include "CollapseHandler.h"

#define tile_width 64
#define grid_dimension 24
#define images 1

int main(void) {
    // Canvas
    cv::Mat canvas = cv::Mat(grid_dimension * tile_width, grid_dimension * tile_width, CV_8UC3, cv::Scalar(0, 0, 0));

    // Handler
    CollapseHandler* Handler = new CollapseHandler(tile_width, grid_dimension);
    Handler->init(TILESET::CITY);
    Handler->generateConnections();

    for (int i = 0; i < images; i++) {
        Handler->collapse(7, false);

        Handler->draw(canvas);

        cv::imwrite("Assets/out" + std::to_string(rand() % 999999) + ".png", canvas);

        if (images <= 1) {
            cv::imshow("Map", canvas);
            cv::moveWindow("Map", 0, 200);

            cv::waitKey(0);
        }
    }
    
    delete Handler;

    return(0);
}