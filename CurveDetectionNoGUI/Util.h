#pragma once

#include <vector>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

class Polygon {
public:
	std::vector<cv::Point2f> contour;
	std::vector<std::vector<cv::Point2f>> holes;
};

std::vector<Polygon> findContours(const cv::Mat& image);
