// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2016 Intel Corporation. All Rights Reserved.

#include <memory>
#include <iostream>
#include <vector>
#include <librealsense/rs.hpp>
#include "rs_sdk.h"
#include "unistd.h"

// Add graphical Display
#include <GLFW/glfw3.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <boost/filesystem.hpp>

using namespace rs::core;
using namespace std;
namespace fs = boost::filesystem;

int main(int argc, char* argv[]) try
{
    if (argc < 2)
    {
        cerr << "missing playback file argument" << endl;
        return -1;
    }
    if (access(argv[1], F_OK) == -1)
    {
        cerr << "playback file does not exists" << endl;
        return -1;
    }
    const string input_file(argv[1]);

    // lazy "flag check"
    bool extract = argc > 2;

    //create a playback enabled context with a given output file
    rs::playback::context context(input_file.c_str());

    //get device count, in playback context there should be a single device.
    //in case device count is 0 there is probably problem with file location or permissions
    int device_count = context.get_device_count();
    if (device_count == 0)
    {
        cerr << "failed to open playback file" << endl;
        return -1;
    }

    //create a playback enabled device
    rs::device* device = context.get_device(0);

    //enable the recorded streams
    vector<rs::stream> streams = { rs::stream::color, rs::stream::depth, rs::stream::infrared, rs::stream::infrared2, rs::stream::fisheye };

    for(auto stream : streams)
    {
        if(device->get_stream_mode_count(stream) > 0)
        {
            device->enable_stream(stream, rs::preset::best_quality);
            std::cout << "stream type: " << stream << ", width: " << device->get_stream_width(stream) << ", height: " << device->get_stream_height(stream) << ", format: " << device->get_stream_format(stream) << ", fps: " << device->get_stream_framerate(stream) << std::endl;
        }
    }

    device->start();

    // Open a GLFW window to display our output
    glfwInit();
    GLFWwindow * win = glfwCreateWindow(1280, 960, "librealsense tutorial #2", nullptr, nullptr);
    glfwMakeContextCurrent(win);

    std::string outname = "./out";
    // create output directory
    if (extract) {
      // Generate output directory name
      int outnum = 0;
      //std::cout << fs::exists("./out") << std::endl;
      while (fs::exists(fs::path(outname + std::to_string(outnum)))) outnum++;
      std::cout << "Output directory num: " << outnum << std::endl;
      outname += std::to_string(outnum)+"/";
      fs::create_directory(outname);

    }
    int framenum = 0;

    //if theres no more frames the playback device will report that its not streaming
    while(device->is_streaming())
    {
        device->wait_for_frames();
        //for(auto stream : streams)
        //{
         //   if(device->is_stream_enabled(stream))
        //std::cout << "stream type: " << stream << ", timestamp: " << device->get_frame_timestamp(stream) << std::endl;
        //auto frame_data = device->get_frame_data(stream);

        //use the recorded frame...
        glClear(GL_COLOR_BUFFER_BIT);
        glPixelZoom(1, -1);

        // Display depth data by linearly mapping depth between 0 and 2 meters to the red channel
        glRasterPos2f(-1, 1);
        glPixelTransferf(GL_RED_SCALE, 0xFFFF * device->get_depth_scale() / 2.0f);
        glDrawPixels(640, 480, GL_RED, GL_UNSIGNED_SHORT, device->get_frame_data(rs::stream::depth));
        glPixelTransferf(GL_RED_SCALE, 1.0f);

        // Display color image as RGB triples
        glRasterPos2f(0, 1);
        glDrawPixels(640, 480, GL_RGB, GL_UNSIGNED_BYTE, device->get_frame_data(rs::stream::color));

        // Display infrared image by mapping IR intensity to visible luminance
        glRasterPos2f(-1, 0);
        glDrawPixels(640, 480, GL_LUMINANCE, GL_UNSIGNED_BYTE, device->get_frame_data(rs::stream::infrared));

        // Display second infrared image by mapping IR intensity to visible luminance
        if(device->is_stream_enabled(rs::stream::infrared2))
        {
          glRasterPos2f(0, 0);
          glDrawPixels(640, 480, GL_LUMINANCE, GL_UNSIGNED_BYTE, device->get_frame_data(rs::stream::infrared2));
        }

        glfwSwapBuffers(win);
        if (extract) {
          //depthimg = cv::Mat(480,640,CV_16UC1, (uint16_t *) depth_frame);
          const uint16_t * depth_frame =  reinterpret_cast< const uint16_t *>(device->get_frame_data(rs::stream::depth));
          cv::imwrite(outname + "depth"+std::to_string(framenum)+".png", cv::Mat(480,640,CV_16UC1, (uint16_t *) depth_frame));
          //std::cout << "Writing depth image with depth scale: " << device->get_depth_scale() << std::endl;
          //cv::imwrite("test.png", cv::Mat(480,640,CV_16UC1,  depth_frame));
          const uint8_t * left_frame = reinterpret_cast<const uint8_t *> (device->get_frame_data(rs::stream::infrared));
          cv::imwrite(outname + "left" + std::to_string(framenum) + ".png", cv::Mat(480,640,CV_8UC1,(uint8_t *) left_frame));
          const uint8_t * right_frame = reinterpret_cast<const uint8_t *> (device->get_frame_data(rs::stream::infrared2));
          cv::imwrite(outname + "right" + std::to_string(framenum) +" .png", cv::Mat(480,640,CV_8UC1,(uint8_t *) right_frame));
          const uint8_t * color_frame = reinterpret_cast<const uint8_t *> (device->get_frame_data(rs::stream::color));
          cv::imwrite(outname + "color" + std::to_string(framenum) +" .png", cv::Mat(480,640,CV_8UC3,(uint8_t *) color_frame));

        }
        framenum++;

        //}
    }
    glfwSetWindowShouldClose(win, GL_TRUE);
    device->stop();

    return 0;
}

catch(rs::error e)
{
  std::cout << e.what() << std::endl;
  return -1;
}
