//
// Created by Oleksandr Padko on 8/13/17.
//

#include "CoinCounter.h"
#include <float.h>

CoinCounter::CoinCounter(const cv::Mat &image) {
    findPaperSheetCoordinates(image);
    if (foundPaperSheet) {
        mPaperSheetRegion = getPaperSheetRegion(image);
        mCoins = getCoins();

        drawFrame();
        drawCoinsInfo();
        drawTotalValue();
    }
}

int CoinCounter::getTotalValue() const {
    int totalValue = 0;
    for (auto it = mCoins.begin(); it != mCoins.end(); it++) {
        totalValue += it->getValue();
    }
    return totalValue;
}

void CoinCounter::drawPaperRegion(cv::Mat &image) const {
    if (foundPaperSheet) {
        std::vector<cv::Point2f> srcPoint = getCornerCoordinates(cPaperSheedWidth, cPaperSheedHeight);

        cv::Mat transformMatrix = cv::getPerspectiveTransform(srcPoint.data(), mPaperSheetCoordinates.data());
        cv::Mat transformedRegion(image.rows, image.cols, CV_8UC3);
        cv::warpPerspective(mPaperSheetRegion, transformedRegion, transformMatrix, image.size());

        cv::Mat mask = transformedRegion > 0;

        transformedRegion.copyTo(image, mask);
    } else {
        drawNotFound(image);
    }
}

std::vector<Coin> CoinCounter::getCoins() const {
    cv::Mat grayImage;
    cv::cvtColor(mPaperSheetRegion, grayImage, cv::COLOR_BGR2GRAY);

    cv::Mat blured;
    cv::GaussianBlur(grayImage, blured, cv::Size(9, 9), 2, 2);

    std::vector<cv::Vec3f> circles;

    cv::HoughCircles(blured, circles, cv::HOUGH_GRADIENT, 1, grayImage.rows / 16, 100, 45);

    std::vector<Coin> coins;

    for (auto it = circles.begin(); it != circles.end(); it++) {
        coins.push_back(Coin(*it, mPaperSheetRegion));
    }

    return coins;
}

void CoinCounter::drawCoinsInfo() {
    for (const auto &coin : mCoins) {
        int coinValue = coin.getValue();

        coin.drawInfo(mPaperSheetRegion);
    }
}

void CoinCounter::drawTotalValue() {
    cv::Point textLocation(mPaperSheetRegion.cols / 2 - 400, mPaperSheetRegion.rows - 50);
    cv::Scalar textColor(0, 0, 0);
    int totalValue = getTotalValue();
    cv::putText(mPaperSheetRegion, "Total Value: " + std::to_string(totalValue), textLocation, cv::FONT_HERSHEY_TRIPLEX,
                3, textColor);
}

void CoinCounter::drawFrame() {
    cv::Scalar color(100, 200, 100);
    int thickness = 10;
    cv::line(mPaperSheetRegion, cv::Point(0, 0), cv::Point(0, mPaperSheetRegion.rows - 1), color, thickness);
    cv::line(mPaperSheetRegion, cv::Point(0, mPaperSheetRegion.rows - 1),
             cv::Point(mPaperSheetRegion.cols - 1, mPaperSheetRegion.rows - 1), color, thickness);
    cv::line(mPaperSheetRegion, cv::Point(mPaperSheetRegion.cols - 1, mPaperSheetRegion.rows - 1),
             cv::Point(mPaperSheetRegion.cols - 1, 0), color, thickness);
    cv::line(mPaperSheetRegion, cv::Point(mPaperSheetRegion.cols - 1, 0), cv::Point(0, 0), color, thickness);
}

void CoinCounter::drawNotFound(cv::Mat &image) const {
    cv::Point textLocation(image.cols / 2 - 400, image.rows - 50);
    cv::Scalar textColor(0, 0, 255);
    cv::putText(image, "Paper sheet not found", textLocation, cv::FONT_HERSHEY_TRIPLEX, 3, textColor);
}

cv::Point2f CoinCounter::findClosestTo(const cv::Point2f &point, const std::vector<cv::Point2f> &points) const {
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

void CoinCounter::findPaperSheetCoordinates(const cv::Mat &image) {
    cv::Mat cannyOut;
    cv::Canny(image, cannyOut, 150, 200);
    imshow("canny", cannyOut);
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

cv::Mat CoinCounter::getPaperSheetTransformationMatrix(const cv::Mat &image) const {
    std::vector<cv::Point2f> dstPoint = getCornerCoordinates(cPaperSheedWidth, cPaperSheedHeight);

    cv::Mat transformMatrix = cv::getPerspectiveTransform(mPaperSheetCoordinates.data(), dstPoint.data());

    return transformMatrix;
}

cv::Mat CoinCounter::getPaperSheetRegion(const cv::Mat &image) const {
    cv::Mat paperSheet(cPaperSheedHeight, cPaperSheedWidth, CV_8UC3);

    cv::Mat transformMatrix = getPaperSheetTransformationMatrix(image);
    cv::warpPerspective(image, paperSheet, transformMatrix, paperSheet.size());

    return paperSheet;
}

std::vector<cv::Point2f> CoinCounter::getCornerCoordinates(const int width, const int height) const {
    std::vector<cv::Point2f> corners{
            cv::Point2f(0, 0),
            cv::Point2f(0, height - 1),
            cv::Point2f(width - 1, height - 1),
            cv::Point2f(width - 1, 0)
    };
    return corners;
}

std::vector<cv::Point2f> CoinCounter::sortCornerCoordinates(const std::vector<cv::Point2f> &points,
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