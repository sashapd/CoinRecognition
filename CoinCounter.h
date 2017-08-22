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
    cv::Mat mPaperSheetRegion;
    std::vector<cv::Point2f> mPaperSheetCoordinates;
    std::vector<Coin> mCoins;
    bool foundPaperSheet = false;

    const int cPaperSheedHeight = 1000;
    const int cPaperSheedWidth = cPaperSheedHeight * 1.4142;

    std::vector<Coin> getCoins() const;

    void drawCoinsInfo();

    void drawTotalValue();

    void drawFrame();

    void drawNotFound(cv::Mat &image) const;

    cv::Point2f findClosestTo(const cv::Point2f &point, const std::vector<cv::Point2f> &points) const;

    void findPaperSheetCoordinates(const cv::Mat &image);

    cv::Mat getPaperSheetTransformationMatrix(const cv::Mat &image) const;

    cv::Mat getPaperSheetRegion(const cv::Mat &image) const;

    std::vector<cv::Point2f> getCornerCoordinates(const int width, const int height) const;

    std::vector<cv::Point2f>
    sortCornerCoordinates(const std::vector<cv::Point2f> &points, const std::vector<cv::Point2f> &imageCorners) const;
};


#endif //COINRECOGNITION_COINCOUNTER_H
