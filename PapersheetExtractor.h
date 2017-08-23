//
// Created by Oleksandr Padko on 8/23/17.
//

#ifndef COINRECOGNITION_PAPERSHEETEXTRACTION_H
#define COINRECOGNITION_PAPERSHEETEXTRACTION_H


#include <opencv2/core/types.hpp>
#include <opencv2/core/mat.hpp>

class PapersheetExtractor {
public:
    PapersheetExtractor(const cv::Mat &image);

    cv::Mat getPaperSheetRegion();

    cv::Mat putBackPapersheet(cv::Mat &paperSheet) const;

private:
    cv::Mat mImage;
    std::vector<cv::Point2f> mPaperSheetCoordinates;
    bool foundPaperSheet = false;

    const int cPaperSheedHeight = 1000;
    const int cPaperSheedWidth = cPaperSheedHeight * 1.4142;

    void findPaperSheetCoordinates(const cv::Mat &image);

    cv::Mat getPaperSheetTransformationMatrix(const cv::Mat &image) const;

    std::vector<cv::Point2f> getCornerCoordinates(const int width, const int height) const;

    static bool comparePointsClockwise(cv::Point2f a, cv::Point2f b, cv::Point2f center);

    std::vector<cv::Point2f> sortCornerCoordinates(std::vector<cv::Point2f> points) const;
};


#endif //COINRECOGNITION_PAPERSHEETEXTRACTION_H
