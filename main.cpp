#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "CoinCounter.h"

int main() {
    cv::Mat image = cv::imread("coins3.jpg", 1);

    CoinCounter counter(image);
    counter.drawPaperRegion(image);

    cv::imshow("image", image);
    cv::waitKey(0);

    return 0;
}