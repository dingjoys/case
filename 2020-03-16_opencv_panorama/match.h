#ifndef CVTASK_MATCH_H
#define CVTASK_MATCH_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include "math.h"

cv::Mat compute_distance(cv::Mat feat1, cv::Mat feat2);

std::vector<cv::DMatch> match_features(cv::Mat& img1, cv::Mat& img2,
        cv::Mat& feat1, cv::Mat& feat2,
        std::vector<cv::KeyPoint>& kp1, std::vector<cv::KeyPoint>& kp2);

#endif //CVTASK_MATCH_H
