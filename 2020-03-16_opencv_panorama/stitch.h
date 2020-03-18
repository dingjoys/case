
#ifndef CVTASK_STICH_H
#define CVTASK_STICH_H

#include <opencv2/core/core.hpp>
// #include <opencv2/highgui/highgui.hpp>
// #include <opencv2/highgui/highgui_c.h>
// #include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/features2d.hpp>
void stitch(cv::Mat image1,cv::Mat  image2,cv::Mat  hom,cv::Mat  homInv,cv::Mat & stitchedImage);

#endif
