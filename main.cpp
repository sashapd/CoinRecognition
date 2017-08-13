#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <map>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "Coin.h"

std::vector<Coin> getCoins(const cv::Mat& image) {
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    cv::Mat blured;
    cv::GaussianBlur(grayImage, blured, cv::Size(9, 9), 2, 2 );

    std::vector<cv::Vec3f> circles;

    cv::HoughCircles( blured, circles, cv::HOUGH_GRADIENT, 1, grayImage.rows/16, 100, 45);

    std::vector<Coin> coins;

    for(auto it = circles.begin(); it != circles.end(); it++) {
        coins.push_back(Coin(*it, image));
    }

    return coins;
}

void drawTotalValue(cv::Mat& image, const int totalValue) {
    cv::Point textLocation(image.cols / 2 - 400, image.rows - 50);
    cv::Scalar textColor(0, 0, 0);
    cv::putText(image, "Total Value: " + std::to_string(totalValue), textLocation, cv::FONT_HERSHEY_TRIPLEX, 3, textColor);
}

void drawFrame(cv::Mat& image) {
    cv::Scalar color(100, 200, 100);
    int thickness = 10;
    cv::line(image, cv::Point(0, 0), cv::Point(0, image.rows - 1), color, thickness);
    cv::line(image, cv::Point(0, image.rows - 1), cv::Point(image.cols - 1, image.rows - 1), color, thickness);
    cv::line(image, cv::Point(image.cols - 1, image.rows - 1), cv::Point(image.cols - 1, 0), color, thickness);
    cv::line(image, cv::Point(image.cols - 1, 0), cv::Point(0, 0), color, thickness);
}

cv::Point2i findClosestTo(const cv::Point2i& point, const std::vector<cv::Point2i>& points) {
    double minDist = 1000000;
    cv::Point2i closestPoint;
    for (const auto &p : points) {
        double distance = pow(pow(point.x - p.x, 2) + pow(point.y - p.y, 2), 0.5);
        if(distance < minDist) {
            closestPoint = p;
            minDist = distance;
        }
    }
    return closestPoint;
}


std::vector<cv::Point2i> getPaperSheetCoordinates(const cv::Mat& image) {
    cv::Mat cannyOut;
    cv::Canny(image, cannyOut, 100, 200);
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(cannyOut, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    std::vector<cv::Point> maxCountour;
    double maxArea = 0;

    for(int i = 0; i< contours.size(); i++) {
        std::vector<cv::Point>& countour = contours[i];
        double area = cv::contourArea(countour);
        if(area > maxArea) {
            maxCountour = countour;
            maxArea = area;
        }
    }
    double perimeter = cv::arcLength(maxCountour, true);

    std::vector<cv::Point> p;
    cv::approxPolyDP(maxCountour, p, perimeter * 0.02, true);

    cv::Point2f imageCorners[4] = {
                cv::Point2f(0, 0),
                cv::Point2f(0, image.rows - 1),
                cv::Point2f(image.cols - 1, image.rows - 1),
                cv::Point2f(image.cols - 1, 0)
    };

    std::vector<cv::Point2i> extremes;
    for(int i = 0; i < 4; i++) {
        extremes.push_back(findClosestTo(imageCorners[i], p));
    }

    //change coordinates order if the paper sheet is rotated 90 degrees
    if(cv::norm(extremes[2] - extremes[1]) < cv::norm(extremes[1] - extremes[0])) {
        extremes.push_back(extremes[0]);
        extremes.erase(extremes.begin());
    }

    return extremes;
}

const int paperSheedHeight = 1000;
const int paperSheedWidth = paperSheedHeight * 1.41421356237;

cv::Mat getPaperSheetTransformationMatrix(const cv::Mat& image) {
    std::vector<cv::Point2i> coordinates = getPaperSheetCoordinates(image);

    cv::Point2f srcPoint[4];
    std::copy(coordinates.begin(), coordinates.end(), srcPoint);

    cv::Point2f dstPoint[4] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, paperSheedHeight - 1),
            cv::Point2f(paperSheedWidth - 1, paperSheedHeight - 1),
            cv::Point2f(paperSheedWidth - 1, 0)
    };

    cv::Mat transformMatrix = cv::getPerspectiveTransform(srcPoint, dstPoint);

    return transformMatrix;
}

cv::Mat getPaperSheetRegion(const cv::Mat& image) {
    cv::Mat paperSheet(paperSheedHeight, paperSheedWidth, CV_8UC3);

    cv::Mat transformMatrix = getPaperSheetTransformationMatrix(image);
    cv::warpPerspective(image, paperSheet, transformMatrix, paperSheet.size());

    return paperSheet;
}

void combinePaperRegion(cv::Mat& image, cv::Mat& region) {
    const std::vector<cv::Point2i> coordinates = getPaperSheetCoordinates(image);

    cv::Point2f srcPoint[4] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, paperSheedHeight - 1),
            cv::Point2f(paperSheedWidth - 1, paperSheedHeight - 1),
            cv::Point2f(paperSheedWidth - 1, 0)
    };

    cv::Point2f dstPoint[4];
    std::copy(coordinates.begin(), coordinates.end(), dstPoint);

    cv::Mat transformMatrix = cv::getPerspectiveTransform(srcPoint, dstPoint);
    cv::Mat transformedRegion(image.rows, image.cols, CV_8UC3);
    cv::warpPerspective(region, transformedRegion, transformMatrix, image.size());

    cv::Mat mask = transformedRegion > 0;

    transformedRegion.copyTo(image, mask);
}

int main() {
    cv::Mat image = cv::imread("coins6.jpg", 1);
    cv::Mat region = getPaperSheetRegion(image);

    std::vector<Coin> coins = getCoins(region);
    drawFrame(region);

    int totalValue = 0;

    for(const auto& coin : coins) {
        int coinValue = coin.getValue();
        totalValue += coinValue;

        coin.drawInfo(region);
    }

    drawTotalValue(region, totalValue);

    combinePaperRegion(image, region);

    cv::Mat imageResized;
    int newWidth = 1000;
    cv::resize(image, imageResized, cv::Size(newWidth, newWidth * image.rows / image.cols));
    cv::imshow("Coin Recognition", imageResized );
    cv::waitKey(0);

    return 0;
}