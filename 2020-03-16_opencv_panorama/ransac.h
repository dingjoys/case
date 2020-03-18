#ifndef CVTASK_RANSAC_H
#define CVTASK_RANSAC_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <math.h>
#include <vector>

void project(float x1, float y1, cv::Mat H, float &x2, float &y2);

void computeInlierCount(cv::Mat H, std::vector<cv::DMatch> matches, int &numMatches, double inlierThreshold, std::vector<cv::KeyPoint> kp1, std::vector<cv::KeyPoint> kp2);

std::vector<cv::DMatch> RANSAC(std::vector<cv::DMatch> matches, int &numMatches, int numIterations, double inlierThreshold,
            cv::Mat &hom, cv::Mat &homInv, cv::Mat &image1Display, cv::Mat &image2Display,
            std::vector<cv::KeyPoint> kp1, std::vector<cv::KeyPoint> kp2);

#endif
