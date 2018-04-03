
#include "ros/ros.h"
#include <std_msgs/String.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <librealsense/rs.hpp>


int count = 0;

void initialize_streaming();

int main (int argc, char **argv)
{
  ros::init(argc, argv, "object_coordinates");
  ros::NodeHandle nh;

  ros::Publisher chatter_pub = nh.advertise<std_msgs::String>("object_coordinates", 1);
  ros::Rate loop_rate(30);

  rs::context _rs_ctx;
  rs::device& _rs_camera = *_rs_ctx.get_device(0);
  initialize_streaming();

    // cv::namedWindow("My window");
  while ( ros::ok() || _rs_camera.is_streamig() )
  {
    std_msgs::String msg;
    std::stringstream ss;
    ss << "Hello world " << count;
    msg.data = ss.str();
    chatter_pub.publish(msg);
    ros::spinOnce();
    loop_rate.sleep();
    _depth_intrin = _rs_camera.get_stream_intrinsics( rs::stream::depth );

    cv::Mat depth_image( _depth_intrin.height, _depth_intrin.width, CV_16U, (uchar *)_rs_camera.get_frame_data( rs::stream::depth ) );

    cv::imshow("Depth Image", depth_image )
    ++count;

  }
  return 0;
}

void initialize_streaming(){

  if( _rs_ctx.get_device_count( ) > 0 )
  {
    //RGB Stream at 60 fps and 640x480 resolution.
    _rs_camera.enable_stream( rs::stream::color, 640, 480, rs::format::rgb8, 30 );
    //Depth stream at 30 fps and 640x480 resolution.
    // _rs_camera.enable_stream( rs::stream::depth, 640, 480, rs::format::z16, 60);
    //Infrared stream at 30 fps and   640x480 resolution.
    //_rs_camera.enable_stream(rs::stream::infrared,640, 480, rs::format::y8,30);
    _rs_camera.start( );

  }
}
