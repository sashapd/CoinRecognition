#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "CoinCounter.h"
#include "PapersheetExtractor.h"
#include "PapersheetDrawer.h"


int main() {
    cv::Mat image = cv::imread("coins01.jpg", 1);

    const int imSize = 1500;
    cv::resize(image, image, cv::Size(imSize, imSize * image.rows / image.cols));

    PapersheetExtractor extractor(image);
    cv::Mat paperSheet = extractor.getPaperSheetRegion();
    
    CoinCounter counter(paperSheet);
    std::vector<Coin> coins = counter.getCoins();

    PapersheetDrawer drawer(paperSheet);
    drawer.drawFrame();
    drawer.drawCoinsInfo(coins);
    drawer.drawTotalValue(coins);

    image = extractor.putBackPapersheet(paperSheet);

    cv::imshow("paperSheet", image);
    cv::waitKey(0);

    return 0;
}