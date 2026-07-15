#include <opencv4/opencv2/opencv.hpp>

#define MARGIN 100
#define LINE_THICKNESS 2
#define MIN_DIMENSION 720.0f

int main()
{

    cv::Mat image = cv::imread("/home/fio/dragonite.png");

    // cv::imshow("Dragonite", image);

    int num_cols = image.cols;
    int num_rows = image.rows;

    float scale_factor = num_cols < num_rows ? MIN_DIMENSION / num_cols : MIN_DIMENSION / num_rows;

    int new_cols = static_cast<int>(num_cols * scale_factor);
    int new_rows = static_cast<int>(num_rows * scale_factor);

    cv::Mat tapestry(new_rows, new_cols, CV_8UC3, cv::Scalar(255, 255, 255));

    int margin = MARGIN;
    int line_thickness = LINE_THICKNESS;

    float cols_scale_factor = (new_cols - 2 * margin - line_thickness * num_cols) / static_cast<float>(num_cols);
    float rows_scale_factor = (new_rows - 2 * margin - line_thickness * num_rows) / static_cast<float>(num_rows);

    for (int i = 0; i < new_rows; ++i)
    {
        for (int j = 0; j < new_cols; ++j)
        {
            int orig_i = static_cast<int>(i / rows_scale_factor);
            int orig_j = static_cast<int>(j / cols_scale_factor);

            if (orig_i < num_rows && orig_j < num_cols)
            {
                tapestry.at<cv::Vec3b>(i + margin + line_thickness * orig_i, j + margin + line_thickness * orig_j) = image.at<cv::Vec3b>(orig_i, orig_j);
            }
        }
    }

    cv::imshow("Tapestry", tapestry);

    cv::waitKey(0);

    return 0;
}