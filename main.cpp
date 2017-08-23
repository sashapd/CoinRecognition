#include "curl/curl.h"
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "CoinCounter.h"
#include "PapersheetExtractor.h"
#include "PapersheetDrawer.h"


int main() {
    cv::Mat image = cv::imread("coins6.jpg", 1);
    PapersheetExtractor extractor(image);
    cv::Mat paperSheet = extractor.getPaperSheetRegion();
    CoinCounter counter(paperSheet);
    std::vector<Coin> coins = counter.getCoins();

    PapersheetDrawer drawer(paperSheet);
    drawer.drawFrame();
    drawer.drawCoinsInfo(coins);
    drawer.drawTotalValue(coins);

    cv::imshow("paperSheet", paperSheet);
    cv::waitKey(0);

    return 0;
}