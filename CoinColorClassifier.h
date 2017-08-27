//
// Created by Oleksandr Padko on 8/28/17.
//

#ifndef COINRECOGNITION_COINCOLORCLASSIFIER_H
#define COINRECOGNITION_COINCOLORCLASSIFIER_H


#include <opencv2/core/types.hpp>
#include "Color.h"

class CoinColorClassifier {
public:
    CoinColorClassifier();

    Color classifyColor(cv::Scalar color);

private:
    std::vector<cv::Scalar> silverTrainData;
    std::vector<cv::Scalar> yellowTrainData;
    static bool cmpScalar(cv::Scalar s1, cv::Scalar s2, cv::Scalar t);

};


#endif //COINRECOGNITION_COINCOLORCLASSIFIER_H
