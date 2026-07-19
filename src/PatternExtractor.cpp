#include "PatternExtractor.h"

std::vector<std::vector<ColorCount>> GetColorCounts(const cv::Mat &image)
{
    std::vector<std::vector<ColorCount>> color_counts;

    int num_rows = image.rows;
    int num_cols = image.cols;

    for (int num_row = 0; num_row < num_rows; ++num_row)
    {
        int real_row = num_rows - num_row - 1; // Calculate the real row number
        ColorCount last_color = {{0, 0, 0}, 0};

        std::vector<ColorCount> current_row;

        for (int num_column = 0; num_column < num_cols; ++num_column)
        {
            cv::Vec3b color = image.at<cv::Vec3b>(real_row, num_column);

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
        }
        current_row.push_back(last_color);
        color_counts.push_back(current_row);
    }

    return color_counts;
}

std::vector<cv::Vec3b> GetColorsUsed(const std::vector<std::vector<ColorCount>> &color_counts)
{
    std::vector<cv::Vec3b> colors_used;

    for (const auto &row : color_counts)
    {
        for (const auto &color_count : row)
        {
            if (std::find(colors_used.begin(), colors_used.end(), color_count.color) == colors_used.end())
            {
                colors_used.push_back(color_count.color);
            }
        }
    }

    return colors_used;
}

cv::Mat DrawPatternGrid(const std::vector<std::vector<ColorCount>> &color_counts, const GridConfig &config)
{
    int num_rows = color_counts.size();
    int num_cols = 0;

    int line_space = config.line_thickness + 2 * config.line_margin;
    for (const auto &row : color_counts)
    {
        int row_cols = 0;
        for (const auto &color_count : row)
        {
            row_cols += color_count.count;
        }
        num_cols = std::max(num_cols, row_cols);
    }

    int new_cols = (num_cols * config.pixel_size) + (num_cols - 1) * (line_space) + 2 * config.margin;
    int new_rows = (num_rows * config.pixel_size) + (num_rows - 1) * (line_space) + 2 * config.margin;

    cv::Mat grid(new_rows, new_cols, CV_8UC3, cv::Scalar(255, 255, 255));

    for (int num_row = 0; num_row < num_rows; ++num_row)
    {
        int y_offset = config.margin + num_row * (config.pixel_size + line_space);

        int x_offset = config.margin;
        for (const auto &color_count : color_counts[num_rows - 1 - num_row])
        {
            for (int i = 0; i < color_count.count; ++i)
            {
                cv::rectangle(grid,
                              cv::Point(x_offset, y_offset),
                              cv::Point(x_offset + config.pixel_size - 1, y_offset + config.pixel_size - 1),
                              cv::Scalar(color_count.color[0], color_count.color[1], color_count.color[2]), cv::FILLED);

                x_offset += config.pixel_size + line_space;
            }
        }

        // Drawing row numbers on the left and right of the grid

        cv::Size text_size = cv::getTextSize(std::to_string(num_row), cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, nullptr);

        int x1 = config.margin - 10 - text_size.width;                             // Position to the left of the grid
        int x2 = new_cols - config.margin + 20 - text_size.width + 2 * line_space; // Position to the right of the grid
        int y = config.margin + line_space * num_row + static_cast<int>(num_row * config.pixel_size) + (config.pixel_size + text_size.height) / 2;

        int real_row = num_rows - num_row; // Calculate the real row number

        cv::putText(grid, std::to_string(real_row), cv::Point(x1, y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);

        cv::putText(grid, std::to_string(real_row), cv::Point(x2, y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);

        // Drawing line
        cv::line(grid,
                 cv::Point(config.margin, config.margin + num_row * (config.pixel_size + line_space) - config.line_margin - config.line_thickness),
                 cv::Point(new_cols - config.margin, config.margin + num_row * (config.pixel_size + line_space) - config.line_margin - config.line_thickness),
                 cv::Scalar(0, 0, 0), config.line_thickness);
    }

    for (int num_column = 0; num_column < num_cols; ++num_column)
    {
        cv::Size text_size = cv::getTextSize(std::to_string(num_column), cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, nullptr);

        int x = config.margin + line_space * num_column + static_cast<int>(num_column * config.pixel_size) + (config.pixel_size - text_size.width) / 2;
        int y1 = config.margin - 10;            // Position above the grid
        int y2 = new_rows - config.margin + 20; // Position below the grid

        std::string text = std::to_string(num_column + 1);
        cv::putText(grid,
                    text,
                    cv::Point(x, y1), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);

        cv::putText(grid,
                    text,
                    cv::Point(x, y2), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);

        // Drawing lines
        cv::line(grid,
                 cv::Point(config.margin + num_column * (config.pixel_size + line_space) - config.line_margin - config.line_thickness, config.margin),
                 cv::Point(config.margin + num_column * (config.pixel_size + line_space) - config.line_margin - config.line_thickness, new_rows - config.margin),
                 cv::Scalar(0, 0, 0), config.line_thickness);
    }

    return grid;
}

cv::Mat DrawSCInstructions(const std::vector<std::vector<ColorCount>> &color_counts, const GridConfig &config)
{
    int num_rows = color_counts.size();
    int num_cols = 0;
    int line_space = config.line_thickness + 2 * config.line_margin;
    for (const auto &row : color_counts)
    {
        int row_cols = 0;
        for (const auto &color_count : row)
        {
            row_cols += color_count.count;
        }
        num_cols = std::max(num_cols, row_cols);
    }

    int num_new_rows = num_rows * config.pixel_size + (num_rows - 1) * line_space + config.margin;
    int new_cols = num_cols * config.pixel_size + (num_cols - 1) * line_space + 2 * config.margin;

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

        int x_offset = config.margin / 4;
        int y = config.margin / 2 + num_row * (config.pixel_size + line_space) + config.pixel_size / 2;

        cv::putText(instructions,
                    text,
                    cv::Point(x_offset, y),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);

        int color_x = x_offset + text_size.width;
        for (int num_color = 0; num_color < color_counts[num_row].size(); ++num_color)
        {
            int use_color = num_color;
            if (num_row % 2 == 0)
            {
                use_color = color_counts[num_row].size() - 1 - num_color;
            }

            // Rectangle fill
            cv::rectangle(instructions,
                          cv::Point(color_x, y - config.pixel_size / 2),
                          cv::Point(color_x + config.pixel_size, y + config.pixel_size / 2),
                          cv::Scalar(color_counts[num_row][use_color].color[0], color_counts[num_row][use_color].color[1], color_counts[num_row][use_color].color[2]), cv::FILLED);

            // Surounding box
            cv::rectangle(instructions,
                          cv::Point(color_x, y - config.pixel_size / 2),
                          cv::Point(color_x + config.pixel_size, y + config.pixel_size / 2),
                          cv::Scalar(0, 0, 0), config.line_thickness);

            color_x += config.pixel_size + line_space;

            std::string count_text = std::to_string(color_counts[num_row][use_color].count);
            cv::Size count_text_size = cv::getTextSize(count_text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 4, nullptr);
            int count_y = y + (count_text_size.height / 2);
            cv::putText(instructions,
                        count_text,
                        cv::Point(color_x - config.pixel_size - line_space + (config.pixel_size - count_text_size.width) / 2 + 2, count_y),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 4);

            count_text_size = cv::getTextSize(count_text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 2, nullptr);
            count_y = y + (count_text_size.height / 2);
            cv::putText(instructions,
                        count_text,
                        cv::Point(color_x - config.pixel_size - line_space + (config.pixel_size - count_text_size.width) / 2 + 1, count_y),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2);
        }
    }
    return instructions;
}

cv::Mat DrawBubbleInstructions(const std::vector<std::vector<ColorCount>> &color_counts, const GridConfig &config)
{
    int num_rows = color_counts.size() * 2;
    int num_cols = 0;
    int line_space = config.line_thickness + 2 * config.line_margin;
    for (const auto &row : color_counts)
    {
        int row_cols = 0;
        for (const auto &color_count : row)
        {
            row_cols += color_count.count;
        }
        num_cols = std::max(num_cols, row_cols);
    }

    int num_new_rows = (num_rows + 1) * config.pixel_size + num_rows * line_space + config.margin;
    int new_cols = num_cols * config.pixel_size + (num_cols - 1) * line_space + 2 * config.margin;

    cv::Mat instructions(num_new_rows, new_cols, CV_8UC3, cv::Scalar(255, 255, 255));

    std::string text = "Row  0 [FC] : ch x " + std::to_string(num_cols * 2 + 2);

    cv::Size text_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, nullptr);

    int x_offset = config.margin / 4;
    int y = config.margin / 2 + config.pixel_size / 2;

    cv::putText(instructions,
                text,
                cv::Point(x_offset, y),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);

    for (int num_row = 0; num_row < color_counts.size(); ++num_row)
    {
        for (int sub_row = 0; sub_row < 2; ++sub_row)
        {
            std::string text = "Row ";
            if (num_row < 9)
            {
                text += " ";
            }

            // Bubble instructions
            text += std::to_string(num_row + 1) + " ";

            if (sub_row == 0)
            {
                text += "[WS] -> (SC): ";
            }
            else
            {
                text += "[RS] -> (BU): ";
            }

            cv::Size text_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, nullptr);

            int x_offset = config.margin / 4;
            int y = config.margin / 2 + (2 * num_row + 1 + sub_row) * (config.pixel_size + line_space) + config.pixel_size / 2;

            cv::putText(instructions,
                        text,
                        cv::Point(x_offset, y),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);

            int color_x = x_offset + text_size.width;
            for (int num_color = 0; num_color < color_counts[num_row].size(); ++num_color)
            {
                int use_color = sub_row == 0 ? num_color : color_counts[num_row].size() - 1 - num_color;

                // Rectangle fill
                cv::rectangle(instructions,
                              cv::Point(color_x, y - config.pixel_size / 2),
                              cv::Point(color_x + config.pixel_size, y + config.pixel_size / 2),
                              cv::Scalar(color_counts[num_row][use_color].color[0], color_counts[num_row][use_color].color[1], color_counts[num_row][use_color].color[2]), cv::FILLED);

                // Surounding box
                cv::rectangle(instructions,
                              cv::Point(color_x, y - config.pixel_size / 2),
                              cv::Point(color_x + config.pixel_size, y + config.pixel_size / 2),
                              cv::Scalar(0, 0, 0), config.line_thickness);

                color_x += config.pixel_size + line_space;

                std::string count_text = std::to_string(color_counts[num_row][use_color].count * (2 - sub_row));
                cv::Size count_text_size = cv::getTextSize(count_text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 4, nullptr);
                int count_y = y + (count_text_size.height / 2);
                cv::putText(instructions,
                            count_text,
                            cv::Point(color_x - config.pixel_size - line_space + (config.pixel_size - count_text_size.width) / 2 + 2, count_y),
                            cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 4);

                count_text_size = cv::getTextSize(count_text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 2, nullptr);
                count_y = y + (count_text_size.height / 2);
                cv::putText(instructions,
                            count_text,
                            cv::Point(color_x - config.pixel_size - line_space + (config.pixel_size - count_text_size.width) / 2 + 1, count_y),
                            cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2);
            }
        }
    }
    return instructions;
}

void GeneratePDF(const char *image_path, const GridConfig &config)
{
    cv::Mat image = cv::imread(image_path);

    std::string output_path = image_path;
    size_t lastindex = output_path.find_last_of(".");

    output_path = output_path.substr(0, lastindex);

    if (config.sc)
    {
        output_path += "_sc_tapestry.png";
    }
    else
    {
        output_path += "_bubble_tapestry.png";
    }

    int line_space = config.line_thickness + 2 * config.line_margin;

    std::vector<std::vector<ColorCount>> color_counts = GetColorCounts(image);

    cv::Mat tapestry = DrawPatternGrid(color_counts, config);

    std::vector<cv::Vec3b> colors_used = GetColorsUsed(color_counts);

    cv::Mat instructions = config.sc ? DrawSCInstructions(color_counts, config) : DrawBubbleInstructions(color_counts, config);

    cv::Mat combined(tapestry.rows + instructions.rows, tapestry.cols, CV_8UC3, cv::Scalar(255, 255, 255));
    tapestry.copyTo(combined(cv::Rect(0, 0, tapestry.cols, tapestry.rows)));
    instructions.copyTo(combined(cv::Rect(0, tapestry.rows, instructions.cols, instructions.rows)));

    for (int num_color = 0; num_color < colors_used.size(); ++num_color)
    {
        int y_offset = tapestry.rows - config.margin / 2;
        cv::rectangle(combined,
                      cv::Point(config.margin + num_color * (config.pixel_size + line_space), y_offset),
                      cv::Point(config.margin + num_color * (config.pixel_size + line_space) + config.pixel_size, y_offset + config.pixel_size),
                      cv::Scalar(colors_used[num_color][0], colors_used[num_color][1], colors_used[num_color][2]), cv::FILLED);

        cv::rectangle(combined,
                      cv::Point(config.margin + num_color * (config.pixel_size + line_space), y_offset),
                      cv::Point(config.margin + num_color * (config.pixel_size + line_space) + config.pixel_size, y_offset + config.pixel_size),
                      cv::Scalar(0, 0, 0), config.line_thickness);
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
        cv::Scalar(255, 255, 255));

    int x = (page_width - resized.cols) / 2;
    int y = (page_height - resized.rows) / 2;

    resized.copyTo(
        page(
            cv::Rect(
                x,
                y,
                resized.cols,
                resized.rows)));

    cv::imwrite(output_path, page);
}