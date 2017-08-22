//
// Created by Oleksandr Padko on 8/23/17.
//

#include <opencv2/imgproc.hpp>
#include <opencv/cv.hpp>
#include "PapersheetExtractor.h"

PapersheetExtractor::PapersheetExtractor(const cv::Mat &image) {
    mImage = image;
}

cv::Mat PapersheetExtractor::getPaperSheetRegion() {
    findPaperSheetCoordinates(mImage);
    cv::Mat paperSheet(cPaperSheedHeight, cPaperSheedWidth, CV_8UC3);

    cv::Mat transformMatrix = getPaperSheetTransformationMatrix(mImage);
    cv::warpPerspective(mImage, paperSheet, transformMatrix, paperSheet.size());

    return paperSheet;
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
    cv::Canny(image, cannyOut, 150, 200);
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(cannyOut, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

    std::vector<cv::Point> maxCountour;
    double maxArea = 0;
    const double minAllowedArea = 1000;

    for (int i = 0; i < contours.size(); i++) {
        std::vector<cv::Point> &countour = contours[i];
        std::vector<cv::Point> approxContour;

        double perimeter = cv::arcLength(countour, true);
        cv::approxPolyDP(countour, approxContour, perimeter * 0.08, true);
        double area = cv::contourArea(approxContour);


        if (area > maxArea && area > minAllowedArea) {
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
        points = sortCornerCoordinates(points, imageCorners);
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

std::vector<cv::Point2f> PapersheetExtractor::sortCornerCoordinates(const std::vector<cv::Point2f> &points,
                                                            const std::vector<cv::Point2f> &imageCorners) const {
    std::vector<cv::Point2f> extremes;
    for (int i = 0; i < 4; i++) {
        extremes.push_back(findClosestTo(imageCorners[i], points));
    }

    //change coordinates order if the paper sheet is rotated 90 degrees
    if (cv::norm(extremes[2] - extremes[1]) < cv::norm(extremes[1] - extremes[0])) {
        extremes.push_back(extremes[0]);
        extremes.erase(extremes.begin());
    }

    return extremes;
}