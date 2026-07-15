#include <opencv4/opencv2/opencv.hpp>

int main()
{

    cv::Mat image = cv::imread("/home/fio/dragonite.png");

    cv::imshow("Dragonite", image);

    cv::waitKey(0);

    return 0;
}