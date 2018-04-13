
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;

int main(int argc, char **argv)
{

  Mat inputImage = imread("data/Square_1.jpg"); // Read Image
  Mat output;
  Mat elementStructure = getStructuringElement( MORPH_RECT, Size(10, 10), Point(-1, -1) );

  inRange(inputImage, Scalar(0,100,0), Scalar(90,255,90), output);


  const char * my_window = "Test Window"; //Name on Pic-Window
  namedWindow(my_window, WINDOW_NORMAL);

 morphologyEx(output, output, MORPH_CLOSE, elementStructure, Point(-1, -1));
// morphologyEx(output, output, MORPH_OPEN, elementStructure, Point(-1, -1));

/*
for(int x = 0; x < output.cols; x++){
  for(int y = 0; y < output.rows; y++){
    if (output.at<Vec3b>(y,x)[0] = 1){


    };
  };
};
*/




  imshow(my_window, output);



  waitKey(0);

  return 0;

}
