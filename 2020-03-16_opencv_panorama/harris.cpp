#include "harris.h"

cv::Mat pooling(cv::Mat img, int grid, int overlap)
{
  cv::Mat pool_img = cv::Mat((int)((img.rows - 1) / overlap) + 1, (int)((img.cols - 1) / overlap) + 1, CV_64F);
  for (int col = 0,pool_col=0; col < img.cols; col+= overlap) {
    for (int row = 0,pool_row=0; row < img.rows; row+= overlap) {
      int minCol = std::min(col + overlap, img.cols);
      double maxData = 0;
      for (int poolX = col; poolX < minCol; poolX++) {
        int minRow = std::min(row + overlap, img.rows);
        for (int poolY = row; poolY<minRow; poolY++) {
          if (img.at<double>(poolY, poolX) > maxData) {
            maxData = img.at<double>(poolY, poolX);
          }
        }
      }
      pool_img.at<double>(pool_row, pool_col) = maxData;
      pool_row++;
    }
    pool_col++;
  }
  return pool_img;
}


cv::Mat harris(cv::Mat& src_img) {
  cv::Mat gray_img;
  cvtColor(src_img, gray_img, cv::COLOR_BGR2GRAY);

  cv::Mat sobel_Ix, sobel_Iy;
  cv::Sobel(gray_img, sobel_Ix, CV_64F, 1, 0, 3);
  cv::Sobel(gray_img, sobel_Iy, CV_64F, 0, 1, 3);

  cv::Mat Ixx, Ixy, Iyy;
  cv::multiply(sobel_Ix, sobel_Ix, Ixx);
  cv::multiply(sobel_Ix, sobel_Iy, Ixy);
  cv::multiply(sobel_Iy, sobel_Iy, Iyy);

  cv::Mat blur_Ixx, blur_Iyy, blur_Ixy;
  cv::GaussianBlur(Ixx, blur_Ixx, cv::Size(3, 3), 5);
  cv::GaussianBlur(Ixy, blur_Ixy, cv::Size(3, 3), 5);
  cv::GaussianBlur(Iyy, blur_Iyy, cv::Size(3, 3), 5);

  cv::Mat response;
  response = blur_Ixx.mul(blur_Iyy) - blur_Ixy.mul(blur_Ixy) - 0.05 * (blur_Ixx + blur_Iyy).mul(blur_Ixx + blur_Iyy);

  cv::Mat tmp = response.reshape(1, 1);
  cv::Mat sorted;
  cv::sort(tmp, sorted, cv::SORT_ASCENDING);
  double median = sorted.at<double>(sorted.cols / 2);

  for (int i = 0; i < response.rows; i++) {
    for (int j = 0; j < response.cols; j++) {
      if (response.at<double>(i, j) < median) {
        response.at<double>(i, j) = 0.0;
      }
    }
  }
  cv::Mat pool = pooling(response, 7, 1);
  for (int i = 0; i < response.rows; i++) {
    for (int j = 0; j < response.cols; j++) {
      if (response.at<double>(i, j) != pool.at<double>(i, j)) {
        response.at<double>(i, j) = 0.0;
      }
    }
  }
  return response;
}