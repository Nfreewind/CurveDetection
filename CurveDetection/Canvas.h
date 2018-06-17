#ifndef CANVAS_H
#define CANVAS_H

#include <vector>
#include <QWidget>
#include <QKeyEvent>
#include "../CurveDetectionNoGUI/CurveDetector.h"
#include "../CurveDetectionNoGUI/LineDetector.h"
#include "../CurveDetectionNoGUI/MeanShift.h"
#include "../CurveDetectionNoGUI/OrientationEstimator.h"

class Canvas : public QWidget {
private:
	static enum { TOP_LEFT = 0, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };

private:
	QImage orig_image;
	QImage image;
	float image_scale;
	std::vector<Polygon> polygons;
	std::vector<Circle> circles;
	std::vector<Line> lines;

	bool ctrlPressed;
	bool shiftPressed;
	
public:
	Canvas(QWidget *parent = NULL);

	void loadImage(const QString& filename);
	void detectContours();
	void detectCurves(int num_iterations, int min_points, float max_error_ratio_to_radius, float cluster_epsilon, float min_angle, float min_radius, float max_radius);
	void detectLines(int num_iterations, int min_points, float max_error, float cluster_epsilon, float min_length);
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent* e);
	void resizeEvent(QResizeEvent *e);
};

#endif // CANVAS_H
