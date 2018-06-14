#include "Canvas.h"
#include <QPainter>
#include <iostream>
#include <QFileInfoList>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QResizeEvent>

Canvas::Canvas(QWidget *parent) : QWidget(parent) {
	ctrlPressed = false;
	shiftPressed = false;
}

void Canvas::loadImage(const QString& filename) {
	orig_image = QImage(filename).convertToFormat(QImage::Format_Grayscale8);
	image_scale = std::min((float)width() / orig_image.width(), (float)height() / orig_image.height());
	image = orig_image.scaled(orig_image.width() * image_scale, orig_image.height() * image_scale);

	polygons.clear();
	circles.clear();

	update();
}

void Canvas::detectContours() {
	if (orig_image.isNull()) return;

	polygons.clear();

	cv::Mat mat = cv::Mat(orig_image.height(), orig_image.width(), CV_8UC1, orig_image.bits(), orig_image.bytesPerLine()).clone();
	cv::threshold(mat, mat, 40, 255, cv::THRESH_BINARY);

	// extract contours
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	//cv::findContours(mat, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	cv::findContours(mat, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));

	for (int i = 0; i < hierarchy.size(); i++) {
		if (hierarchy[i][3] != -1) continue;
		if (contours[i].size() < 3) continue;

		Polygon polygon;
		polygon.contour.resize(contours[i].size());
		for (int j = 0; j < contours[i].size(); j++) {
			polygon.contour[j] = cv::Point2f(contours[i][j].x, contours[i][j].y);
		}

		if (polygon.contour.size() >= 3) {
			// obtain all the holes inside this contour
			int hole_id = hierarchy[i][2];
			while (hole_id != -1) {
				std::vector<cv::Point2f> hole;
				hole.resize(contours[hole_id].size());
				for (int j = 0; j < contours[hole_id].size(); j++) {
					hole[j] = cv::Point2f(contours[hole_id][j].x, contours[hole_id][j].y);
				}
				polygon.holes.push_back(hole);
				hole_id = hierarchy[hole_id][0];
			}

			polygons.push_back(polygon);
		}
	}
}

void Canvas::detectCurves(int num_iterations, int min_points, float max_error_ratio_to_radius, float cluster_epsilon, float min_angle, float min_radius, float max_radius) {
	circles.clear();

	if (polygons.size() == 0) detectContours();

	CurveDetector cd;
	for (int i = 0; i < polygons.size(); i++) {
		if (polygons[i].contour.size() < 100) continue;

		std::vector<Circle> results;
		cd.detect(polygons[i].contour, num_iterations, min_points, max_error_ratio_to_radius, cluster_epsilon, min_angle, min_radius, max_radius, results);
		circles.insert(circles.end(), results.begin(), results.end());
	}
}

void Canvas::keyPressEvent(QKeyEvent* e) {
	ctrlPressed = false;
	shiftPressed = false;

	if (e->modifiers() & Qt::ControlModifier) {
		ctrlPressed = true;
	}
	if (e->modifiers() & Qt::ShiftModifier) {
		shiftPressed = true;
	}

	switch (e->key()) {
	case Qt::Key_Space:
		break;
	}

	update();
}

void Canvas::keyReleaseEvent(QKeyEvent* e) {
	switch (e->key()) {
	case Qt::Key_Control:
		ctrlPressed = false;
		break;
	case Qt::Key_Shift:
		shiftPressed = false;
		break;
	default:
		break;
	}
}

void Canvas::paintEvent(QPaintEvent *event) {
	if (!image.isNull()) {
		QPainter painter(this);
		painter.drawImage(0, 0, image);

		painter.setPen(QPen(QColor(0, 0, 255), 3));
		for (auto& polygon : polygons) {
			QPolygon pgon;
			for (auto& p : polygon.contour) {
				pgon.push_back(QPoint(p.x * image_scale, p.y * image_scale));
			}
			painter.drawPolygon(pgon);
			for (auto& hole : polygon.holes) {
				QPolygon pgon;
				for (auto& p : hole) {
					pgon.push_back(QPoint(p.x * image_scale, p.y * image_scale));
				}
				painter.drawPolygon(pgon);
			}
		}

		for (auto& circle : circles) {
			painter.setPen(QPen(QColor(255, 255, 0), 3));
			painter.drawArc((circle.center.x - circle.radius) * image_scale, (circle.center.y - circle.radius) * image_scale, circle.radius * 2 * image_scale, circle.radius * 2 * image_scale, -circle.start_angle / CV_PI * 180 * 16, -circle.angle_range / CV_PI * 180 * 16);
			//painter.drawEllipse(QPointF(circle.center.x * image_scale, circle.center.y * image_scale), circle.radius * image_scale, circle.radius * image_scale);

			painter.setPen(QPen(QColor(255, 0, 0), 1));
			for (int i = 0; i < circle.points.size(); i++) {
				painter.drawRect(circle.points[i].x * image_scale - 1, circle.points[i].y * image_scale - 1, 3, 3);
			}
		}
	}
}

void Canvas::mousePressEvent(QMouseEvent* e) {
	update();
}

void Canvas::resizeEvent(QResizeEvent *e) {
	if (!orig_image.isNull()) {
		image_scale = std::min((float)width() / orig_image.width(), (float)height() / orig_image.height());
		image = orig_image.scaled(orig_image.width() * image_scale, orig_image.height() * image_scale);
	}
}

