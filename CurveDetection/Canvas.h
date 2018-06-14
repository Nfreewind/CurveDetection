#ifndef CANVAS_H
#define CANVAS_H

#include <vector>
#include <QWidget>
#include <QKeyEvent>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "CurveDetector.h"

class Polygon {
public:
	std::vector<cv::Point2f> contour;
	std::vector<std::vector<cv::Point2f>> holes;
};

class Canvas : public QWidget {
private:
	static enum { TOP_LEFT = 0, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };

private:
	QImage orig_image;
	QImage image;
	float image_scale;
	std::vector<Polygon> polygons;
	std::vector<Circle> circles;

	bool ctrlPressed;
	bool shiftPressed;
	
public:
	Canvas(QWidget *parent = NULL);

	void loadImage(const QString& filename);
	void detectContours();
	void detectCurves(int num_iterations, int min_points, float max_error_ratio_to_radius, float cluster_epsilon, float min_angle, float min_radius, float max_radius);
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent* e);
	void resizeEvent(QResizeEvent *e);
};

#endif // CANVAS_H
