#pragma once

#include <opencv4/opencv2/opencv.hpp>

#define MARGIN 100
#define LINE_MARGIN 1
#define LINE_THICKNESS 1
#define PIXEL_SIZE 25

struct ColorCount
{
    cv::Vec3b color;
    int count;
};

struct GridConfig
{
    int margin{MARGIN};
    int line_thickness{LINE_THICKNESS};
    int line_margin{LINE_MARGIN};
    int pixel_size{PIXEL_SIZE};
    bool sc = true;
};

// From bottom to top, left to right
std::vector<std::vector<ColorCount>> GetColorCounts(const cv::Mat &image);
std::vector<cv::Vec3b> GetColorsUsed(const std::vector<std::vector<ColorCount>> &color_counts);

cv::Mat DrawPatternGrid(const std::vector<std::vector<ColorCount>> &color_counts, const GridConfig &config);

cv::Mat DrawSCInstructions(const std::vector<std::vector<ColorCount>> &color_counts, const GridConfig &config);

cv::Mat DrawBubbleInstructions(const std::vector<std::vector<ColorCount>> &color_counts, const GridConfig &config);

void GeneratePDF(const char *image_path, const GridConfig &config);