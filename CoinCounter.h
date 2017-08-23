//
// Created by Oleksandr Padko on 8/13/17.
//

#ifndef COINRECOGNITION_COINCOUNTER_H
#define COINRECOGNITION_COINCOUNTER_H


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "Coin.h"

class CoinCounter {
public:
    CoinCounter(const cv::Mat &image);

    int getTotalValue() const;

    std::vector<Coin> getCoins() const;

private:
    std::vector<Coin> mCoins;
    cv::Mat mPapersheetImage;

    void findCoins();
};


#endif //COINRECOGNITION_COINCOUNTER_H
