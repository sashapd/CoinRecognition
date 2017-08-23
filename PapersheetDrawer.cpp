//
// Created by Oleksandr Padko on 8/23/17.
//

#include <opencv2/imgproc.hpp>
#include "PapersheetDrawer.h"

PapersheetDrawer::PapersheetDrawer(cv::Mat& papersheetImage) {
    mPapersheetImage = papersheetImage;
}

void PapersheetDrawer::drawCoinsInfo(const std::vector<Coin>& coins) {
    for(auto& coin : coins) {
        int coinValue = coin.getValue();
        coin.drawInfo(mPapersheetImage);
    }
}

void PapersheetDrawer::drawTotalValue(const std::vector<Coin>& coins) {
    cv::Point textLocation(mPapersheetImage.cols / 2 - 400, mPapersheetImage.rows - 50);
    cv::Scalar textColor(0, 0, 0);
    int totalValue = 0;
    for (auto &&coin : coins) {
        totalValue += coin.getValue();
    }
    cv::putText(mPapersheetImage, "Total Value: " + std::to_string(totalValue), textLocation, cv::FONT_HERSHEY_TRIPLEX, 3, textColor);
}

void PapersheetDrawer::drawFrame() {
    cv::Scalar color(100, 200, 100);
    int thickness = 10;
    cv::line(mPapersheetImage, cv::Point(0, 0), cv::Point(0, mPapersheetImage.rows - 1), color, thickness);
    cv::line(mPapersheetImage, cv::Point(0, mPapersheetImage.rows - 1), cv::Point(mPapersheetImage.cols - 1, mPapersheetImage.rows - 1), color, thickness);
    cv::line(mPapersheetImage, cv::Point(mPapersheetImage.cols - 1, mPapersheetImage.rows - 1), cv::Point(mPapersheetImage.cols - 1, 0), color, thickness);
    cv::line(mPapersheetImage, cv::Point(mPapersheetImage.cols - 1, 0), cv::Point(0, 0), color, thickness);
}