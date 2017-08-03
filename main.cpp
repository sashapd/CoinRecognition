#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <map>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"

std::vector<cv::Vec3f> getCoinLocations(const cv::Mat& image) {
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    GaussianBlur(grayImage, grayImage, cv::Size(9, 9), 2, 2 );

    std::vector<cv::Vec3f> circles;

    cv::HoughCircles( grayImage, circles, cv::HOUGH_GRADIENT, 1, grayImage.rows/32, 150, 50);

    return circles;
}

int coinValueByRadius(const double diameter) {
    const std::map<double, int> coins {{0.016, 1},
                                       {0.0173, 2},
                                       {0.024, 5},
                                       {0.0163, 10},
                                       {0.0208, 25},
                                       {0.023, 50},
                                       {0.026, 100}};
    double minDiff = fabs(coins.begin()->first - diameter);
    int mostLikelyCoin = coins.begin()->second;
    for(const auto& element : coins) {
        double diameterDiff = fabs(element.first - diameter);
        if(diameterDiff < minDiff) {
            minDiff = diameterDiff;
            mostLikelyCoin = element.second;
        }
    }
    return mostLikelyCoin;
}

double pixelsToMeters(const int pixels, const cv::Mat& a4Paper) {
    const double a4Width = 0.297;
    return (double) pixels / a4Paper.cols * a4Width;
}

void drawCoinValue(cv::Mat& image, const cv::Point& center, int radius, int value) {
    cv::Point textLocation(center.x - 15, center.y - radius);
    cv::Scalar textColor(255, 255, 255);
    cv::putText(image, std::to_string(value), textLocation, cv::FONT_HERSHEY_PLAIN, 2, textColor);
}

void drawCoinLocation(cv::Mat& image, const cv::Point& center, int radius) {
    cv::circle(image, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
    cv::circle(image, center, radius, cv::Scalar(0,255,0), 1, 8, 0 );
}

void drawCoinInfo(cv::Mat& image, const cv::Point& center, int radius, int value) {
    drawCoinValue(image, center, radius, value);
    drawCoinLocation(image, center, radius);
}

std::vector<cv::Point2i> getPaperSheetCoordinates(const cv::Mat& image) {
    

}

cv::Mat getPaperSheetTransformationMatrix(const cv::Mat& image) {
    //getting corner coordinates
    std::vector<cv::Point2i> coordinates = getPaperSheetCoordinates(image);

    cv::Point2f srcPoint[3];
    cv::Point2f dstPoint[3];

    srcPoint[1] = coordinates[0];
    srcPoint[1] = coordinates[1];
    srcPoint[1] = coordinates[2];

    dstPoint[0] = cv::Point2f(0, 0);
    dstPoint[1] = cv::Point2f(image.cols - 1, 0 );
    dstPoint[2] = cv::Point2f(0, image.rows - 1 );

    cv::Mat transformMatrix = cv::getAffineTransform(srcPoint, dstPoint);

    return transformMatrix;
}

cv::Mat getPaperSheetRegion(const cv::Mat& image) {
    const int sheetSize = 1000;
    cv::Mat paperSheet = cv::Mat::zeros(sheetSize, (int)(sheetSize * 1.4142), image.type());

    cv::Mat transformMatrix = getPaperSheetTransformationMatrix(image);

    cv::warpAffine(image, paperSheet, transformMatrix, paperSheet.size());

    return paperSheet;
}

int main() {
    cv::Mat image = cv::imread("coins2.jpg", 1);

    std::vector<cv::Vec3f> circles = getCoinLocations(image);

    for(const auto& circle : circles) {
        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        int radius = cvRound(circle[2]);

        double radiusInMeters = pixelsToMeters(radius, image);
        int coinValue = coinValueByRadius(radiusInMeters * 2);

        std::cout << coinValue << " : " << radiusInMeters * 2 << std::endl;
        drawCoinInfo(image, center, radius, coinValue);
    }

    imshow("Coin Recognition", image );

    cv::waitKey(0);
    return 0;
}