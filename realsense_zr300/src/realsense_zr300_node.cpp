#include <memory>
#include <vector>
#include <iostream>
#include <eigen3/Eigen/Dense>
#include <math.h>

#include <ros/ros.h>
#include <geometry_msgs/Point32.h>
#include "rupee_msgs/camera.h"

#include <librealsense/rs.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>

#include <rs/utils/librealsense_conversion_utils.h>

#include <rs_sdk.h>

using namespace std;
using namespace rs::core;
using namespace rs::utils;

#define M_PI 3.14159265358979323846

void condefects(std::vector<cv::Vec4i> convexityDefectsSet, std::vector<cv::Point> mycontour, cv::Mat &original);
float distP2P(cv::Point a, cv::Point b);
float getAngle(cv::Point s, cv::Point f, cv::Point e);

int main (int argc, char **argv)
{
    // Initialize ROS
    ros::init(argc, argv, "realsense_node");
    ros::NodeHandle nh;
    ros::Publisher obj_position_pub = nh.advertise<rupee_msgs::camera>("RS_object", 1);
    ros::Publisher hand_position_pub = nh.advertise<rupee_msgs::camera>("hand_position", 1);
    ros::Rate loop_rate(30);

/*        HSV GUI for Object          */
    // Values for HSV threshold
    int lowHue = 25; //23
    int highHue = 90; //82

    int lowSat = 106; //104
    int highSat = 255;

    int lowVal = 0; //41
    int highVal = 255; //114

    const int maxThreshold = 100;
    int cannyThreshold  = 0;
    // Canny thresholds input with a ratio 1:3
    int ratio = 3;

    // Create a window for our test Control
    const char* control_window = "HSV - Object - Control";
    cv::namedWindow(control_window, CV_WINDOW_KEEPRATIO);

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

/*        HSV GUI for Hand          */
    // Values for HSV threshold
    int lowHandHue = 108; //93, 97
    int highHandHue = 156; //172

    int lowHandSat = 12; //0, 45
    int highHandSat = 202; //255, 255

    int lowHandVal = 118; //118, 31
    int highHandVal = 255; //255, 176

    // Create a window for our test Control
    const char* hsv_window = "HSV - Hand - Controls";
    cv::namedWindow(hsv_window, CV_WINDOW_KEEPRATIO);

    //Create trackbars in "HSV - Control" window
    cvCreateTrackbar("Low Hue", hsv_window, &lowHandHue, 179);
    cvCreateTrackbar("High Hue", hsv_window, &highHandHue, 179);
    cvCreateTrackbar("Low Saturation", hsv_window, &lowHandSat, 255);
    cvCreateTrackbar("High Saturation", hsv_window, &highHandSat, 255);
    cvCreateTrackbar("Low Value", hsv_window, &lowHandVal, 255);
    cvCreateTrackbar("High Value", hsv_window, &highHandVal, 255);

    // error message used for extrinsic parameters
    // rs_error *rs_error_ = NULL;
    rs::context context;
    if(context.get_device_count() == 0)
    {
        cerr<<"No Realsense device detected" << endl;
        return -1;
    }

    rs::device* device = context.get_device(0);
    // This instance of rs_device was put here to get the extrinsics of the camera
    // rs_device* rs_device;
    const cv::String color_win_name = "Color Image with object detection";
    const cv::String depth_win_name = "Depth Image";

    //color profile
    int color_width = 640, color_height = 480, color_fps = 60;
    const rs::format color_format = rs::format::rgb8;

    //depth profile
    int depth_width = 628, depth_height = 468, depth_fps = 60, depth_pixel_size = 2;
    const rs::format depth_format = rs::format::z16;

    device->enable_stream(rs::stream::color, color_width, color_height, color_format, color_fps);
    device->enable_stream(rs::stream::depth, depth_width, depth_height, depth_format, depth_fps);

    device->start();

    while ( ros::ok() || device->is_streaming() )
    {
      intrinsics color_intrin = convert_intrinsics(device->get_stream_intrinsics(rs::stream::color));
      intrinsics depth_intrin = convert_intrinsics(device->get_stream_intrinsics(rs::stream::depth));
      extrinsics extrin = convert_extrinsics(device->get_extrinsics(rs::stream::depth, rs::stream::color));
      // Test to get the extrinsics, not used now, can be used to get distance from depth to color frame
      // rs_extrinsics depth2color_extrinsic;
      // rs_get_device_extrinsics(rs_device, RS_STREAM_COLOR, RS_STREAM_DEPTH, &depth2color_extrinsic, &rs_error_);

      auto projection_ = rs::utils::get_unique_ptr_with_releaser(projection_interface::create_instance(&color_intrin, &depth_intrin, &extrin));

      device->wait_for_frames();

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

      cv::cvtColor( _color_image, _color_image, cv::COLOR_BGR2RGB);

      // Create a vector to hold our hand coordinates
      vector<pointF32> hand_coordinates;

      // Find a hand in the image
      cv::Mat handImage;
      cv::cvtColor( _color_image, handImage, cv::COLOR_RGB2HSV);
      cv::Mat handThreshold(handImage.rows, handImage.cols, CV_8UC1);
      cv::inRange( handImage, cv::Scalar(lowHandHue, lowHandSat, lowHandVal), cv::Scalar(highHandHue, highHandSat, highHandVal), handThreshold);
      cv::Mat morph1(handImage.rows, handImage.cols, CV_8UC1);
      cv::Mat morph2(handImage.rows, handImage.cols, CV_8UC1);
      cv::Mat handEdges;
      if(!handThreshold.empty())
      {
          cv::Mat square_element = cv::getStructuringElement( cv::MORPH_RECT, cv::Size(11, 11), cv::Point(-1, -1) );
          cv::Mat ellipse_element = cv::getStructuringElement( cv::MORPH_ELLIPSE, cv::Size(5, 5));

          cv::dilate(handThreshold, morph1, ellipse_element);
          cv::erode(morph1, morph1, square_element);
          cv::dilate(morph1, morph1, ellipse_element);
          cv::medianBlur(morph1, morph1, 5);

          cv::Mat ellipse2_element = cv::getStructuringElement( cv::MORPH_ELLIPSE, cv::Size(8, 8));
          cv::dilate(morph1, morph2, ellipse2_element);
          cv::medianBlur(morph2, morph2, 5);
          cv::threshold(morph2, morph2, 127, 255, 0);
          //
          // // Morphological opening (remove small objects)
          // cv::morphologyEx(handThreshold, morph1, cv::MORPH_OPEN, element, cv::Point(-1, -1));
          // // Morphological closing (fill small hloes)
          // cv::morphologyEx(morph1, morph2, cv::MORPH_CLOSE, element, cv::Point(-1, -1));

          std::vector<std::vector<cv::Point> > handContours;
          std::vector<cv::Vec4i> hierarchy;

          cv::findContours(morph2, handContours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

          int largest_area = 0;
          uint lcIdx = 0;
          for( uint i = 0; i < handContours.size(); i++)
          {
            // Get the size of the contour
            double a = cv::contourArea( handContours[i], false);

            // Catch the biggest contour
            if(a>largest_area)
            {
              largest_area = a;
              lcIdx = i;
            }
          }
          // Find the convex hull, contours and defects for each contour
          std::vector<std::vector<cv::Point> >hull(handContours.size());
          std::vector<std::vector<int> >inthull(handContours.size());
          std::vector<std::vector<cv::Vec4i> >defects(handContours.size());

          if(!handContours.empty())
          {
            cv::Rect dRect = cv::boundingRect(cv::Mat(handContours[lcIdx]));
            cv::convexHull(cv::Mat(handContours[lcIdx]), hull[lcIdx], false, true);
            cv::convexHull(cv::Mat(handContours[lcIdx]), inthull[lcIdx], false, false);
            cv::approxPolyDP(cv::Mat(hull[lcIdx]), hull[lcIdx], 18, true);
            std::vector<cv::Vec4i> newDefects;

            // Find the defects and eliminate unwanted
            if(handContours[lcIdx].size()>3)
            {
              cv::convexityDefects(handContours[lcIdx], inthull[lcIdx], defects[lcIdx]);
              int tolerance = dRect.height/5;
              float angleTol = 95;
              std::vector<cv::Vec4i>::iterator d = defects[lcIdx].begin();
              while( d!=defects[lcIdx].end() )
              {
                cv::Vec4i v =(*d);

                int startIdx = v[0]; cv::Point ptStart(handContours[lcIdx][startIdx]);
                int endIdx = v[1]; cv::Point ptEnd(handContours[lcIdx][endIdx]);
                int farIdx = v[2]; cv::Point ptFar(handContours[lcIdx][farIdx]);

                if(distP2P(ptStart, ptFar) > tolerance && distP2P(ptEnd, ptFar) > tolerance && getAngle(ptStart, ptFar, ptEnd) < angleTol )
                {
                  if(ptEnd.y > (dRect.y + dRect.height - dRect.height/4)){
                  }else if(ptStart.y > (dRect.y + dRect.height - dRect.height/4)){
                  }else {
                    newDefects.push_back(v);
                  }
                }
                d++;
              }
            }
            defects[lcIdx].swap(newDefects);

            // Next we romove redundant defecets
            int startidx, endidx;
            int startidx2, endidx2;
            float tolerance = dRect.width/6;
            for(uint i = 0; i < newDefects.size(); i++)
            {
              for(uint j = 0; j <newDefects.size(); j++)
              {
                startidx = newDefects[i][0]; cv::Point ptStart(handContours[lcIdx][startidx]);
                endidx = newDefects[i][1]; cv::Point ptEnd(handContours[lcIdx][endidx]);
                startidx2 = newDefects[j][0]; cv::Point ptStart2(handContours[lcIdx][startidx2]);
                endidx2 = newDefects[j][1]; cv::Point ptEnd2(handContours[lcIdx][endidx2]);
                if(distP2P(ptStart, ptEnd2) < tolerance )
                {
                  handContours[lcIdx][startidx] = ptEnd2;
                  break;
                }if(distP2P(ptEnd, ptStart2) < tolerance)
                {
                  handContours[lcIdx][startidx2] = ptEnd;
                }
              }
            }

            double h = dRect.height;
            double w = dRect.width;

            bool isHand = true;
            // Todo add amount of fingers
            // if(count of fingers > 5)
            // false

            if(h == 0 || w == 0){
              isHand = false;
            }else if(h/w > 4 || w/h > 4){
              isHand = false;
            }else if(dRect.x<20){
              isHand = false;
            }

            // If the hand is the largest contour we draw the contours
            if(isHand)
            {
              cv::Moments mu;
              // Find moments of the largest contour
              mu = cv::moments(cv::Mat(handContours[lcIdx]));

              ///  Get the mass center:
              cv::Point2f mc;
              mc = cv::Point2f( mu.m10/mu.m00 , mu.m01/mu.m00 );

              // Draw the hand contour
              cv::drawContours( _color_image, handContours, lcIdx, cv::Scalar(0, 0, 255), 2, 8, hierarchy );
              // Draw a rectangle around the hand
              cv::rectangle( _color_image, dRect.tl(), dRect.br(), cv::Scalar(200, 200, 0));
              // Draw the hull around the hand
              cv::drawContours( _color_image, hull, lcIdx, cv::Scalar(0, 255, 0), 3, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
              // Draw a circle at the center of mass
              cv::circle( _color_image, mc, 4, cv::Scalar(255, 0, 0), -1, 8, 0 );
              std::vector<cv::Vec4i>::iterator d = defects[lcIdx].begin();

              while(d!=defects[lcIdx].end()) // Should there be a +1? end+1
              {
                cv::Vec4i v=(*d);
                int startIdx = v[0]; cv::Point ptStart(handContours[lcIdx][startIdx]);
                int endIdx = v[1]; cv::Point ptEnd(handContours[lcIdx][endIdx]);
                int farIdx = v[2]; cv::Point ptFar(handContours[lcIdx][farIdx]);
                // Draw circles at the fingertips
                cv::circle( _color_image, ptEnd, 9, cv::Scalar(0, 200, 200));
                d++;
              }

              pointF32 hc;
              hc.x = mc.x;
              hc.y = mc.y;
              hand_coordinates.push_back(hc);
              // condefects(defects[lcIdx], handContours[lcIdx], _color_image);
            } //  If the largest contour is a hand then we find the color coordinates
          } // if hand contours is not empty
      } // if hand threshold is not empty

      /*     Hand Detction End    */

      // Find our training object
      // Change the incoming frame to HSV
      cv::Mat objectImage(_color_image.rows, _color_image.cols, CV_8UC3);
      cv::cvtColor( _color_image, objectImage, cv::COLOR_RGB2HSV);
      cv::Mat thresholdImage(objectImage.rows, objectImage.cols, CV_8UC1);
      cv::inRange(objectImage, cv::Scalar(lowHue, lowSat, lowVal), cv::Scalar(highHue, highSat, highVal), thresholdImage);

      // This was moved up, waiting for a green light to delete
      // // Create a vector to hold our color coordinates
      // vector<pointF32> hand_coordinates;
      // Create a vector to hold our color coordinates
      vector<pointF32> color_coordinates;
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

        int largest_area = 0;
        uint lcIdx = 0;
        for( uint i = 0; i < contours.size(); i++)
        {
          // Get the size of the contour
          double a = cv::contourArea( contours[i], false);

          cv::Rect boudRect = cv::boundingRect(cv::Mat(contours[i]));
          bool isCube = true;
          double h = boudRect.height;
          double w = boudRect.width;

          if (h == 0 || w == 0) {
            isCube = false;
          }else if(h/w > 1.5 || w/h > 1.5){
            isCube = false;
          }//else if(h/w < 0.5 || w/h < 0.5){
            //isCube = false;
          //}
          // Catch the biggest contour
          if(a>largest_area && isCube)
          {
            largest_area = a;
            lcIdx = i;
            minRect[i] = cv::minAreaRect( cv::Mat(contours[i]) );
          }
        }


        if(!minRect.empty())
        {
          cv::Scalar rotatedColor( 255, 0, 0);
          // for (unsigned int i = 0; i <= contours.size(); i++)
          // {
          cv::Point2f rect_points[4]; minRect[lcIdx].points( rect_points );
          for( int j = 0; j < 4; j++ )
          cv::line( _color_image, rect_points[j], rect_points[(j+1)%4], rotatedColor, 1, 8);

          pointF32 coordinate;
          coordinate.x = minRect[lcIdx].center.x;
          coordinate.y = minRect[lcIdx].center.y;
          color_coordinates.push_back(coordinate);

          // }
        }
      }

      // Change the color image to RGB before showing it
      // cv::cvtColor( _color_image, _color_image, cv::COLOR_BGR2RGB);

      cv::namedWindow(color_win_name, cv::WINDOW_AUTOSIZE);
      // cv::namedWindow(depth_win_name, cv::WINDOW_AUTOSIZE);

      cv::imshow(color_win_name, _color_image);
      // cv::imshow(depth_win_name, depth8);
      // cv::imshow("Thresholded Image", edges);

      // Hand detection images
      // cv::imshow("Hand detection", handThreshold);
      // cv::imshow("Morph open", morph1);
      // cv::imshow("Morph close", morph2);

      point3dF32 object_position;
      rupee_msgs::camera object_position_msg;
      object_position_msg.detected.data = false;
      if(!color_coordinates.empty())
      {
        vector<pointF32> mapped_depth_coordinates(color_coordinates.size());
        if(projection_->map_color_to_depth(depth_image.get(), static_cast<int32_t>(color_coordinates.size()), color_coordinates.data(), mapped_depth_coordinates.data()) < status_no_error)
        {
          cerr<<"failed to map the color coordinates to depth" << endl;
          return -1;
        }

        for(size_t i = 0; i < color_coordinates.size(); i++)
        {

          // If the object coordinates are greater than 0 we get the global coordinates
          if(mapped_depth_coordinates[i].x < 0 || mapped_depth_coordinates[i].y < 0 ){
            // ROS_INFO("coordinates size: %d",  mapped_depth_coordinates.size());
          }else{
            auto depth_image_info = depth_image->query_info();
            vector<point3dF32> vertices(depth_image_info.width * depth_image_info.height);
            if(projection_->query_vertices(depth_image.get(), vertices.data()) < status_no_error)
            {
              cerr<<"failed to query vertices" << endl;
              return -1;
            }
            object_position.x = vertices[static_cast<int32_t>(mapped_depth_coordinates[i].y) * depth_image_info.width + static_cast<int32_t>(mapped_depth_coordinates[i].x)].x;
            object_position.y = vertices[static_cast<int32_t>(mapped_depth_coordinates[i].y) * depth_image_info.width + static_cast<int32_t>(mapped_depth_coordinates[i].x)].y;
            object_position.z = vertices[static_cast<int32_t>(mapped_depth_coordinates[i].y) * depth_image_info.width + static_cast<int32_t>(mapped_depth_coordinates[i].x)].z;

            cout << "Object Coordinate X: " << object_position.x << endl;
            cout << "Object Coordinate Y: " << object_position.y << endl;
            cout << "Object Coordinate Z: " << object_position.z << endl;
            }

            // Robot to camera Transform, rotaion of Y, Z, and the translation
            Eigen::Matrix4f tr_camera_robot, rot_camera2robot_y, rot_camera2robot_z, transl_robot_camera;
            Eigen::Matrix4f rot_camera2robot_x;

            // Camera to object, we only need the translation in a transformation matrix
            Eigen::Matrix4f tr_camera_object, rot_camera_y;

            // Robot to object Transformation matrix
            Eigen::Matrix4f tr_robot_object;

            // Point to store the transform from the robot to the camera
            point3dF32 translate_camera;
            translate_camera.x = - 0.54;
            translate_camera.y = 0;
            translate_camera.z = 0.02;

            // Transform from depth to color frame
            // ros::Time transform_ts_;
            // geometry_msgs::TransformStamped b2d_msg;
            // b2d_msg.header.stamp = transform_ts_; // time of tramsform
            // b2d_msg.transform.translation.x =  depth2color_extrinsic.translation[2];
            // b2d_msg.transform.translation.y =  depth2color_extrinsic.translation[0];
            // b2d_msg.transform.translation.z =  depth2color_extrinsic.translation[1];

            // Get the translation and rotation from the robot to the camera into
            // a transformation matrix
            float theta_90 = 90 * M_PI/180;
            float theta_35 = 35 * M_PI/180;

            rot_camera2robot_z << cos(theta_90),  -sin(theta_90),   0,  0,
                                  sin(theta_90),  cos(theta_90),    0,  0,
                                        0,               0,         1,  0,
                                        0,               0,         0,  1;

            rot_camera2robot_x << 1,         0,              0,          0,
                                  0,  cos(-theta_90),  -sin(-theta_90),  0,
                                  0,  sin(-theta_90),   cos(-theta_90),  0,
                                  0,         0,              0,          1;

            // Rotation for the angle of the camera
            rot_camera_y << cos(-theta_35),  0, sin(-theta_35),  0,
                                  0,         1,      0,          0,
                            -sin(-theta_35), 0, cos(-theta_35),  0,
                                  0,         0,       0,         1;

            tr_camera_object << 1, 0, 0, object_position.x/1000,
                                0, 1, 0, object_position.y/1000,
                                0, 0, 1, object_position.z/1000,
                                0, 0, 0,         1;

            transl_robot_camera << 1, 0, 0, translate_camera.x,
                                   0, 1, 0, translate_camera.y,
                                   0, 0, 1, translate_camera.z,
                                   0, 0, 0,         1;

            tr_camera_robot = (((transl_robot_camera*rot_camera2robot_z*rot_camera2robot_z)*rot_camera2robot_x)*rot_camera_y)*tr_camera_object;

            // rupee_msgs::camera object_position_msg;
            object_position_msg.location.x = tr_camera_robot(0,3);
            object_position_msg.location.y = tr_camera_robot(1,3);
            object_position_msg.location.z = tr_camera_robot(2,3);
            object_position_msg.detected.data = true;
            // obj_position_pub.publish(object_position_msg);

          // If we only get the hand coordinates then we just publish them
        }
      }
        point3dF32 hand_position;
        rupee_msgs::camera hand_coord_msg;
        hand_coord_msg.detected.data = false;
        if( !hand_coordinates.empty() ){
          vector<pointF32> mapped_depth_coordinates(hand_coordinates.size());
          if(projection_->map_color_to_depth(depth_image.get(), static_cast<int32_t>(hand_coordinates.size()), hand_coordinates.data(), mapped_depth_coordinates.data()) < status_no_error)
          {
            cerr<<"failed to map the color coordinates to depth" << endl;
            return -1;
          }

          for(size_t i = 0; i < hand_coordinates.size(); i++)
          {
            // If the hand coordinates are greater than 0 we get the global coordinates
            if(mapped_depth_coordinates[i].x < 0 || mapped_depth_coordinates[i].y < 0 ){
              // ROS_INFO("coordinates size: %d",  mapped_depth_coordinates.size());
            }else{
              auto depth_image_info = depth_image->query_info();
              vector<point3dF32> vertices(depth_image_info.width * depth_image_info.height);
              if(projection_->query_vertices(depth_image.get(), vertices.data()) < status_no_error)
              {
                cerr<<"failed to query vertices" << endl;
                return -1;
              }
              hand_position.x = vertices[static_cast<int32_t>(mapped_depth_coordinates[i].y) * depth_image_info.width + static_cast<int32_t>(mapped_depth_coordinates[i].x)].x;
              hand_position.y = vertices[static_cast<int32_t>(mapped_depth_coordinates[i].y) * depth_image_info.width + static_cast<int32_t>(mapped_depth_coordinates[i].x)].y;
              hand_position.z = vertices[static_cast<int32_t>(mapped_depth_coordinates[i].y) * depth_image_info.width + static_cast<int32_t>(mapped_depth_coordinates[i].x)].z;

              cout << "Hand Coordinate X: " << hand_position.x << endl;
              cout << "Hand Coordinate Y: " << hand_position.y << endl;
              cout << "Hand Coordinate Z: " << hand_position.z << endl;
              }
            // Hand world coordinates from Verticies in mm from depth frame.
            // rupee_msgs::camera hand_coord_msg;
            hand_coord_msg.location.x = hand_position.x;
            hand_coord_msg.location.y = hand_position.y;
            hand_coord_msg.location.z = hand_position.z;
            hand_coord_msg.detected.data = true;
            hand_position_pub.publish(hand_coord_msg);

          }
          //cerr << "No depth data found! " << endl;
        // }
      }

      // If both coordinates have been found we check the distance between them
      if( hand_coord_msg.detected.data && object_position_msg.detected.data )
      {
        float x = hand_position.x-object_position.x;
        float y = hand_position.y-object_position.y;
        float z = hand_position.z-object_position.z;
        float d = sqrt( pow(x,2) + pow(y,2) + pow(z,2) );
        cout << "d: " << d << endl;

        if( d > 200)
        {
          object_position_msg.detected.data = false;
        }
      }else{
        // If they are not both detected the hand is not in the workspace
        object_position_msg.detected.data = false;
      }

      obj_position_pub.publish(object_position_msg);

      if(cv::waitKey(30) >= 0)
      {
        device->stop();
        if(!device->is_streaming())
          break;
      }
      ros::spinOnce();
    } // end of while ros ok

    return 0;
} // end of main

void condefects(std::vector<cv::Vec4i> convexityDefectsSet, std::vector<cv::Point> mycontour, cv::Mat &original)
{
    for (uint cDefIt = 0; cDefIt < convexityDefectsSet.size(); cDefIt++) {

        int startIdx = convexityDefectsSet[cDefIt].val[0]; cv::Point ptStart(mycontour[startIdx]);
        int endIdx = convexityDefectsSet[cDefIt].val[1]; cv::Point ptEnd(mycontour[endIdx]);
        int farIdx = convexityDefectsSet[cDefIt].val[2]; cv::Point ptFar(mycontour[farIdx]);

        // double depth = static_cast<double>(convexityDefectsSet[cDefIt].val[3]) / 256;
        //cout << "depth" << depth << endl;
        //display start points
        cv::circle(original,ptStart,5,cv::Scalar(255,0,0),2,8);
        //display all end points
        cv::circle(original, ptEnd, 5, cv::Scalar(255, 255, 0), 2, 8);
        //display all far points
        cv::circle(original,ptFar,5,cv::Scalar(0,0,255),2,8);
    }
}

float distP2P(cv::Point a, cv::Point b)
{
	float d = sqrt(fabs( pow(a.x-b.x,2) + pow(a.y-b.y,2) ));
	return d;
}

float getAngle(cv::Point s, cv::Point f, cv::Point e)
{
	float l1 = distP2P(f,s);
	float l2 = distP2P(f,e);
	float dot=(s.x-f.x)*(e.x-f.x) + (s.y-f.y)*(e.y-f.y);
	float angle = acos(dot/(l1*l2));
	angle=angle*180/M_PI;
	return angle;
}
