#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <map>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"

std::vector<cv::Vec3f> getCoinLocations(const cv::Mat& image) {
    cv::Mat newImg;
    image.convertTo(newImg, -1, 1.1, 1);

    cv::Mat grayImage;
    cv::cvtColor(newImg, grayImage, cv::COLOR_BGR2GRAY);

    cv::Mat blured;
    cv::GaussianBlur(grayImage, blured, cv::Size(9, 9), 2, 2 );

    std::vector<cv::Vec3f> circles;

    cv::HoughCircles( blured, circles, cv::HOUGH_GRADIENT, 1, grayImage.rows/16, 70, 45);

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

cv::Point2i findClosestTo(const cv::Point2i& point, const std::vector<cv::Point2i>& points) {
    double minDist = 1000000;
    cv::Point2i closestPoint;
    for (const auto &p : points) {
        double distance = pow(pow(point.x - p.x, 2) + pow(point.y - p.y, 2), 0.5);
        if(distance < minDist) {
            closestPoint = p;
            minDist = distance;
        }
    }
    return closestPoint;
}


std::vector<cv::Point2i> getPaperSheetCoordinates(const cv::Mat& image) {
    cv::Mat hsvImg, thresholdedImg;
    cv::cvtColor(image, hsvImg, cv::COLOR_BGR2HSV);

    cv::inRange(hsvImg, cv::Scalar(0, 0, 120), cv::Scalar(255, 75, 255), thresholdedImg);

    cv::Mat t;
    cv::bitwise_and(image, image, t, thresholdedImg);

    cv::Mat blured;
    cv::GaussianBlur(t, blured, cv::Size(9, 9), 3);

    std::vector<cv::Point2i> corners;
    int maxCorners = 100;
    double qualityLevel = 0.01;
    double minDistance = 10;
    int blockSize = 3;
    bool useHarrisDetector = true;
    double k = 0.04;

    cv::Mat grayImg;
    cv::cvtColor(blured, grayImg, cv::COLOR_BGR2GRAY);
    /// Apply corner detection
    cv::goodFeaturesToTrack( grayImg,
                         corners,
                         maxCorners,
                         qualityLevel,
                         minDistance,
                         thresholdedImg,
                         blockSize,
                         useHarrisDetector,
                         k );

    cv::Point2f imageCorners[4] = {
                cv::Point2f(0, 0),
                cv::Point2f(0, image.rows - 1),
                cv::Point2f(image.cols - 1, image.rows - 1),
                cv::Point2f(image.cols - 1, 0)
    };

    std::vector<cv::Point2i> extremes;
    for(int i = 0; i < 4; i++) {
        extremes.push_back(findClosestTo(imageCorners[i], corners));
    }

    //if(cv::norm(extremes[2] - extremes[1]) < cv::norm(extremes[1] - extremes[0])) {
        //extremes.push_back(extremes[0])
        //extremes.erase(extremes.begin());
    //}

    return extremes;
}

const int paperSheedHeight = 1000;
const int paperSheedWidth = paperSheedHeight * 1.41421356237;

cv::Mat getPaperSheetTransformationMatrix(const cv::Mat& image) {
    std::vector<cv::Point2i> coordinates = getPaperSheetCoordinates(image);

    cv::Point2f srcPoint[4];
    std::copy(coordinates.begin(), coordinates.end(), srcPoint);

    cv::Point2f dstPoint[4] = {
            cv::Point2f(0, 0),
            cv::Point2f(0, paperSheedHeight - 1),
            cv::Point2f(paperSheedWidth - 1, paperSheedHeight - 1),
            cv::Point2f(paperSheedWidth - 1, 0)
    };

    cv::Mat transformMatrix = cv::getPerspectiveTransform(srcPoint, dstPoint);

    return transformMatrix;
}

cv::Mat getPaperSheetRegion(const cv::Mat& image) {
    cv::Mat paperSheet(paperSheedHeight, paperSheedWidth, CV_8UC3);

    cv::Mat transformMatrix = getPaperSheetTransformationMatrix(image);
    cv::warpPerspective(image, paperSheet, transformMatrix, paperSheet.size());

    return paperSheet;
}

int main() {
    cv::Mat image = cv::imread("coins7.jpg", 1);
    cv::Mat region = getPaperSheetRegion(image);

    std::vector<cv::Vec3f> circles = getCoinLocations(region);


    for(const auto& circle : circles) {
        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        int radius = cvRound(circle[2]);

        double radiusInMeters = pixelsToMeters(radius, region);
        int coinValue = coinValueByRadius(radiusInMeters * 2);

        std::cout << coinValue << " : " << radiusInMeters * 2 << std::endl;
        drawCoinInfo(region, center, radius, coinValue);
    }

    cv::Mat regionResized;
    cv::resize(region, regionResized, cv::Size(), 0.75, 0.75);
    imshow("Coin Recognition", regionResized );
    cv::waitKey(0);
    return 0;
}