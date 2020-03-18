#include "ransac.h"
#include <algorithm>
#include <iostream>
#include <opencv2/calib3d.hpp>
#include <time.h>
using namespace std;
using namespace cv;

void project(float x1, float y1, cv::Mat H, float &x2, float &y2)
{
    x2 = (H.at<double>(0, 0) * x1 + H.at<double>(0, 1) * y1 + H.at<double>(0, 2)) /
         (H.at<double>(2, 0) * x1 + H.at<double>(2, 1) * y1 + H.at<double>(2, 2));
    y2 = (H.at<double>(1, 0) * x1 + H.at<double>(1, 1) * y1 + H.at<double>(1, 2)) /
         (H.at<double>(2, 0) * x1 + H.at<double>(2, 1) * y1 + H.at<double>(2, 2));
}

void generateRandomIndex(int randIndex[], int max, int length)
{

    std::vector<int> initIndex(max);
    int rands[max - 1];

    for (int i = 0; i < max; i++)
    {
        initIndex[i] = i;
    }
    for (int i = 0; i < length; i++)
    {
        rands[i] = rand() % (max - i);
    }
    for (int i = 0; i < length; i++)
    {
        int delta = rands[i];
        int tmpIndex = initIndex[delta];
        randIndex[i] = tmpIndex;
        remove(begin(initIndex), end(initIndex), tmpIndex);
    }
}

void computeInlierCount(cv::Mat H, std::vector<cv::DMatch> matches, int &numMatches, double inlierThreshold,
                        vector<cv::KeyPoint> kp1, vector<cv::KeyPoint> kp2)
{
    numMatches = 0;
    double squaredThres = inlierThreshold * inlierThreshold;
    for (int i = 0; i < matches.size(); i++)
    {
        DMatch tmpM = matches[i];
        Point2f pt1 = kp1[tmpM.queryIdx].pt;
        Point2f pt2 = kp2[tmpM.trainIdx].pt;
        float x2, y2;
        project(pt1.x, pt1.y, H, x2, y2);
        if (squaredThres >= (pt2.x - x2) * (pt2.x - x2) + (pt2.y - y2) * (pt2.y - y2))
            numMatches++;
    }
}

std::vector<cv::DMatch> refineMatches(
    std::vector<cv::KeyPoint> kp1, std::vector<cv::KeyPoint> kp2, 
    double inlierThreshold,
    std::vector<cv::DMatch> matches, cv::Mat H)
{
    std::vector<cv::DMatch> result;
    double squaredThres = inlierThreshold * inlierThreshold;
    for (int i = 0; i < matches.size(); i++)
    {
        DMatch tmpM = matches[i];
        Point2f pt1 = kp1[tmpM.queryIdx].pt;
        Point2f pt2 = kp2[tmpM.trainIdx].pt;
        float x2, y2;
        project(pt1.x, pt1.y, H, x2, y2);
        if (squaredThres >= (pt2.x - x2) * (pt2.x - x2) + (pt2.y - y2) * (pt2.y - y2))
            result.emplace_back(matches[i]);
    }
    return result;
}

std::vector<cv::DMatch> RANSAC(std::vector<cv::DMatch> matches, int &numMatches, int numIterations, double inlierThreshold,
            cv::Mat &hom, cv::Mat &homInv, cv::Mat &image1Display, cv::Mat &image2Display,
            vector<cv::KeyPoint> kp1, vector<cv::KeyPoint> kp2)
{
    srand((unsigned)time(NULL));
    int maxInlier = 0;
    for (int i = 0; i < numIterations; i++)
    {
        int indices[4];
        generateRandomIndex(indices, matches.size(), 4);
        vector<Point2f> pts1, pts2, projectedPts2;
        for (int j = 0; j < 4; j++)
        {
            DMatch tmpM = matches[indices[j]];
            pts1.emplace_back(kp1[tmpM.queryIdx].pt);
            pts2.emplace_back(kp2[tmpM.trainIdx].pt);
        }
        vector<uchar> m;
        cv::Mat H = cv::findHomography(pts1, pts2, 0);
        // perspectiveTransform(pts1, projectedPts2, H);
        for (int j = 0; j < 4; j++)
        {
            float x2, y2;
            Point2f tmpPt;
            project(pts1[j].x, pts1[j].y, H, x2, y2);
            tmpPt.x = x2;
            tmpPt.y = y2;
            projectedPts2.push_back(tmpPt);
        }
        int numMatches;
        computeInlierCount(H, matches, numMatches, inlierThreshold, kp1, kp2);
        if (numMatches > maxInlier)
        {
            hom = H;
            maxInlier = numMatches;
        }
    }

    std::vector<cv::DMatch> refined = refineMatches(kp1, kp2, inlierThreshold, matches, hom);
    homInv = hom.inv(); 
    return refined;
}
