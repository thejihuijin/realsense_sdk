// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2016 Intel Corporation. All Rights Reserved.

#include <memory>
#include <iostream>
#include <librealsense/rs.hpp>

#include "rs_sdk.h"

#include <GLFW/glfw3.h>

using namespace rs::core;
using namespace std;

enum Recording_Status { stream_only, start_recording, recording, end_recording };
std::string timestamp() {
  std::time_t now = std::time(NULL);
  std::tm * ptm = std::localtime(&now);
  char buffer[32];
  // Format: Mo, 15.06.2009 20:20:00
  std::strftime(buffer, 32, "%Y%m%d_%H%M%S", ptm);
  return buffer;
}
Recording_Status current_session = stream_only;
bool new_stream = true;
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  std::cout << "key call back" << std::endl;
  if (action == GLFW_RELEASE) {
    switch(key)
    {
      case GLFW_KEY_ESCAPE:
        new_stream = false;
        current_session = end_recording;
        break;
      case GLFW_KEY_SPACE:
        //std::cout << "space" << std::endl;
        //if (current_session == stream_only)
        //  current_session = start_recording;
        if (current_session == recording) 
          current_session = end_recording;
        break;
      default:
        break;
    }
  }
}
int main(int argc, char* argv[]) try
{

  // Continuously create context/recordings until escaped
  //for (int i=0; i < 4; ++i) {
  while (new_stream) {
    //create a record file
    std::string filename = timestamp() + ".rs";
    rs::record::context context(filename.c_str());
    current_session = recording;//stream_only;
    int number_of_frames = 300;

    if(context.get_device_count() == 0)
    {
      cerr<<"no device detected" << endl;
      return -1;
    }
    std::cout << "device found" << std::endl;

    //each device created from the record enabled context will write the streaming data to the given file
    rs::record::device* device = context.get_record_device(0);

    //enable required streams
    vector<rs::stream> streams = { rs::stream::color, rs::stream::depth, rs::stream::infrared  };

    device->enable_stream(rs::stream::color, 640, 480, rs::format::rgb8, 30);
    device->enable_stream(rs::stream::depth, 640, 480, rs::format::z16, 30);
    device->enable_stream(rs::stream::infrared, 640, 480, rs::format::y8, 30);
    if (device->supports((rs::capabilities) rs::stream::infrared2)) {
      device->enable_stream(rs::stream::infrared2, 640, 480, rs::format::y8, 30);
      streams.push_back(rs::stream::infrared2);
    }


    for(auto stream : streams)
    {
      std::cout << "stream type: " << stream << ", width: " << device->get_stream_width(stream) << ", height: " << device->get_stream_height(stream) << ", format: " << device->get_stream_format(stream) << ", fps: " << device->get_stream_framerate(stream) << std::endl;
    }

    std:: cout << "starting new session" << std::endl;
    //device->pause_record();
    // device->start(rs::source::all_sources);
    device->start();

    // Open a GLFW window to display our output
    glfwInit();
    GLFWwindow * win = glfwCreateWindow(640, 480, "librealsense tutorial #2", nullptr, nullptr);
    glfwMakeContextCurrent(win);
    glfwSetKeyCallback(win, key_callback);
    while(!glfwWindowShouldClose(win))
    //for (int i = 0; i < number_of_frames; ++i) 
    {
      if (current_session == start_recording) {
        // space key pressed, beginning recording
        std::cout << "Beginning record" << std::endl;
        current_session = recording;
        //device->resume_record();
      } else if (current_session == end_recording) {
        std::cout << "Ending record" << std::endl;
        glfwSetWindowShouldClose(win, GL_TRUE);
      }
      device->wait_for_frames();
      //use the recorded frame...
      //glClear(GL_COLOR_BUFFER_BIT);
      //glPixelZoom(1, -1);

      // Display depth data by linearly mapping depth between 0 and 2 meters to the red channel
      //glRasterPos2f(-1, 1);
      //glPixelTransferf(GL_RED_SCALE, 0xFFFF * device->get_depth_scale() / 2.0f);
      //glDrawPixels(640, 480, GL_RED, GL_UNSIGNED_SHORT, device->get_frame_data(rs::stream::depth));
      //glPixelTransferf(GL_RED_SCALE, 1.0f);

      // Display color image as RGB triples
      //glRasterPos2f(0, 1);
      glDrawPixels(640, 480, GL_RGB, GL_UNSIGNED_BYTE, device->get_frame_data(rs::stream::color));

      // Display infrared image by mapping IR intensity to visible luminance
      //glRasterPos2f(-1, 0);
      //glDrawPixels(640, 480, GL_LUMINANCE, GL_UNSIGNED_BYTE, device->get_frame_data(rs::stream::infrared));
      

      // Display second infrared image by mapping IR intensity to visible luminance
      //if(device->is_stream_enabled(rs::stream::infrared2))
      //{
        //glRasterPos2f(0, 0);
      //  glDrawPixels(640, 480, GL_LUMINANCE, GL_UNSIGNED_BYTE, device->get_frame_data(rs::stream::infrared2));
     // }

      glfwSwapBuffers(win);
      glfwPollEvents();

    }
    glfwDestroyWindow(win);
    glfwWaitEvents();
    std::cout << "stopping device" << std::endl;
    device->stop();
  }

  std::cout << "done" << std::endl;

  return 0;
}

catch(rs::error e)
{
  std::cout << e.what() << std::endl;
  return -1;
}
