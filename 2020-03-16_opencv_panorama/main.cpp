#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#define STITCH_ ^
#include <iostream>
#include "harris.h"
#include "descriptor.h"
#include "match.h"
#include "ransac.h"
#include "stitch.h"

#include <iostream>
using namespace std;
using namespace cv;
class SURF;

void printMat(Mat A)
{
  for (int i = 0; i < A.rows; i++)
  {
    for (int j = 0; j < A.cols; j++)
      cout << A.at<double>(i, j) << ' ';
    cout << endl;
  }
  cout << endl;
}

cv::Mat doHarrisDetect(cv::Mat img1)
{
  cv::Mat corners1 = harris(img1);
  std::vector<cv::KeyPoint> kp1;
  cv::Mat tmp = corners1.reshape(1, 1);
  cv::Mat sorted;
  cv::sort(tmp, sorted, cv::SORT_DESCENDING);
  int sz = std::min(3000, tmp.cols);
  double v1 = sorted.at<double>(sz);

  for (int i = 0; i < corners1.rows; i++)
  {
    for (int j = 0; j < corners1.cols; j++)
    {
      if (corners1.at<double>(i, j) > v1)
      {
        kp1.emplace_back(cv::KeyPoint(j, i, 1));
      }
    }
  }
  cv::Mat result1;
  cv::drawKeypoints(img1, kp1, result1);
  return result1;
}

cv::Mat doStitch(cv::Mat img1, cv::Mat img2)
{
  if (!img1.data || !img2.data)
  {
    return img1;
  }
  //downsampling
  double sizeF1 = (img1.rows * img1.cols) * 1.0 / (800 * 600);
  double sizeF2 = (img2.rows * img2.cols) * 1.0 / (800 * 600);
  while (sizeF1 >= 2)
  {
    cv::pyrDown(img1, img1, Size(img1.cols / 2, img1.rows / 2));
    sizeF1 = img1.rows * img1.cols / (800 * 600);
  }
  while (sizeF2 >= 1)
  {
    cv::pyrDown(img2, img2, Size(img2.cols / 2, img2.rows / 2));
    sizeF2 = img2.rows * img2.cols / (800 * 600);
  }
  // Feature detection
  cv::Mat corners1 = harris(img1);
  cv::Mat corners2 = harris(img2);
  std::vector<cv::KeyPoint> kp1, kp2;
  cv::Mat res1, res2;

  cv::Mat tmp = corners1.reshape(1, 1);
  cv::Mat sorted;
  cv::sort(tmp, sorted, cv::SORT_DESCENDING);
  int sz = std::min(3000, tmp.cols);
  double v1 = sorted.at<double>(sz);

  for (int i = 0; i < corners1.rows; i++)
  {
    for (int j = 0; j < corners1.cols; j++)
    {
      if (corners1.at<double>(i, j) > v1)
      {
        kp1.emplace_back(cv::KeyPoint(j, i, 1));
      }
    }
  }

  tmp = corners2.reshape(1, 1);
  cv::sort(tmp, sorted, cv::SORT_DESCENDING);
  sz = std::min(3000, tmp.cols);
  double v2 = sorted.at<double>(sz);

  for (int i = 0; i < corners2.rows; i++)
  {
    for (int j = 0; j < corners2.cols; j++)
    {
      if (corners2.at<double>(i, j) > v2)
      {
        kp2.emplace_back(cv::KeyPoint(j, i, 1));
      }
    }
  }

  cv::Mat img1_feat = sift(img1, kp1);
  cv::Mat img2_feat = sift(img2, kp2);

  std::vector<cv::DMatch> matches = match_features(img1, img2, img1_feat, img2_feat, kp1, kp2);

  double min_dist = 0, max_dist = 0;

  for (int i = 0; i < img1_feat.rows; ++i)
  {
    double dist = matches[i].distance;
    if (dist < min_dist)
      min_dist = dist;
    if (dist > max_dist)
      max_dist = dist;
  }
  std::vector<cv::DMatch> good_matches;
  for (int j = 0; j < img1_feat.rows; ++j)
  {
    if (matches[j].distance <= std::max(2 * min_dist, 30.0))
      good_matches.emplace_back(matches[j]);
  }

  cv::Mat img_match;
  cv::drawMatches(img1, kp1, img2, kp2, good_matches, img_match);
  cv::imwrite("./output/2.png", img_match);

  int numMatches;
  int numIterations = 10000;
  double inlierThreshold = 3;

  cv::Mat hom, homInv;
  cv::Mat image1Display, image2Display;

  std::vector<cv::DMatch> ransacRefined = RANSAC(good_matches, numMatches, numIterations, inlierThreshold,
                                                 hom, homInv, image1Display, image2Display,
                                                 kp1, kp2);
  cv::Mat img_match2;
  cv::drawMatches(img1, kp1, img2, kp2, ransacRefined, img_match2);
  cv::imwrite("../output/3.png", img_match);
  cv::Mat stitchedImage;
  stitch(img1, img2, hom, homInv, stitchedImage);
  return stitchedImage;
}

int main(int argc, char const *argv[])
{
  cv::Mat boxImg = cv::imread("../project_images/Boxes.png");
  cv::Mat rainier1 = cv::imread("../project_images/Rainier1.png");
  cv::Mat rainier2 = cv::imread("../project_images/Rainier2.png");
  cv::Mat boxResult = doHarrisDetect(boxImg);
  cv::Mat rainier1Result = doHarrisDetect(rainier1);
  cv::Mat rainier2Result = doHarrisDetect(rainier2);

  cv::imwrite("../output/1a.png", boxResult);
  cv::imwrite("../output/1b.png", rainier1Result);
  cv::imwrite("../output/1c.png", rainier2Result);
  cout<<"..."<<endl;
  cv::Mat img1, img2;
  img1 = cv::imread("../project_images/Rainier1.png");
  img2 = cv::imread("../project_images/Rainier2.png");
  cv::Mat stitchedImage = doStitch(img1, img2);
  cout<<"..."<<endl;
  cv::imwrite("../output/4.png", stitchedImage);
  cv::imwrite("../output/stiched1.png", stitchedImage);
  cv::Mat img3 = cv::imread("../project_images/Rainier3.png");
  cv::Mat stitchedImage2 = doStitch(stitchedImage, img3);
  cout<<"..."<<endl;
  cv::imwrite("../output/stiched2.png", stitchedImage2);

  cv::Mat img4 = cv::imread("../project_images/Rainier5.png");
  cv::Mat stitchedImage3 = doStitch(stitchedImage2, img4);
  cout<<"..."<<endl;
  cv::imwrite("../output/stiched3.png", stitchedImage3);

  cv::Mat img5 = cv::imread("../project_images/Rainier4.png");
  cv::Mat stitchedImage4 = doStitch(stitchedImage3, img5);
  cout<<"..."<<endl;
  cv::imwrite("../output/stiched4.png", stitchedImage4);

  cv::Mat img6 = cv::imread("../project_images/Rainier6.png");
  cv::Mat stitchedImage5 = doStitch(stitchedImage4, img6);
  cout<<"..."<<endl;
  cv::imwrite("../output/allStiched.png", stitchedImage5);

  cv::Mat img7, img8;
  img7 = cv::imread("../project_images/my1.jpg");
  img8 = cv::imread("../project_images/my2.jpg");
  cv::Mat myStitchedImage = doStitch(img7, img8);
  cout<<"..."<<endl;
  cv::imwrite("../output/myStitch.png", myStitchedImage);
  return 0;
}
