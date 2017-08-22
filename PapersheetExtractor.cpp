//
// Created by Oleksandr Padko on 8/23/17.
//

#include <opencv2/imgproc.hpp>
#include <opencv/cv.hpp>
#include <iostream>
#include "PapersheetExtractor.h"

PapersheetExtractor::PapersheetExtractor(const cv::Mat &image) {
    mImage = image;
}

cv::Mat PapersheetExtractor::getPaperSheetRegion() {
    findPaperSheetCoordinates(mImage);
    if(foundPaperSheet) {
        cv::Mat paperSheet(cPaperSheedHeight, cPaperSheedWidth, CV_8UC3);

        cv::Mat transformMatrix = getPaperSheetTransformationMatrix(mImage);
        cv::warpPerspective(mImage, paperSheet, transformMatrix, paperSheet.size());

        return paperSheet;
    }
}

cv::Point2f PapersheetExtractor::findClosestTo(const cv::Point2f &point, const std::vector<cv::Point2f> &points) const {
    double minDist = DBL_MAX;
    cv::Point2f closestPoint;
    for (const auto &p : points) {
        double distance = pow(pow(point.x - p.x, 2) + pow(point.y - p.y, 2), 0.5);
        if (distance < minDist) {
            closestPoint = p;
            minDist = distance;
        }
    }
    return closestPoint;
}

void PapersheetExtractor::findPaperSheetCoordinates(const cv::Mat &image) {
    cv::Mat cannyOut;
    cv::Canny(image, cannyOut, 50, 150);
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(cannyOut, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

    std::vector<cv::Point> maxCountour;
    double maxArea = 0;
    const double minAllowedArea = 1000;

    std::vector<std::vector<cv::Point> > hull(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        cv::convexHull(cv::Mat(contours[i]), hull[i]);
    }

    for (auto &&contour : hull) {
        std::vector<cv::Point> approxContour;

        double perimeter = cv::arcLength(contour, true);
        cv::approxPolyDP(contour, approxContour, perimeter * 0.08, true);
        double area = cv::contourArea(approxContour);

        if (area > maxArea && area > minAllowedArea && approxContour.size() == 4) {
            maxCountour = approxContour;
            maxArea = area;
            foundPaperSheet = true;
        }
    }
    if (foundPaperSheet) {
        double perimeter = cv::arcLength(maxCountour, true);

        std::vector<cv::Point2f> points;
        cv::approxPolyDP(maxCountour, points, perimeter * 0.08, true);

        std::vector<cv::Point2f> imageCorners = getCornerCoordinates(image.cols, image.rows);
        points = sortCornerCoordinates(points);
        mPaperSheetCoordinates = points;
    }
}

cv::Mat PapersheetExtractor::getPaperSheetTransformationMatrix(const cv::Mat &image) const {
    std::vector<cv::Point2f> dstPoint = getCornerCoordinates(cPaperSheedWidth, cPaperSheedHeight);

    cv::Mat transformMatrix = cv::getPerspectiveTransform(mPaperSheetCoordinates.data(), dstPoint.data());

    return transformMatrix;
}

std::vector<cv::Point2f> PapersheetExtractor::getCornerCoordinates(const int width, const int height) const {
    std::vector<cv::Point2f> corners{
            cv::Point2f(0, 0),
            cv::Point2f(0, height - 1),
            cv::Point2f(width - 1, height - 1),
            cv::Point2f(width - 1, 0)
    };
    return corners;
}

bool PapersheetExtractor::comparePointsClockwise(cv::Point2f a, cv::Point2f b, cv::Point2f center) {
    if (a.x - center.x >= 0 && b.x - center.x < 0)
        return true;
    if (a.x - center.x < 0 && b.x - center.x >= 0)
        return false;
    if (a.x - center.x == 0 && b.x - center.x == 0) {
        if (a.y - center.y >= 0 || b.y - center.y >= 0)
            return a.y > b.y;
        return b.y > a.y;
    }

    // compute the cross product of vectors (center -> a) x (center -> b)
    int det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
    if (det < 0)
        return true;
    if (det > 0)
        return false;

    // points a and b are on the same line from the center
    // check which point is closer to the center
    int d1 = (a.x - center.x) * (a.x - center.x) + (a.y - center.y) * (a.y - center.y);
    int d2 = (b.x - center.x) * (b.x - center.x) + (b.y - center.y) * (b.y - center.y);
    return d1 > d2;
}

std::vector<cv::Point2f> PapersheetExtractor::sortCornerCoordinates(std::vector<cv::Point2f> points) const {
    cv::Point2f center(0, 0);
    for (auto &&point : points) {
        center += point;
    }
    center.x /= points.size();
    center.y /= points.size();

    std::sort(points.begin(), points.end(),
              std::bind(comparePointsClockwise, std::placeholders::_1, std::placeholders::_2, center));

    std::reverse(points.begin(), points.end());

    //change coordinates order if the paper sheet is rotated 90 degrees
    if (cv::norm(points[2] - points[1]) < cv::norm(points[1] - points[0])) {
        points.push_back(points[0]);
        points.erase(points.begin());
    }

    return points;
}