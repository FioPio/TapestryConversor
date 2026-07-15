
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(
    Tapestry
    main.cpp
)

target_link_libraries( Tapestry ${OpenCV_LIBS} )