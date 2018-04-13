
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;

// int intensityLef;
int tes = 0;


void MyEllipse(Mat biImg, int thickness, int y, int x )
{
  int angle = 360;
  int lineType = 8;
  ellipse( biImg,
       Point( x, y),
       Size( 50, 50 ),
       angle,
       0,
       360,
       Scalar( 255, 255, 255 ),
       thickness,
       lineType );
}


int main(int argc, char **argv)
{

  Mat inputImage = imread("data/Square_1.jpg"); // Read Image
  Mat morp = getStructuringElement( MORPH_ELLIPSE , Size(300, 300), Point(-1, -1) );
  Mat biImg;
  inRange(inputImage, Scalar(0,100,0), Scalar(90,255,90), biImg);
  double squaCoordi[8];
  morphologyEx(biImg, biImg, MORPH_CLOSE, morp, Point(-1, -1));

   for(int x = 0; x < biImg.rows; x++){
     for(int y = 0; y < biImg.cols; y++){

       int intensityMid = biImg.at<uchar>(y,x);
       if (intensityMid > 0) {

          if (biImg.at<uchar>(y-1,x-1) == 0 && biImg.at<uchar>(y,x-1) == 0 && biImg.at<uchar>(y+1,x-1) == 0 && biImg.at<uchar>(y+1,x) == 0 && biImg.at<uchar>(y+1,x+1) == 0) {
              std::cout << "Low Corner left: " << x <<", "<< y << '\n';
              tes = tes+1;

              MyEllipse(biImg, 2, y, x);
            }

          else if (biImg.at<uchar>(y-1,x+1) == 0 && biImg.at<uchar>(y-1,x) == 0 && biImg.at<uchar>(y-1,x-1) == 0 && biImg.at<uchar>(y,x-1) == 0 && biImg.at<uchar>(y+1,x-1) == 0) {
              std::cout << "Top Corner left: " << x <<", "<< y << '\n';
              tes = tes+1;

              MyEllipse(biImg, 2, y, x);
            }

          else if (biImg.at<uchar>(y+1,x+1) == 0 && biImg.at<uchar>(y,x+1) == 0 && biImg.at<uchar>(y-1,x+1) == 0 && biImg.at<uchar>(y-1,x) == 0 && biImg.at<uchar>(y-1,x-1) == 0) {
              std::cout << "Top Corner left: " << x <<", "<< y << '\n';
              tes = tes+1;

              MyEllipse(biImg, 2, y, x);
            }

          else if (biImg.at<uchar>(y+1,x-1) == 0 && biImg.at<uchar>(y+1,x) == 0 && biImg.at<uchar>(y+1,x+1) == 0 && biImg.at<uchar>(y,x+1) == 0 && biImg.at<uchar>(y-1,x-1) == 0) {
              std::cout << "Top Corner left: " << x <<", "<< y << '\n';
              tes = tes+1;
              MyEllipse(biImg, 2, y, x);
}

            std::cout << x <<", " << y << ", " << tes << '\n';
/*
         if (int intensityLef = biImg.at<uchar>(y,--x) = 0 ) {
           // if the pixel to left is emty
         }

         if (int intensityRig = biImg.at<uchar>(y,++x) = 0 ) {

           // if the pixel to right is emty
         }



         if (int intensityUp = biImg.at<uchar>(y,--x) = 0 ) {
           // if the pixel above is emty
         }

         if (int intensityDown = biImg.at<uchar>(y,--x) = 0 ) {
           // if the pixel underneth is emty
         } */

  //       std::cout << intensity << "Location" << x << ", " << y << '\n';
       };


     };
   };




  const char * my_window = "Test Window"; //Name on Pic-Window
  namedWindow(my_window, WINDOW_NORMAL);

  imshow(my_window, biImg);


  waitKey(0);

  return 0;

}
