#pragma once
#include "dconfig.h"

struct MatchResult {
    double minVal; 
    double maxVal;
    cv::Point maxLocation;
    cv::Point minLocation;
    MatchResult() : minVal(0), maxVal(0), maxLocation(cv::Point(-1, -1)), minLocation(cv::Point(-1, -1)) {}
};

double compareImagesByNorm(const cv::Mat &img1, const cv::Mat &img2) {
    cv::Mat diff;
    cv::absdiff(img1, img2, diff); // Compute absolute difference
    double normVal = cv::norm(diff, cv::NORM_L2); // Use L2 norm for comparison
    return normVal;
}

cv::Mat createSubImage(cv::Mat imgage, int x, int y, int width, int height) {
    /* here x and y are the coordinates of the upper left corner */
    cv::Rect roi(x, y, width, height);

    // Create the subimage (ROI) using the rectangle
    return imgage(roi);
}

cv::Mat createMaskFromTemplate(const cv::Mat& templ) {
    cv::Mat grayTemplate, mask;
    cv::cvtColor(templ, grayTemplate, cv::COLOR_BGR2GRAY);
    cv::threshold(grayTemplate, mask, 1, 255, cv::THRESH_BINARY);
    return mask;
}

MatchResult findTemplateWithMask(const cv::Mat& img, const cv::Mat& templ, const cv::Mat& mask) {
    MatchResult result;
    cv::Mat resultMat;
    
    /* reduce the resolution */
    cv::Mat downscaledImg, downscaledTempl, downscaledMask;
    double down_size_coef = config_down_size_coef();
    cv::resize(img, downscaledImg, cv::Size(), down_size_coef, down_size_coef);
    cv::resize(templ, downscaledTempl, cv::Size(), down_size_coef, down_size_coef);
    cv::resize(mask, downscaledMask, cv::Size(), down_size_coef, down_size_coef);

    /* other methods are: cv::TM_CCOEFF_NORMED, cv::TM_CCORR_NORMED, cv::TM_SQDIFF_NORMED, ... */
    cv::matchTemplate(downscaledImg, downscaledTempl, resultMat, cv::TM_SQDIFF_NORMED, downscaledMask);

    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(resultMat, &minVal, &maxVal, &minLoc, &maxLoc);

    /* Capture the locations */
    result.minVal = minVal;
    result.maxVal = minVal;
    result.maxLocation = maxLoc;
    result.minLocation = minLoc;

    /* Scale locations back to the original image coordinates */
    result.minLocation.x = int(result.minLocation.x / down_size_coef);
    result.minLocation.y = int(result.minLocation.y / down_size_coef);
    result.maxLocation.x = int(result.maxLocation.x / down_size_coef);
    result.maxLocation.y = int(result.maxLocation.y / down_size_coef);

    
    return result;
}

