
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_library(PatternExtractor
    PatternExtractor.cpp
)


target_include_directories(PatternExtractor
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(PatternExtractor
    PUBLIC
    ${OpenCV_LIBS}
)