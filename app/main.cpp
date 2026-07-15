#include <opencv4/opencv2/opencv.hpp>

#define MARGIN 100
#define LINE_MARGIN 1
#define LINE_THICKNESS 1
#define PIXEL_SIZE 25

int main(int argc, char **argv)
{
    // Get image path from command line arguments
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <image_path> " << std::endl;
        return 1;
    }

    cv::Mat image = cv::imread(argv[1]);

    std::string output_path = argv[1];
    size_t lastindex = output_path.find_last_of(".");
    output_path = output_path.substr(0, lastindex) + "_tapestry.png";

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

        if (real_row % 2 == 0) // [RS]
        {
            cv::putText(tapestry, std::to_string(real_row), cv::Point(x1, y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
        }
        else // [WS]
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

    int num_new_rows = num_rows * pixel_size + (num_rows - 1) * line_space + margin;

    cv::Mat instructions(num_new_rows, new_cols, CV_8UC3, cv::Scalar(255, 255, 255));

    for (int num_row = 0; num_row < color_counts.size(); ++num_row)
    {
        std::string text = "Row ";
        if (num_row < 9)
        {
            text += " ";
        }

        text += std::to_string(num_row + 1) + " ";

        if (num_row % 2 == 0)
        {
            text += "[RS] <- : ";
        }
        else
        {
            text += "[WS] -> : ";
        }

        cv::Size text_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, nullptr);

        int x_offset = margin / 4;
        int y = margin / 2 + num_row * (pixel_size + line_space) + pixel_size / 2;
        cv::putText(instructions,
                    text,
                    cv::Point(x_offset, y),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);

        int color_x = x_offset + text_size.width;
        for (int num_color = 0; num_color < color_counts[num_row].size(); ++num_color)
        {
            // Rectangle fill
            cv::rectangle(instructions,
                          cv::Point(color_x, y - pixel_size / 2),
                          cv::Point(color_x + pixel_size, y + pixel_size / 2),
                          cv::Scalar(color_counts[num_row][num_color].color[0], color_counts[num_row][num_color].color[1], color_counts[num_row][num_color].color[2]), cv::FILLED);

            // Surounding box
            cv::rectangle(instructions,
                          cv::Point(color_x, y - pixel_size / 2),
                          cv::Point(color_x + pixel_size, y + pixel_size / 2),
                          cv::Scalar(0, 0, 0), line_thickness);

            color_x += pixel_size + line_space;

            std::string count_text = std::to_string(color_counts[num_row][num_color].count);
            cv::Size count_text_size = cv::getTextSize(count_text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 4, nullptr);
            int count_y = y + (count_text_size.height / 2);
            cv::putText(instructions,
                        count_text,
                        cv::Point(color_x - pixel_size - line_space + (pixel_size - count_text_size.width) / 2 + 2, count_y),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 4);

            count_text_size = cv::getTextSize(count_text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 2, nullptr);
            count_y = y + (count_text_size.height / 2);
            cv::putText(instructions,
                        count_text,
                        cv::Point(color_x - pixel_size - line_space + (pixel_size - count_text_size.width) / 2 + 1, count_y),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2);
        }
    }

    cv::Mat combined(tapestry.rows + instructions.rows, tapestry.cols, CV_8UC3, cv::Scalar(255, 255, 255));
    tapestry.copyTo(combined(cv::Rect(0, 0, tapestry.cols, tapestry.rows)));
    instructions.copyTo(combined(cv::Rect(0, tapestry.rows, instructions.cols, instructions.rows)));

    for (int num_color = 0; num_color < colors_used.size(); ++num_color)
    {
        int y_offset = tapestry.rows - margin / 2;
        cv::rectangle(combined,
                      cv::Point(margin + num_color * (pixel_size + line_space), y_offset),
                      cv::Point(margin + num_color * (pixel_size + line_space) + pixel_size, y_offset + pixel_size),
                      cv::Scalar(colors_used[num_color][0], colors_used[num_color][1], colors_used[num_color][2]), cv::FILLED);

        cv::rectangle(combined,
                      cv::Point(margin + num_color * (pixel_size + line_space), y_offset),
                      cv::Point(margin + num_color * (pixel_size + line_space) + pixel_size, y_offset + pixel_size),
                      cv::Scalar(0, 0, 0), line_thickness);
    }

    // Conversió a prePDF
    const int page_width = 2480; // A4 a 300 DPI
    const int page_height = 3508;

    double scale = std::min(
        (double)page_width / combined.cols,
        (double)page_height / combined.rows);

    cv::Mat resized;
    cv::resize(
        combined,
        resized,
        cv::Size(),
        scale,
        scale,
        cv::INTER_CUBIC);

    cv::Mat page(
        page_height,
        page_width,
        combined.type(),
        cv::Scalar(255, 255, 255) // fons blanc
    );

    int x = (page_width - resized.cols) / 2;
    int y = (page_height - resized.rows) / 2;

    resized.copyTo(
        page(
            cv::Rect(
                x,
                y,
                resized.cols,
                resized.rows)));

    // cv::imshow("Final", page);
    //  cv::imshow("Instructions", instructions);

    cv::imwrite(output_path, page);

    // cv::waitKey(0);

    return 0;
}