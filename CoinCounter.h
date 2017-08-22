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

    void drawPaperRegion(cv::Mat &image) const;

private:
    std::vector<Coin> mCoins;

    std::vector<Coin> getCoins() const;

    void drawCoinsInfo();

    void drawTotalValue();

    void drawFrame();

    void drawNotFound(cv::Mat &image) const;
};


#endif //COINRECOGNITION_COINCOUNTER_H
