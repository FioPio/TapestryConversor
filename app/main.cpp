
#include "PatternExtractor.h"

int main(int argc, char **argv)
{
    // Get image path from command line arguments
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <image_path> " << std::endl;
        return 1;
    }

    GridConfig config{
        .margin = MARGIN,
        .line_thickness = LINE_THICKNESS,
        .line_margin = LINE_MARGIN,
        .pixel_size = PIXEL_SIZE,
        .sc = true};

    if (argc > 2)
    {
        std::string sc_arg = argv[2];
        if (sc_arg == "sc")
        {
            config.sc = true;
        }
        else if (sc_arg == "bubble")
        {
            config.sc = false;
        }
        else
        {
            std::cerr << "Invalid second argument. Use 'sc' for single color or 'bubble' for bubble instructions." << std::endl;
            return 1;
        }
    }

    GeneratePDF(argv[1], config);

    return 0;
}