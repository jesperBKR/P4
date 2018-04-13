
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;

int main(int argc, char **argv)
{

  Mat inputImage = imread("data/Square_1.jpg"); // Read Image
  Mat biImg;
  inRange(inputImage, Scalar(0,100,0), Scalar(90,255,90), biImg);

  const char * my_window = "Test Window"; //Name on Pic-Window
  namedWindow(my_window, WINDOW_NORMAL);








  imshow(my_window, output);



  // Mat appleImage = imread("data/apple.jpg");
  // Mat morp = getStructuringElement( MORPH_ELLIPSE , Size(19, 19), Point(-1, -1) );
  // Mat binImg = Mat(appleImage.rows, appleImage.cols, CV_8U);
  // Mat contourImg = Mat(appleImage.rows, appleImage.cols, CV_8UC3);
  //
  //
  //
  // if(appleImage.empty())
  // {
  //   std::cout <<"No image was loaded. Goodbye!" << std::endl;
  //   return -1;
  // }
  // for(int x = 0; x < appleImage.cols; x++){
  //   for(int y = 0; y < appleImage.rows; y++){
  //     if(( appleImage.at<Vec3b>(y,x)[0] < 200
  //       && appleImage.at<Vec3b>(y,x)[1] < 200
  //       && appleImage.at<Vec3b>(y,x)[2] > 200 ) ||
  //        ( appleImage.at<Vec3b>(y,x)[0] < 100
  //       && appleImage.at<Vec3b>(y,x)[1] < 100
  //       && appleImage.at<Vec3b>(y,x)[2] > 100 )) //appleImage.at<Vec3b>(y,x)[0] < 100 &&
  //     {
  //       binImg.at<char>(y, x) = 255;
  //     } else {
  //       binImg.at<char>(y, x) = 0;
  //     }
  //   }
  // }
  //
  // morphologyEx(binImg, binImg, MORPH_CLOSE, morp, Point(-1, -1));
  // morphologyEx(binImg, binImg, MORPH_OPEN, morp, Point(-1, -1));
  // namedWindow(my_window);
  // imshow(my_window, binImg);

  waitKey(0);

  return 0;

}
