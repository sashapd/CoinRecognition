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