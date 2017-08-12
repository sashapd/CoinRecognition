//
// Created by Oleksandr Padko on 8/7/17.
//

#include "Coin.h"
#include <map>
#include <opencv2/imgproc.hpp>


Coin::Coin(cv::Vec3f circle, const cv::Mat& image) {
    position.x = cvRound(circle[0]);
    position.y = cvRound(circle[1]);
    radius = circle[2];

    Color color = getCoinColor(image);
    double radiusInMeters = pixelsToMeters(radius, image);
    value = coinValueByRadius(radiusInMeters * 2, color);
}

double Coin::pixelsToMeters(const double pixels, const cv::Mat& a4Paper) const {
    const double a4Width = 0.297;
    return pixels / a4Paper.cols * a4Width;
}

int Coin::coinValueByRadius(const double diameter, Color color) const {
    std::map<double, int> coins;
    if(color == YELLOW) {
        coins = {{0.0163, 10}, {0.0208, 25}, {0.023, 50}, {0.026, 100}};
    } else if (color == SILVER) {
        coins = {{0.016, 1}, {0.0173, 2}, {0.024, 5}};
    }

    double minDiff = fabs(coins.begin()->first - diameter);
    int mostLikelyCoin = coins.begin()->second;
    for(const auto& element : coins) {
        double diameterDiff = fabs(element.first - diameter);
        if(diameterDiff < minDiff) {
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
    cv::Scalar color(0,255,0);
    cv::circle(image, position, 3, color, -1, 8, 0 );
    cv::circle(image, position, radius, color, 2, 8, 0 );
}

void Coin::drawValue(cv::Mat &image) const {
    int xOffset;
    if(value < 10) {
        xOffset = 20;
    } else {
        xOffset = 42;
    }
    cv::Point textLocation(position.x - xOffset, position.y - radius);
    cv::Scalar textColor(0, 0, 0);
    cv::putText(image, std::to_string(value), textLocation, cv::FONT_HERSHEY_PLAIN, 4, textColor);
}

int Coin::getValue() const {
    return value;
}

Coin::Color Coin::getCoinColor(const cv::Mat &image) const {
    cv::Rect boundingRect(position.x - radius, position.y - radius, radius * 2 + 1, radius * 2 + 1);
    cv::Mat circleROI(image, boundingRect);

    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

    cv::Mat yellowMask;
    cv::inRange(hsv, cv::Scalar(30, 40, 40), cv::Scalar(45, 255, 255), yellowMask);

    return SILVER;
}

