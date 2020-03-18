#include "descriptor.h"

cv::Mat sift(cv::Mat& src_img, std::vector<cv::KeyPoint>& kp) {
  cv::Mat gray_img, des;
  cvtColor(src_img, gray_img, cv::COLOR_BGR2GRAY);

  cv::Mat sobel_Ix, sobel_Iy;
  cv::Sobel(gray_img, sobel_Ix, CV_64F, 1, 0, 3);
  cv::Sobel(gray_img, sobel_Iy, CV_64F, 0, 1, 3);

  // rotational invariance
  double angles[8];
  for (int i = 0; i < 8; i++) {
    angles[i] = (double)i * M_PI * 0.25;
  }
  cv::Mat rotate[8];
  for (int i = 0; i < 8; i++) {
    rotate[i] = sobel_Ix * cos(angles[i]) + sobel_Iy * sin(angles[i]);
  }
  cv::Mat weighting_factor;
  cv::sqrt(sobel_Ix.mul(sobel_Ix) + sobel_Iy.mul(sobel_Iy), weighting_factor);

  cv::Mat acc[8];
  for (auto & x : acc) {
    x = cv::Mat::zeros(src_img.size(), CV_64F);
  }


  for (int i = 0; i < src_img.rows; i++) {
    for (int j = 0; j < src_img.cols; j++) {

      int idx = -1;
      double max_val = -DBL_MAX;
      for (int k = 0; k < 8; k++) {
        if (rotate[k].at<double>(i, j) > max_val) {
          idx = k;
          max_val = rotate[k].at<double>(i, j);
        }
      }

      acc[idx].at<double>(i, j) = weighting_factor.at<double>(i, j);
    }
  }

  cv::Mat kernel = (cv::Mat_<double>(4, 4) <<
          1, 1, 1, 1,
          1, 1, 1, 1,
          1, 1, 1, 1,
          1, 1, 1, 1);

  cv::Mat feature[8];
  for (int i = 0; i < 8; i++) {
    cv::filter2D(acc[i], feature[i], acc[i].depth(), kernel);
  }

  int sz = kp.size();
  cv::Mat fvs = cv::Mat::zeros(sz, 128, CV_64F);
  for (int i = 0; i < sz; i++) {
    int x_grid[16] = {0};
    int y_grid[16] = {0};
    for (int m = 0; m < 4; m++) {
      for (int n = 0; n < 4; n++) {
        x_grid[m * 4 + n] = (int)kp[i].pt.x + 4 * n - 6;
        y_grid[m * 4 + n] = (int)kp[i].pt.y + 4 * m - 6;
      }
    }
    for (int j = 0; j < 16; j++) {
      for (int k = 0; k < 8; k++) {
        fvs.at<double>(i, 8 * j + k) = feature[k].at<double>(y_grid[j], x_grid[j]);
      }
    }
  }
  cv::Mat fvs_norm;
  cv::normalize(fvs, fvs_norm);

  cv::Mat res;
  cv::pow(fvs_norm, 0.9, res);
  return res;
}