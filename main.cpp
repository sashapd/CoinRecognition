#include "curl/curl.h"
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "CoinCounter.h"
#include "PapersheetExtractor.h"



int main() {
    cv::Mat image = cv::imread("2017-08-21 15.00.53.jpg", 1);
    PapersheetExtractor extractor(image);
    cv::Mat i = extractor.getPaperSheetRegion();
    //CoinCounter counter(image);
    //counter.drawPaperRegion(image);
    cv::imshow("image", i);
    cv::waitKey(0);

    return 0;
}