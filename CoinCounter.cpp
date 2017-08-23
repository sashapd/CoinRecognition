//
// Created by Oleksandr Padko on 8/13/17.
//

#include "CoinCounter.h"
#include <float.h>

CoinCounter::CoinCounter(const cv::Mat &papersheetImage) {
    mPapersheetImage = papersheetImage;
    findCoins();
}

int CoinCounter::getTotalValue() const {
    int totalValue = 0;
    for (auto it = mCoins.begin(); it != mCoins.end(); it++) {
        totalValue += it->getValue();
    }
    return totalValue;
}

std::vector<Coin> CoinCounter::getCoins() const {
    return mCoins;
}

void CoinCounter::findCoins() {
    cv::Mat grayImage;
    cv::cvtColor(mPapersheetImage, grayImage, cv::COLOR_BGR2GRAY);

    cv::Mat blured;
    cv::GaussianBlur(grayImage, blured, cv::Size(9, 9), 2, 2);

    std::vector<cv::Vec3f> circles;

    cv::HoughCircles(blured, circles, cv::HOUGH_GRADIENT, 1, grayImage.rows / 16, 100, 45);

    for (auto &&circle : circles) {
        mCoins.emplace_back(circle, mPapersheetImage);
    }
}
