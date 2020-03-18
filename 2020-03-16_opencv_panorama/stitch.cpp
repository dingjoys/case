#define seamless false

#include "stitch.h"
#include "ransac.h"
#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;

void getProjectedMinMax(float x, float y, cv::Mat homInv, float &minX, float &maxX, float &minY, float &maxY)
{
    float tmpX, tmpY;
    project(x, y, homInv, tmpX, tmpY);

    if (tmpX < minX)
    {
        minX = tmpX;
    }
    if (tmpX > maxX)
    {
        maxX = tmpX;
    }

    if (tmpY < minY)
    {
        minY = tmpY;
    }
    if (tmpY > maxY)
    {
        maxY = tmpY;
    }
}

void getIntMinMax(int x, int y, int &minX, int &maxX, int &minY, int &maxY)
{
    if (x < minX)
    {
        minX = x;
    }
    if (x > maxX)
    {
        maxX = x;
    }
    if (y < minY)
    {
        minY = y;
    }
    if (y > maxY)
    {
        maxY = y;
    }
}

void stitch(cv::Mat image1, cv::Mat image2, cv::Mat hom, cv::Mat homInv, cv::Mat &stitchedImage)
{
    float x, y, minX = 0, maxX = image1.cols, minY = 0, maxY = image1.rows;
    getProjectedMinMax(0, 0, homInv, minX, maxX, minY, maxY);
    getProjectedMinMax(image2.cols - 1, 0, homInv, minX, maxX, minY, maxY);
    getProjectedMinMax(image2.cols - 1, image2.rows - 1, homInv, minX, maxX, minY, maxY);
    getProjectedMinMax(0, image2.rows - 1, homInv, minX, maxX, minY, maxY);

    stitchedImage = cv::Mat::zeros(ceil(maxY - minY), ceil(maxX - minX), image1.type());
    cv::Mat stitchedMask1 = cv::Mat::zeros(ceil(maxY - minY), ceil(maxX - minX), image1.type());
    cv::Mat stitchedImage2 = cv::Mat::zeros(ceil(maxY - minY), ceil(maxX - minX), image1.type()),
            stitchedMask2 = cv::Mat::zeros(ceil(maxY - minY), ceil(maxX - minX), image1.type());

    int maskMinX1, maskMaxX1, maskMinY1, maskMaxY1, maskMinX2, maskMaxX2, maskMinY2, maskMaxY2;
    maskMaxX1 = maskMaxY1 = maskMaxX2 = maskMaxY2 = 0;
    maskMinX1 = maskMinX2 = stitchedImage.cols;
    maskMinY1 = maskMinY2 = stitchedImage.rows;
    for (int i = 0; i < image1.cols; i++)
    {
        for (int j = 0; j < image1.rows; j++)
        {
            stitchedImage.at<cv::Vec3b>(ceil(j - minY), ceil(i - minX)) = image1.at<cv::Vec3b>(j, i);
            cv::Vec3b tmpColor(255, 255, 255);
            stitchedMask1.at<cv::Vec3b>(ceil(j - minY), ceil(i - minX)) = tmpColor;
            getIntMinMax(ceil(i - minX), ceil(j - minY), maskMinX1, maskMaxX1, maskMinY1, maskMaxY1);
        }
    }

    cv::Size subRectSize(1, 1);
    for (int i = 0; i < stitchedImage.cols; i++)
    {
        for (int j = 0; j < stitchedImage.rows; j++)
        {

            float x, y;
            project(i + minX, j + minY, hom, x, y);
            if (x >= 0 && ceil(x) < image2.cols && y >= 0 && ceil(y) < image2.rows)
            {
                cv::Point2f pt(x, y);
                cv::Mat tmpM;
                cv::getRectSubPix(image2, subRectSize, pt, tmpM, -1);

                if (!(stitchedImage.at<cv::Vec3b>(j, i)[0] == 0 && stitchedImage.at<cv::Vec3b>(j, i)[1] == 0 && stitchedImage.at<cv::Vec3b>(j, i)[2] == 0))
                {
                    cv::Vec3b tmpColor;
                    tmpColor[0] = (stitchedImage.at<cv::Vec3b>(j, i)[0] + tmpM.at<cv::Vec3b>(0, 0)[0]) / 2;
                    tmpColor[1] = (stitchedImage.at<cv::Vec3b>(j, i)[1] + tmpM.at<cv::Vec3b>(0, 0)[1]) / 2;
                    tmpColor[2] = (stitchedImage.at<cv::Vec3b>(j, i)[2] + tmpM.at<cv::Vec3b>(0, 0)[2]) / 2;
                    stitchedImage.at<cv::Vec3b>(j, i) = tmpColor;
                }
                else
                {
                    stitchedImage.at<cv::Vec3b>(j, i) = tmpM.at<cv::Vec3b>(0, 0);
                }
                if (seamless)
                {
                    stitchedImage2.at<cv::Vec3b>(j, i) = tmpM.at<cv::Vec3b>(0, 0);
                    cv::Vec3b tmpColor(255, 255, 255);
                    stitchedMask2.at<cv::Vec3b>(j, i) = tmpColor;
                    getIntMinMax(i, j, maskMinX2, maskMaxX2, maskMinY2, maskMaxY2);
                }
            }
        }
    }
    if (seamless)
    {
        cv::Point center1(floor((maskMinX1 + maskMaxX1) / 2), floor((maskMinY1 + maskMaxY1) / 2)),
            center2((maskMinX2 + maskMaxX2) / 2, (maskMinY2 + maskMaxY2) / 2);
        cv::Mat result(stitchedImage);
        cout << image1.cols << ", " << image1.rows << result.cols << ", " << result.rows << endl;
        stitchedMask1 = cv::Mat::ones(image1.rows, image1.cols, image1.type());
        for (int i = 0; i < image1.cols; i++)
        {
            for (int j = 0; j < image1.rows; j++)
            {
                cv::Vec3b tmpColor(255, 255, 255);
                stitchedMask1.at<cv::Vec3b>(j, i) = tmpColor;
            }
        }
        cv::seamlessClone(image1, result, stitchedMask1, center1, result, cv::NORMAL_CLONE);
        cv::seamlessClone(stitchedImage2, stitchedImage, stitchedMask2, center2, result, cv::NORMAL_CLONE);
    }
}