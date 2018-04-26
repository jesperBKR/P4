#include <memory>
#include <vector>
#include <iostream>
#include <Eigen/Dense>


#include "ros/ros.h"
#include <std_msgs/String.h>
#include <geometry_msgs/Point32.h>

#include <librealsense/rs.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>

#include "rs/utils/librealsense_conversion_utils.h"

#include "rs_sdk.h"

using namespace std;
using namespace rs::core;
using namespace rs::utils;

#define M_PI 3.14159265358979323846

void get_single_coordinate_on_color_image(std::shared_ptr<image_interface> color_image, vector<pointF32> &color_coordinates);

// void get_color_coordinate_with_depth_value(std::shared_ptr<image_interface> color_image, std::shared_ptr<image_interface> depth_image, vector<point3dF32> &color_coordinate_with_depth);

int main (int argc, char **argv)
{
    ros::init(argc, argv, "realsense_node");
    ros::NodeHandle nh;

    ros::Publisher chatter_pub = nh.advertise<std_msgs::String>("realsense_node", 1);
    ros::Publisher position_pub = nh.advertise<geometry_msgs::Point32>("object_pos", 1);
    ros::Rate loop_rate(30);

    // Create a window for our test Control
    const char* control_window = "HSV - Control";
    cv::namedWindow(control_window, 0);

    int lowHue = 34;
    int highHue = 63;

    int lowSat = 202;
    int highSat = 255;

    int lowVal = 137;
    int highVal = 255;

    const int maxThreshold = 100;
    int cannyThreshold  = 0;
    // Canny thresholds input with a ratio 1:3
    int ratio = 3;

    //Create trackbars in "HSV - Control" window
    //Hue (0 - 179)
    cvCreateTrackbar("Low Hue", control_window, &lowHue, 179);
    cvCreateTrackbar("High Hue", control_window, &highHue, 179);

    // Saturation (0 - 255)
    cvCreateTrackbar("Low Saturation", control_window, &lowSat, 255);
    cvCreateTrackbar("High Saturation", control_window, &highSat, 255);

    // Value (0 - 255)
    cvCreateTrackbar("Low Value", control_window, &lowVal, 255);
    cvCreateTrackbar("High Value", control_window, &highVal, 255);

    // Canny low threshold (0 - 100)
    cvCreateTrackbar("Canny low threshold", control_window, &cannyThreshold, maxThreshold);

    int count = 0;
    rs_error *rs_error_ = NULL;
    rs::context context;
    if(context.get_device_count() == 0)
    {
        cerr<<"No Realsense device detected" << endl;
        return -1;
    }

    rs::device* device = context.get_device(0);
    rs_device* rs_device;
    const cv::String color_win_name = "Color Image";
    const cv::String depth_win_name = "Depth Image";

    //color profile
    int color_width = 640, color_height = 480, color_fps = 60, color_pixel_size = 3;
    const rs::format color_format = rs::format::rgb8;

    //depth profile
    int depth_width = 628, depth_height = 468, depth_fps = 60, depth_pixel_size = 2;
    const rs::format depth_format = rs::format::z16;

    device->enable_stream(rs::stream::color, color_width, color_height, color_format, color_fps);
    device->enable_stream(rs::stream::depth, depth_width, depth_height, depth_format, depth_fps);

    device->start();

    while ( ros::ok() || device->is_streaming() )
    {
      // std_msgs::String msg;
      // std::stringstream ss;
      // ss << "Hello world " << count;
      // msg.data = ss.str();
      // chatter_pub.publish(msg);
      // ros::spinOnce();
      // count++;

      intrinsics color_intrin = convert_intrinsics(device->get_stream_intrinsics(rs::stream::color));
      intrinsics depth_intrin = convert_intrinsics(device->get_stream_intrinsics(rs::stream::depth));
      extrinsics extrin = convert_extrinsics(device->get_extrinsics(rs::stream::depth, rs::stream::color));
      // rs_extrinsics depth2color_extrinsic;
      // rs_get_device_extrinsics(rs_device, RS_STREAM_COLOR, RS_STREAM_DEPTH, &depth2color_extrinsic, &rs_error_);

      auto projection_ = rs::utils::get_unique_ptr_with_releaser(projection_interface::create_instance(&color_intrin, &depth_intrin, &extrin));

      device->wait_for_frames();

      image_info  color_info;
      color_info.width = color_width;
      color_info.height = color_height;
      color_info.format = rs::utils::convert_pixel_format(color_format);
      color_info.pitch = color_pixel_size * color_width;

      std::shared_ptr<image_interface> color_image = get_shared_ptr_with_releaser(image_interface::create_instance_from_raw_data(
                                                                                     &color_info,
                                                                                     {device->get_frame_data(rs::stream::color), nullptr},
                                                                                     stream_type::color,
                                                                                     image_interface::flag::any,
                                                                                     device->get_frame_timestamp(rs::stream::color),
                                                                                     device->get_frame_number(rs::stream::color)));

      image_info  depth_info;
      depth_info.width = depth_width;
      depth_info.height = depth_height;
      depth_info.format = rs::utils::convert_pixel_format(depth_format);
      depth_info.pitch = depth_pixel_size * depth_width;

      std::shared_ptr<image_interface> depth_image = get_shared_ptr_with_releaser(image_interface::create_instance_from_raw_data(
                                                                                     &depth_info,
                                                                                     {device->get_frame_data(rs::stream::depth), nullptr},
                                                                                     stream_type::depth,
                                                                                     image_interface::flag::any,
                                                                                     device->get_frame_timestamp(rs::stream::depth),
                                                                                     device->get_frame_number(rs::stream::depth)));




      device->wait_for_frames();

      cv::Mat _depth_image(depth_intrin.height, depth_intrin.width, CV_16U , (void *)device->get_frame_data( rs::stream::depth ), cv::Mat::AUTO_STEP);
      cv::Mat _color_image(color_intrin.height, color_intrin.width, CV_8UC3, (void *)device->get_frame_data( rs::stream::color ), cv::Mat::AUTO_STEP);

      cv::Mat depth8 = _depth_image;
      depth8.convertTo( depth8, CV_8UC1, 255.0/1000 );
      // cv::Mat frame(color_intrin.height, color_intrin.width, CV_8UC3, (uchar *)device->get_frame_data( rs::frame ));

      cv::Mat hsvImage;
      // Change the incoming frame to RGB
      // cv::cvtColor( _color_image, _color_image, cv::COLOR_BGR2RGB);
      cv::cvtColor( _color_image, hsvImage, cv::COLOR_BGR2HSV);

      cv::Mat thresholdImage;

      cv::inRange(hsvImage, cv::Scalar(lowHue, lowSat, lowVal), cv::Scalar(highHue, highSat, highVal), thresholdImage);

      // Create a vector to hold our color coordinates
      vector<pointF32> color_coordinates;

      // get_single_coordinate_on_color_image(color_image, color_coordinates);

      // Find the
      // cv::Mat binaryImage(color_intrin.height, color_intrin.width, CV_8UC1);
      // for(int x = 0; x < _color_image.cols; x++){
      //   for(int y = 0; y < _color_image.rows; y++){
      //     if(( _color_image.at<cv::Vec3b>(y,x)[0] < 60
      //       && _color_image.at<cv::Vec3b>(y,x)[1] > 80
      //       && _color_image.at<cv::Vec3b>(y,x)[2] < 70 ) ||
      //        ( _color_image.at<cv::Vec3b>(y,x)[0] < 20
      //       && _color_image.at<cv::Vec3b>(y,x)[1] > 30
      //       && _color_image.at<cv::Vec3b>(y,x)[2] < 20 ))
      //     {
      //       binaryImage.at<uchar>(y, x) = 255;
      //     } else {
      //       binaryImage.at<uchar>(y, x) = 0;
      //     }
      //   }
      // }
      // if(!binaryImage.empty())
      // {
      //   cv::Mat element = cv::getStructuringElement( cv::MORPH_RECT , cv::Size(9, 9), cv::Point(-1, -1) );
      //
      //   cv::morphologyEx(binaryImage, binaryImage, cv::MORPH_OPEN, element, cv::Point(-1, -1));
      //   // cv::morphologyEx(binaryImage, binaryImage, cv::MORPH_CLOSE, element, cv::Point(-1, -1));
      //
      //   std::vector<std::vector<cv::Point> > contours;
      //   std::vector<cv::Vec4i> hierarchy;
      //
      //   cv::findContours(binaryImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
      //
      //   std::vector<cv::RotatedRect> minRect( contours.size() );
      //
      //   for( unsigned int i = 0; i < contours.size(); i++)
      //   {
      //     minRect[i] = cv::minAreaRect( cv::Mat(contours[i]) );
      //   }
      //   if(!minRect.empty())
      //   {
      //     cv::Scalar rotatedColor( 255, 0, 0);
      //     for (unsigned int i = 0; i <= contours.size(); i++)
      //     {
      //       cv::Point2f rect_points[4]; minRect[i].points( rect_points );
      //       for( int j = 0; j < 4; j++ )
      //       cv::line( binaryImage, rect_points[j], rect_points[(j+1)%4], rotatedColor, 1, 8);
      //
      //       pointF32 coordinate;
      //       coordinate.x = minRect[i].center.x;
      //       coordinate.y = minRect[i].center.y;
      //       color_coordinates.push_back(coordinate);
      //
      //     }
      //   }
      // }
      cv::Mat edges;

      if(!thresholdImage.empty())
      {
        cv::Mat element = cv::getStructuringElement( cv::MORPH_RECT , cv::Size(9, 9), cv::Point(-1, -1) );

        // Morphological opening (remove small objects)
        cv::morphologyEx(thresholdImage, thresholdImage, cv::MORPH_OPEN, element, cv::Point(-1, -1));

        // Morphological closing (fill small hloes)
        cv::morphologyEx(thresholdImage, thresholdImage, cv::MORPH_CLOSE, element, cv::Point(-1, -1));

        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;


        // Maybe add edge here and use that for contours
        cv::Canny(thresholdImage, edges, cannyThreshold, cannyThreshold*ratio, 3);

        cv::findContours(edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

        std::vector<cv::RotatedRect> minRect( contours.size() );

        // for( unsigned int i = 0; i < contours.size(); i++)
        // {
        //   minRect[i] = cv::minAreaRect( cv::Mat(contours[i]) );
        // }
        int largest_area = 0;
        unsigned int largest_contour_index = 0;
        for( unsigned int i = 0; i < contours.size(); i++)
        {
          double a = cv::contourArea( contours[i], false);
          if(a>largest_area)
          {
            largest_area = a;
            largest_contour_index = i;
            minRect[i] = cv::minAreaRect( cv::Mat(contours[i]) );
          }
        }

        if(!minRect.empty())
        {
          cv::Scalar rotatedColor( 255, 0, 0);
          // for (unsigned int i = 0; i <= contours.size(); i++)
          // {
            cv::Point2f rect_points[4]; minRect[largest_contour_index].points( rect_points );
            for( int j = 0; j < 4; j++ )
            cv::line( edges, rect_points[j], rect_points[(j+1)%4], rotatedColor, 1, 8);

            pointF32 coordinate;
            coordinate.x = minRect[largest_contour_index].center.x;
            coordinate.y = minRect[largest_contour_index].center.y;
            color_coordinates.push_back(coordinate);

          // }
        }
      }

      cv::namedWindow(color_win_name, cv::WINDOW_AUTOSIZE);
      cv::namedWindow(depth_win_name, cv::WINDOW_AUTOSIZE);

      cv::imshow(color_win_name, _color_image);
      cv::imshow(depth_win_name, depth8);
      // cv::imshow("Binary: ",binaryImage);
      cv::imshow("Thresholded Image", edges);

      if(!color_coordinates.empty())
      {
        vector<pointF32> mapped_dDepth_coordinates(color_coordinates.size());
        if(projection_->map_color_to_depth(depth_image.get(), static_cast<int32_t>(color_coordinates.size()), color_coordinates.data(), mapped_dDepth_coordinates.data()) < status_no_error)
        {
          cerr<<"failed to map the color coordinates to depth" << endl;
          return -1;
        }
        // geometry_msgs::Point32 position_msg;
        for(size_t i = 0; i < color_coordinates.size(); i++)
        {
          if(mapped_dDepth_coordinates[i].x < 0 || mapped_dDepth_coordinates[i].y < 0){
            cerr << "No depth data found! " << endl;
          } else
          {
            auto depth_image_info = depth_image->query_info();
            vector<point3dF32> vertices(depth_image_info.width * depth_image_info.height);
            point3dF32 translate_object;
            if(projection_->query_vertices(depth_image.get(), vertices.data()) < status_no_error)
            {
              cerr<<"failed to query vertices" << endl;
              return -1;
            }
            // World coordinates from Verticies in mm from depth frame.
            translate_object.x = vertices[static_cast<int32_t>(mapped_dDepth_coordinates[i].y) * depth_image_info.width + static_cast<int32_t>(mapped_dDepth_coordinates[i].x)].x;
            translate_object.y = vertices[static_cast<int32_t>(mapped_dDepth_coordinates[i].y) * depth_image_info.width + static_cast<int32_t>(mapped_dDepth_coordinates[i].x)].y;
            translate_object.z = vertices[static_cast<int32_t>(mapped_dDepth_coordinates[i].y) * depth_image_info.width + static_cast<int32_t>(mapped_dDepth_coordinates[i].x)].z;

            cout << "Vert. Coordinate X: " << translate_object.x << endl;
            cout << "Vert. Coordinate Y: " << translate_object.y << endl;
            cout << "Vert. Coordinate Z: " << translate_object.z << endl;



            // Robot to camera Transform, rotaion of Y, Z, and the translation
            Eigen::Matrix4f tr_robot_camera, rot_robot2camera_y, rot_robot2camera_z, transl_robot_camera;

            // Camera to object, we only need the translation in a transformation matrix
            Eigen::Matrix4f tr_camera_object;

            // Robot to object Transformation matrix
            Eigen::Matrix4f tr_robot_object;

            // Point to store the transform from the robot to the camera
            point3dF32 translate_camera;
            translate_camera.x = 0.1;
            translate_camera.y = 0.1;
            translate_camera.z = 0.1;


            // Transform from depth to color frame
            // ros::Time transform_ts_;
            // geometry_msgs::TransformStamped b2d_msg;
            // b2d_msg.header.stamp = transform_ts_; // time of tramsform
            // b2d_msg.transform.translation.x =  depth2color_extrinsic.translation[2];
            // b2d_msg.transform.translation.y =  depth2color_extrinsic.translation[0]; // it is plus because we move in that direction
            // b2d_msg.transform.translation.z =  depth2color_extrinsic.translation[1];

            // Get the translation and rotation from the robot to the camera into
            // a transformation matrix
            float theta = 90 * M_PI/180;
            rot_robot2camera_y << cos(theta), 0, sin(theta),  0,
                                      0,      1,     0,       0,
                                  -sin(theta),0, cos(theta),  0,
                                      0,      0,      0,      1;

            rot_robot2camera_z << cos(theta),  sin(theta),  0,  0,
                                  -sin(theta), cos(theta),  0,  0,
                                        0,         0,       0,  0,
                                        0,         0,       0,  1;

            transl_robot_camera << 1, 0, 0, translate_camera.x,
                                   0, 1, 0, translate_camera.y,
                                   0, 0, 1, translate_camera.z,
                                   0, 0, 0,         1;

            tr_robot_camera = rot_robot2camera_y*rot_robot2camera_z*transl_robot_camera;

            // Transformation matrix from the camera to the object
            tr_camera_object << 1, 0, 0, translate_object.x/1000,
                                0, 1, 0, translate_object.y/1000,
                                0, 0, 1, translate_object.z/1000,
                                0, 0, 0,      1;

            tr_robot_object = tr_robot_camera*tr_camera_object;
            geometry_msgs::Point32 translate_object_msg;
            translate_object_msg.x = tr_robot_object(0,3);
            translate_object_msg.y = tr_robot_object(1,3);
            translate_object_msg.z = tr_robot_object(2,3);

            position_pub.publish(translate_object_msg);
          }
        }
      }

      if(cv::waitKey(30) >= 0)
      {
        device->stop();
        if(!device->is_streaming())
          break;
      }
      ros::spinOnce();
    } // end of for

    return 0;
} // end of main

void get_single_coordinate_on_color_image(std::shared_ptr<image_interface> color_image, vector<pointF32> &color_coordinate)
{

  if(!color_image)
  {
    cerr<<"Can't use null image" << endl;
    return;
  }

  auto color_image_info = color_image->query_info();

  const int pointX = color_image_info.width / 2;
  const int pointY = color_image_info.height / 2;

  pointF32 coordinate;
  coordinate.x = static_cast<float>(pointX);
  coordinate.y = static_cast<float>(pointY);
  color_coordinate.push_back(coordinate);
}
