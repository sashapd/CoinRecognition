//
// Created by Oleksandr Padko on 8/28/17.
//

#include <fstream>
#include "CoinColorClassifier.h"

CoinColorClassifier::CoinColorClassifier() {
    std::fstream silverColors("silver_colors.txt", std::ios_base::in);
    std::fstream yellowColors("yellow_colors.txt", std::ios_base::in);
    double b, g, r;
    while (silverColors >> b >> g >> r) {
        silverTrainData.emplace_back(b, g, r);
    }
    while (yellowColors >> b >> g >> r) {
        yellowTrainData.emplace_back(b, g, r);
    }

}

Color CoinColorClassifier::classifyColor(cv::Scalar color) {
    std::sort(silverTrainData.begin(), silverTrainData.end(),
              std::bind(CoinColorClassifier::cmpScalar, std::placeholders::_1, std::placeholders::_2, color));
    std::sort(yellowTrainData.begin(), yellowTrainData.end(),
              std::bind(cmpScalar, std::placeholders::_1, std::placeholders::_2, color));

    int k = 3;
    auto silverIterator = silverTrainData.begin();
    auto yellowIterator = yellowTrainData.begin();

    int yellowN = 0;
    int silverN = 0;

    for (int i = 0; i < k; ++i) {
        double sDist = cv::norm(*silverIterator - color);
        double yDist = cv::norm(*yellowIterator - color);
        if (sDist < yDist) {
            silverN++;
            silverIterator++;
        } else {
            yellowN++;
            yellowIterator++;
        }
    }

    if (yellowN > silverN) {
        return YELLOW;
    } else if (yellowN < silverN) {
        return SILVER;
    }
    return UNKNOWN;
}

bool CoinColorClassifier::cmpScalar(cv::Scalar s1, cv::Scalar s2, cv::Scalar t) {
    double d1 = cv::norm(s1 - t);
    double d2 = cv::norm(s2 - t);
    return d1 < d2;
}