cmake_minimum_required(VERSION 2.8)
project(realsense_playback_sync_sample)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenGL REQUIRED)
set(DEPENDENCIES ${OPENGL_LIBRARIES})

#GLFW
# Find glfw header
  find_path(GLFW_INCLUDE_DIR NAMES GLFW/glfw3.h
      PATHS /usr/X11R6/include
      /usr/include/X11
      /opt/graphics/OpenGL/include
      /opt/graphics/OpenGL/contrib/libglfw
      /usr/local/include
      /usr/include/GL
      /usr/include
      )
# Find glfw library
  find_library(GLFW_LIBRARIES NAMES glfw glfw3
      PATHS /usr/lib64
      /usr/lib
      /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}
      /usr/local/lib64
      /usr/local/lib
      /usr/local/lib/${CMAKE_LIBRARY_ARCHITECTURE}
      /usr/X11R6/lib
      )
  list(APPEND DEPENDENCIES m ${GLFW_LIBRARIES} ${LIBUSB1_LIBRARIES})
  include_directories(${GLFW_INCLUDE_DIR})

# Opencv
#message("OPen CV dir: ${OpenCV_DIR}")
#set(OpenCV_DIR "/usr/local/lib")
#message("OPen CV dir: ${OpenCV_DIR}")
#find_package(OpenCV REQUIRED)
set(OpenCV_INCLUDE_DIRS "/usr/local/include/opencv;/usr/local/include")
set(OpenCV_LIBS "opencv_viz;opencv_videostab;opencv_videoio;opencv_video;opencv_superres;opencv_stitching;opencv_shape;opencv_photo;opencv_objdetect;opencv_ml;opencv_imgproc;opencv_imgcodecs;opencv_highgui;opencv_flann;opencv_features2d;opencv_core;opencv_calib3d")
include_directories(${OpenCV_INCLUDE_DIRS})
list(APPEND DEPENDENCIES ${OpenCV_LIBS})

# Boost
find_package( Boost REQUIRED COMPONENTS filesystem system )
include_directories(${Boost_INCLUDE_DIRS})
list(APPEND DEPENDENCIES ${Boost_LIBRARIES})




add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} realsense
      realsense_image
      realsense_playback
      opencv_imgproc
      opencv_core pthread
      ${DEPENDENCIES})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

