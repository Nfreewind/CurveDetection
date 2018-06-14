#include "CurveDetector.h"

CurveDetector::CurveDetector() {
}

CurveDetector::~CurveDetector() {
}

void CurveDetector::detect(const std::vector<cv::Point2f>& polygon, int min_points, float epsilon, float cluster_epsilon, float min_angle, float min_radius, float max_radius, std::vector<Circle>& circles) {
	circles.clear();

	int N = polygon.size();
	int N2 = N;
	while (N2 < cluster_epsilon) N2 += N;

	std::vector<bool> used(polygon.size(), false);

	while (true) {
		int max_num_points = 0;
		Circle best_circle;
		int best_index1;

		for (int iter = 0; iter < 3000; iter++) {
			int index1 = -1;
			int index2 = -1;
			int index3 = -1;
			for (int iter2 = 0; iter2 < 3000; iter2++) {
				index1 = rand() % N;
				if (used[index1]) continue;
				index2 = (int)(rand() % (int)(cluster_epsilon * 2 + 1) - cluster_epsilon + N2) % N;
				if (used[index2]) continue;
				index3 = (int)(rand() % (int)(cluster_epsilon * 2 + 1) - cluster_epsilon + N2) % N;
				if (used[index3]) continue;
				break;
			}

			if (index1 == -1 || index2 == -1 || index3 == -1) continue;
			if (std::abs(crossProduct(polygon[index2] - polygon[index1], polygon[index3] - polygon[index1])) < 0.001) continue;

			// calculate the circle center from three points
			Circle circle = circleFromPoints(polygon[index1], polygon[index2], polygon[index3]);
			if (circle.radius < min_radius || circle.radius > max_radius) continue;

			// check whether the points are supporting this circle
			std::vector<float> angles;
			angles.push_back(std::atan2(polygon[index1].y - circle.center.y, polygon[index1].x - circle.center.x));
			int num_points = 0;
			int prev = 0;
			for (int i = 0; i < N / 2 && i - prev < cluster_epsilon; i++) {
				int idx = (index1 + i) % N;
				if (used[idx]) continue;
				if (circle.distance(polygon[idx]) < epsilon) {
					num_points++;
					prev = i;
					angles.push_back(std::atan2(polygon[idx].y - circle.center.y, polygon[idx].x - circle.center.x));
				}
			}
			prev = 0;
			for (int i = 0; i < N / 2 && i - prev < cluster_epsilon; i++) {
				int idx = (index1 - i + N) % N;
				if (used[idx]) continue;
				if (circle.distance(polygon[idx]) < epsilon) {
					num_points++;
					prev = i;
					angles.push_back(std::atan2(polygon[idx].y - circle.center.y, polygon[idx].x - circle.center.x));
				}
			}

			circle.setMinMaxAngles(angles);
			if (circle.angle_range < min_angle) continue;

			if (num_points > max_num_points) {
				max_num_points = num_points;
				best_circle = circle;
				best_index1 = index1;
			}
		}

		if (max_num_points < min_points) break;

		// updated used flag
		int prev = 0;
		for (int i = 0; i < N / 2 && i - prev < cluster_epsilon; i++) {
			int idx = (best_index1 + i) % N;
			if (used[idx]) continue;
			if (best_circle.distance(polygon[idx]) < epsilon) {
				used[idx] = true;
				best_circle.points.push_back(polygon[idx]);
				prev = i;
			}
		}
		prev = 0;
		for (int i = 0; i < N / 2 && i - prev < cluster_epsilon; i++) {
			int idx = (best_index1 - i + N) % N;
			if (used[idx]) continue;
			if (best_circle.distance(polygon[idx]) < epsilon) {
				used[idx] = true;
				best_circle.points.push_back(polygon[idx]);
				prev = i;
			}
		}

		circles.push_back(best_circle);
	}
}

Circle CurveDetector::circleFromPoints(const cv::Point2f& p1, const cv::Point2f& p2, const cv::Point2f& p3) {
	float offset = p2.x * p2.x + p2.y * p2.y;
	float bc = (p1.x * p1.x + p1.y * p1.y - offset) / 2.0;
	float cd = (offset - p3.x * p3.x - p3.y * p3.y) / 2.0;
	float det = (p1.x - p2.x) * (p2.y - p3.y) - (p2.x - p3.x)* (p1.y - p2.y);	
	if (std::abs(det) < 0.0000001) throw "Three points are collinear.";	 
	float idet = 1 / det;	
	float centerx = (bc * (p2.y - p3.y) - cd * (p1.y - p2.y)) * idet;
	float centery = (cd * (p1.x - p2.x) - bc * (p2.x - p3.x)) * idet;
	float radius = std::sqrt((p2.x - centerx) * (p2.x - centerx) + (p2.y - centery) * (p2.y - centery));	
	return Circle(cv::Point2f(centerx, centery), radius);
}

float CurveDetector::crossProduct(const cv::Point2f& a, const cv::Point2f& b) {
	return a.x * b.y - a.y * b.x;
}
