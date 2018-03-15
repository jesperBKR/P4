#ifndef IMG_PROC_H
#define IMG_PROC_H

//OpenCV Headers
#include "ros/ros.h"
#include <opencv2/core/core.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/highgui/highgui.hpp>
//RealSense Headers
#include <librealsense/rs.hpp>
class img_proc {
public:
  bool initialize_streaming();
  cv::Mat display_next_frame();
  void track(cv::Mat current_frame);
  int debugger();
  rs::context      _rs_ctx;
  rs::device&      _rs_camera = *_rs_ctx.get_device( 0 );
  rs::intrinsics   _depth_intrin;
  rs::intrinsics  _color_intrin;
  rs::intrinsics  _infrared;
  bool         _loop = true;
  bool debugMode = false;
  float pixelx = 0;
  float pixely = 0;
  rs::float2 float2d;
  rs::float3 coord;
  float depth_distance;
  float depth_meters;


private:
  bool first_loop = true;
  int image_type = 0;
  int current_frame;
  bool pause = false;
  bool done = true;
  void drawSquare(cv::Mat input);
  cv::Mat new_frame, lf_grey, nf_grey, difference,threshholding;


};

#endif
