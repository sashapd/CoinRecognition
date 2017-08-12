//
// Created by Oleksandr Padko on 8/7/17.
//

#ifndef COINRECOGNITION_COIN_H
#define COINRECOGNITION_COIN_H

#include <opencv2/core/core.hpp>


class Coin {
public:
    Coin(cv::Vec3f, const cv::Mat& image);

    int getValue() const;
    void drawInfo(cv::Mat& image) const;
private:
    cv::Point position;
    double radius;
    int value;

    enum Color {
        YELLOW,
        SILVER
    };

    Color getCoinColor(const cv::Mat& image) const;
    double pixelsToMeters(const double pixels, const cv::Mat &a4Paper) const;
    int coinValueByRadius(const double diameter, Color color) const;
    void drawOutline(cv::Mat& image) const;
    void drawValue(cv::Mat& image) const;
};


#endif //COINRECOGNITION_COIN_H
