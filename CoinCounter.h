//
// Created by Oleksandr Padko on 8/13/17.
//

#ifndef COINRECOGNITION_COINCOUNTER_H
#define COINRECOGNITION_COINCOUNTER_H


#include <opencv2/core/mat.hpp>
#include "Coin.h"

class CoinCounter {
public:
    cv::Mat paperSheetRegion;
    std::vector<cv::Point2i> paperSheetCoordinates;
    std::vector<Coin> coins;

    int getTotalValue() const;

};


#endif //COINRECOGNITION_COINCOUNTER_H
