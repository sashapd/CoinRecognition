//
// Created by Oleksandr Padko on 8/7/17.
//

#ifndef COINRECOGNITION_COIN_H
#define COINRECOGNITION_COIN_H

#include <opencv2/core/core.hpp>


class Coin {
private:
    cv::Point position;
    double radius;
    int value;

    int getValue();
};


#endif //COINRECOGNITION_COIN_H
