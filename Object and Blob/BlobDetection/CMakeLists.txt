cmake_minimum_required(VERSION 2.8)
project( BlobAnalysis )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
# add_executable( BlobAnalysis src/BlobAnalysis.cpp )
# add_executable( BlobAnalysis src/OpenCV_simpleBlobDetection.cpp)
 add_executable( BlobAnalysis src/BlobOnlyGreen.cpp )
target_link_libraries( BlobAnalysis ${OpenCV_LIBS} )
