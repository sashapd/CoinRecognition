//
// Created by Oleksandr Padko on 8/7/17.
//

#include "Coin.h"
#include <map>
#include <opencv2/imgproc.hpp>
#include <opencv/cv.hpp>
#include <opencv2/ml.hpp>
#include <fstream>

CoinColorClassifier Coin::classifier;

Coin::Coin(const cv::Vec3f &circle, const cv::Mat &image) {
    mPosition.x = cvRound(circle[0]);
    mPosition.y = cvRound(circle[1]);
    mRadius = circle[2];

    Color color = getCoinColor(image);
    double radiusInMeters = pixelsToMeters(mRadius, image);
    mValue = coinValueByRadius(radiusInMeters * 2, color);
}

double Coin::pixelsToMeters(const double pixels, const cv::Mat &a4Paper) const {
    const double a4Width = 0.297;
    return pixels / a4Paper.cols * a4Width;
}

int Coin::coinValueByRadius(const double diameter, Color color) const {
    std::map<double, int> coins;
    std::map<double, int> yellowCoins = {{0.0163, 10},
                                         {0.0208, 25},
                                         {0.023,  50},
                                         {0.026,  100}};
    std::map<double, int> silverCoins = {{0.016,  1},
                                         {0.0173, 2},
                                         {0.024,  5}};
    if (color == YELLOW) {
        coins = yellowCoins;
    } else if (color == SILVER) {
        coins = silverCoins;
    } else {
        coins.insert(yellowCoins.begin(), yellowCoins.end());
        coins.insert(silverCoins.begin(), silverCoins.end());
    }

    double minDiff = fabs(coins.begin()->first - diameter);
    int mostLikelyCoin = coins.begin()->second;
    for (const auto &element : coins) {
        double diameterDiff = fabs(element.first - diameter);
        if (diameterDiff < minDiff) {
            minDiff = diameterDiff;
            mostLikelyCoin = element.second;
        }
    }
    return mostLikelyCoin;
}

void Coin::drawInfo(cv::Mat &image) const {
    drawOutline(image);
    drawValue(image);
}

void Coin::drawOutline(cv::Mat &image) const {
    cv::Scalar color(0, 255, 0);
    cv::circle(image, mPosition, 3, color, -1, 8, 0);
    cv::circle(image, mPosition, mRadius, color, 2, 8, 0);
}

void Coin::drawValue(cv::Mat &image) const {
    int xOffset;
    if (mValue < 10) {
        xOffset = 20;
    } else {
        xOffset = 42;
    }
    cv::Point textLocation(mPosition.x - xOffset, mPosition.y - mRadius);
    cv::Scalar textColor(0, 0, 0);
    cv::putText(image, std::to_string(mValue), textLocation, cv::FONT_HERSHEY_PLAIN, 4, textColor);
}

int Coin::getValue() const {
    return mValue;
}

cv::Scalar Coin::getMeanLabColor(const cv::Mat &image) const {
    cv::Rect boundingRect(mPosition.x - mRadius, mPosition.y - mRadius, mRadius * 2 + 1, mRadius * 2 + 1);
    cv::Mat circleROI(image, boundingRect);

    cv::Mat circularMask = cv::Mat::zeros(circleROI.size(), CV_8UC1);
    cv::circle(circularMask, circularMask.size() / 2, mRadius, 255, -1, 8, 0);

    cv::Scalar meanColor = cv::mean(circleROI, circularMask);

    return meanColor;
}

Color Coin::getCoinColor(const cv::Mat &image) {
    cv::Scalar meanColor = getMeanLabColor(image);
    Color c = Coin::classifier.classifyColor(meanColor);


    return c;
}

