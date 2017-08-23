//
// Created by Oleksandr Padko on 8/23/17.
//

#ifndef COINRECOGNITION_PAPERSHEETDRAWER_H
#define COINRECOGNITION_PAPERSHEETDRAWER_H


#include <opencv2/core/mat.hpp>
#include "Coin.h"

class PapersheetDrawer {
public:
    PapersheetDrawer(cv::Mat &papersheetImage);

    void drawCoinsInfo(const std::vector<Coin>& coins);

    void drawTotalValue(const std::vector<Coin>& coins);

    void drawFrame();

private:
    cv::Mat mPapersheetImage;
};


#endif //COINRECOGNITION_PAPERSHEETDRAWER_H
