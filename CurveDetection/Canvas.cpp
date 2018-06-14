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
	circles.clear();

	cv::Mat mat = cv::Mat(orig_image.height(), orig_image.width(), CV_8UC1, orig_image.bits(), orig_image.bytesPerLine()).clone();
	polygons = CurveDetector::findContours(mat);
}

void Canvas::detectCurves(int num_iterations, int min_points, float max_error_ratio_to_radius, float cluster_epsilon, float min_angle, float min_radius, float max_radius) {
	circles.clear();

	if (polygons.size() == 0) detectContours();

	for (int i = 0; i < polygons.size(); i++) {
		if (polygons[i].contour.size() < 100) continue;

		std::vector<Circle> results;
		CurveDetector::detect(polygons[i].contour, num_iterations, min_points, max_error_ratio_to_radius, cluster_epsilon, min_angle, min_radius, max_radius, results);
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

		if (polygons.size() == 0) painter.drawImage(0, 0, image);

		painter.setPen(QPen(QColor(0, 0, 0), 1));
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
			painter.setPen(QPen(QColor(0, 0, 255), 3));
			painter.drawArc((circle.center.x - circle.radius) * image_scale, (circle.center.y - circle.radius) * image_scale, circle.radius * 2 * image_scale, circle.radius * 2 * image_scale, -circle.start_angle / CV_PI * 180 * 16, -circle.angle_range / CV_PI * 180 * 16);

			/*
			painter.setPen(QPen(QColor(255, 0, 0), 1));
			for (int i = 0; i < circle.points.size(); i++) {
				painter.drawRect(circle.points[i].x * image_scale - 1, circle.points[i].y * image_scale - 1, 3, 3);
			}
			*/
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

