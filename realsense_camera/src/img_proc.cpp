//Including img_proc class header.
#include <realsense_camera/img_proc.h>
#include "std_msgs/String.h"

using namespace cv;
cv::Mat last_frame;//(640, 480, CV_8UC3);


int main (int argc, char **argv){

ros::init(argc, argv, "obj_coord");
ros::NodeHandle n;
ros::Publisher chatter_pub = n.advertise<std_msgs::String>("obj_coord", 1000);
ros::Rate loop_rate(30);
int count = 0;

img_proc ip;
ip.debugMode = argv[1];
if( !ip.initialize_streaming()){
  std::cout << "Unable to locate a camera" <<std::endl;
  rs::log_to_console( rs::log_severity::fatal );
  return EXIT_FAILURE;
}
//cv::namedWindow("Depth Image", 0);
//cv::namedWindow("RGB Image", 0);
//cv::namedWindow("Infrared Image",0);
while( ip._loop && ros::ok()) {
  if( ip._rs_camera.is_streaming( ) ){
    ip._rs_camera.wait_for_frames();

    std_msgs::String msg;
    std::stringstream ss;
    ss << "hello world " << count;
    msg.data = ss.str();
    //ROS_INFO("%s", msg.data.c_str());
    chatter_pub.publish(msg);
    ros::spinOnce();
    loop_rate.sleep();
    ++count;
    ip.display_next_frame();
    //ip.track(ip.display_next_frame());
  }
}
ip._rs_camera.stop();
cv::destroyAllWindows();
return EXIT_SUCCESS;
return 0;
}


bool img_proc::initialize_streaming(){
  bool success = false;
  if( _rs_ctx.get_device_count( ) > 0 )
  {
    //RGB Stream at 60 fps and 640x480 resolution.
    _rs_camera.enable_stream( rs::stream::color, 640, 480, rs::format::rgb8, 30 );
    //Depth stream at 30 fps and 640x480 resolution.
    _rs_camera.enable_stream( rs::stream::depth, 640, 480, rs::format::z16, 60);
    //Infrared stream at 30 fps and   640x480 resolution.
    //_rs_camera.enable_stream(rs::stream::infrared,640, 480, rs::format::y8,30);
    _rs_camera.start( );
    success = true;
  }
  return success;
}

cv::Mat img_proc::display_next_frame(){
  _depth_intrin       = _rs_camera.get_stream_intrinsics( rs::stream::depth );
  _color_intrin       = _rs_camera.get_stream_intrinsics( rs::stream::color );
  //_infrared = _rs_camera.get_stream_intrinsics(rs::stream::infrared);


  // Create depth image
  cv::Mat depth16( _depth_intrin.height,_depth_intrin.width,CV_16U,(uchar *)_rs_camera.get_frame_data( rs::stream::depth) );
  depth_distance = depth16.at<float>(320,240);
  depth_meters =depth_distance * _rs_camera.get_depth_scale();
  //std::cout << depth_distance<<std::endl;
  float2d.x = 320;
  float2d.y = 240;

  int y1 = 220; int x1 = 340;
  const uint16_t * depth_image = (const uint16_t *)_rs_camera.get_frame_data(rs::stream::depth_aligned_to_color);
  float scale = _rs_camera.get_depth_scale();
  uint16_t p1_depth_value = depth_image[y1 * _depth_intrin.width + x1];
  float p1_depth_in_meters = p1_depth_value * scale;
  rs::float2 p1_depth_pixel = {(float)x1, (float)y1};
  rs::float3 p1_depth_point = _depth_intrin.deproject(p1_depth_pixel, p1_depth_in_meters);
  //coord = _depth_intrin.deproject(float2d, depth_meters);
  std::cout <<"X: "<< p1_depth_point.x<<std::endl;
  std::cout<<"Y: " << p1_depth_point.y<<std::endl;
  std::cout <<"Z: "<< p1_depth_point.z<<std::endl;

  // Create color image
  cv::Mat rgb( _color_intrin.height,
  _color_intrin.width,
  CV_8UC3,
  (uchar *)_rs_camera.get_frame_data( rs::stream::color ) );

  // Create infrared image
  //cv::Mat infra(_infrared.height, _infrared.width,CV_8U,(uchar *)_rs_camera.get_frame_data(rs::stream::infrared));
  //imshow("Infrared Image", infra);
  cvWaitKey(1);

  // < 800
  cv::Mat depth8u = depth16;
  depth8u.convertTo( depth8u, CV_8UC3 );//255.0/1000

  imshow( "Depth Image", depth16 );
  cvWaitKey(1);

  cv::Mat rgb_(_color_intrin.height,
                _color_intrin.width,
                CV_8UC3);

  cv::cvtColor( rgb, rgb_, cv::COLOR_BGR2RGB);
  cvWaitKey(1);
  //drawSquare(rgb);
  drawSquare(rgb_);
  imshow( "RGB Image", rgb_ );
  cvWaitKey(1);

  return rgb;
}

void img_proc::drawSquare(cv::Mat input){
  for(int x = 320; x <330 ; x++){
    int y = 240;
    input.at<cv::Vec3b>(y,x)[0] = 0;
    input.at<cv::Vec3b>(y,x)[1] = 255;
    input.at<cv::Vec3b>(y,x)[2] = 0;
  }
  for(int y = 240; y<250; y++){
    int x = 320;
    input.at<cv::Vec3b>(y,x)[0] = 0;
    input.at<cv::Vec3b>(y,x)[1] = 255;
    input.at<cv::Vec3b>(y,x)[2] = 0;
  }
  for(int x = 320; x < 330; x++){
    int y = 250;
    input.at<cv::Vec3b>(y,x)[0] = 0;
    input.at<cv::Vec3b>(y,x)[1] = 255;
    input.at<cv::Vec3b>(y,x)[2] = 0;
  }
  for(int y = 240; y<250; y++){
    int x = 330;
    input.at<cv::Vec3b>(y,x)[0] = 0;
    input.at<cv::Vec3b>(y,x)[1] = 255;
    input.at<cv::Vec3b>(y,x)[2] = 0;
  }
}

void img_proc::track(cv::Mat current_frame){
  if (!first_loop){
    new_frame = current_frame;
    cv::cvtColor(new_frame,nf_grey,cv::COLOR_BGR2GRAY);
    cv::cvtColor(last_frame,lf_grey,cv::COLOR_BGR2GRAY);
    cv::absdiff(nf_grey,lf_grey,difference);
    cv::threshold(difference,threshholding,50,255,CV_THRESH_BINARY);
    cv::blur(threshholding,threshholding,cv::Size(20,20));
    cv::threshold(threshholding,threshholding,50,255,CV_THRESH_BINARY);
    if(debugMode == true){
    imshow("New",new_frame);
    imshow("Last",last_frame);
    imshow("Difference",difference);
    imshow("Threshholded",threshholding);
    }
  }
  last_frame = current_frame;
  first_loop = false;
  cvWaitKey(10);
}
/*
int img_proc::debugger(){

  switch(waitKey(10)){
      case 27:
        return 0;
      case 100:
        debugMode = !debugMode;
        done = !done;
        if(debugMode == false) std::cout<<"Debug mode disabled."<<std::endl;
        else std::cout<<"Debug mode enabled."<<std::endl;
        break;
      case 112:
        pause =!pause;
        if(pause == true){ std::cout<<"Code paused, press 'p' again to resume"<<std::endl;
          while (pause == true){
            //stay in this loop until
            switch (waitKey()){
              case 112:
              //change pause back to false
              pause = false;
              std::cout<<"Code Resumed"<<std::endl;
              break;
            }

          }
        }
      }
  return 1;
}
*/
