#ifndef CVTASK_DESCRIPTOR_H
#define CVTASK_DESCRIPTOR_H

#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include "math.h"

cv::Mat sift(cv::Mat& src_img, std::vector<cv::KeyPoint>& kp);

#endif //CVTASK_DESCRIPTOR_H
