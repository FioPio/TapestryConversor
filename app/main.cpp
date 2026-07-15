#include <opencv4/opencv2/opencv.hpp>

#define MARGIN 100
#define LINE_MARGIN 1
#define LINE_THICKNESS 1
#define PIXEL_SIZE 25

int main()
{

    cv::Mat image = cv::imread("/home/fio/dragonite.png");

    // cv::imshow("Dragonite", image);

    int num_cols = image.cols;
    int num_rows = image.rows;

    int margin = MARGIN;
    int line_thickness = LINE_THICKNESS;
    int line_margin = LINE_MARGIN;                     // Adjust for the inner line thickness
    int line_space = line_thickness + 2 * line_margin; // Total space taken by the line and margin
    int pixel_size = PIXEL_SIZE;

    int new_cols = (num_cols * pixel_size) + (num_cols - 1) * line_space + 2 * margin;
    int new_rows = (num_rows * pixel_size) + (num_rows - 1) * line_space + 2 * margin;

    cv::Mat tapestry(new_rows, new_cols, CV_8UC3, cv::Scalar(255, 255, 255));
    // Copy image
    for (int i = 0; i < new_rows; ++i)
    {
        for (int j = 0; j < new_cols; ++j)
        {
            int orig_i = static_cast<int>(i / pixel_size);
            int orig_j = static_cast<int>(j / pixel_size);

            if (orig_i < num_rows && orig_j < num_cols)
            {
                tapestry.at<cv::Vec3b>(i + margin + line_space * orig_i, j + margin + line_space * orig_j) = image.at<cv::Vec3b>(orig_i, orig_j);
            }
        }
    }
    // Draw 1px line to separate each square
    for (int num_row = 0; num_row < num_rows + 1; ++num_row)
    {
        cv::line(tapestry,
                 cv::Point(margin, margin + num_row * (pixel_size + line_space) - line_margin - line_thickness),
                 cv::Point(new_cols - margin, margin + num_row * (pixel_size + line_space) - line_margin - line_thickness),
                 cv::Scalar(0, 0, 0), line_thickness);
    }
    for (int num_column = 0; num_column < num_cols + 1; ++num_column)
    {
        cv::line(tapestry,
                 cv::Point(margin + num_column * (pixel_size + line_space) - line_margin - line_thickness, margin),
                 cv::Point(margin + num_column * (pixel_size + line_space) - line_margin - line_thickness, new_rows - margin),
                 cv::Scalar(0, 0, 0), line_thickness);
    }
    // Write the number of column and row to the image on the center of each square
    for (int num_column = 0; num_column < num_cols; ++num_column)
    {
        cv::Size text_size = cv::getTextSize(std::to_string(num_column), cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, nullptr);

        int x = margin + line_space * num_column + static_cast<int>(num_column * pixel_size) + (pixel_size - text_size.width) / 2;
        int y1 = margin - 10;            // Position above the grid
        int y2 = new_rows - margin + 20; // Position below the grid

        std::string text = std::to_string(num_column + 1);
        cv::putText(tapestry,
                    text,
                    cv::Point(x, y1), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);

        cv::putText(tapestry,
                    text,
                    cv::Point(x, y2), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
    }
    for (int num_row = 0; num_row < num_rows; ++num_row)
    {
        cv::Size text_size = cv::getTextSize(std::to_string(num_row), cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, nullptr);

        int x1 = margin - 10 - text_size.width;                             // Position to the left of the grid
        int x2 = new_cols - margin + 20 - text_size.width + 2 * line_space; // Position to the right of the grid
        int y = margin + line_space * num_row + static_cast<int>(num_row * pixel_size) + (pixel_size + text_size.height) / 2;

        int real_row = num_rows - num_row; // Calculate the real row number

        if (real_row % 2 == 0) // [WS]
        {
            cv::putText(tapestry, std::to_string(real_row), cv::Point(x1, y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
        }
        else // [RS]
        {
            cv::putText(tapestry, std::to_string(real_row), cv::Point(x2, y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
        }
    }

    struct ColorCount
    {
        cv::Vec3b color;
        int count;
    };

    std::vector<cv::Vec3b> colors_used = {};
    std::vector<std::vector<ColorCount>> color_counts = {};

    for (int num_row = 0; num_row < num_rows; ++num_row)
    {
        int real_row = num_rows - num_row - 1; // Calculate the real row number
        ColorCount last_color = {{0, 0, 0}, 0};

        std::vector<ColorCount> current_row;

        for (int num_column = 0; num_column < num_cols; ++num_column)
        {
            int real_column = real_row % 2 == 0 ? num_column : num_cols - 1 - num_column;
            cv::Vec3b color = image.at<cv::Vec3b>(real_row, real_column);

            if (last_color.color == color)
            {
                last_color.count++;
            }
            else
            {
                if (last_color.count > 0)
                {
                    current_row.push_back(last_color);
                }
                last_color = {color, 1};
            }

            if (std::find(colors_used.begin(), colors_used.end(), color) == colors_used.end())
            {
                colors_used.push_back(color);
            }
        }
        current_row.push_back(last_color);
        color_counts.push_back(current_row);
    }

    std::cout << "Color counts per row:" << std::endl;
    for (int num_row = 0; num_row < color_counts.size(); ++num_row)
    {
        std::cout << "Row " << num_row + 1 << ": ";
        for (const auto &color_count : color_counts[num_row])
        {
            std::cout << "(" << (int)color_count.color[2] << ", "
                      << (int)color_count.color[1] << ", "
                      << (int)color_count.color[0] << ") x " << color_count.count << " | ";
        }
        std::cout << std::endl;
    }

    cv::imshow("Tapestry", tapestry);

    cv::waitKey(0);

    std::cout << "Colors used: " << colors_used.size() << std::endl;

    return 0;
}