#include "MainWindow.h"
#include <QFileDialog>
#include "OptionDialog.h"
#include "LineOptionDialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	setCentralWidget(&canvas);

	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionDetectContours, SIGNAL(triggered()), this, SLOT(onDetectContours()));
	connect(ui.actionDetectCurves, SIGNAL(triggered()), this, SLOT(onDetectCurves()));
	connect(ui.actionDetectLines, SIGNAL(triggered()), this, SLOT(onDetectLines()));
}

void MainWindow::onOpen() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load voxel data..."), "", tr("Image files (*.png *.jpg *.bmp)"));
	if (filename.isEmpty()) return;

	setWindowTitle("Line/Curve Detection - " + filename);
	
	canvas.loadImage(filename);
	canvas.update();
}

void MainWindow::onDetectContours() {
	canvas.detectContours();
	canvas.update();
}

void MainWindow::onDetectCurves() {
	OptionDialog dlg;
	if (dlg.exec()) {
		canvas.detectCurves(dlg.getNumIterations(), dlg.getMinPoints(), dlg.getMaxErrorRatioToRadius(), dlg.getClusterEpsilon(), dlg.getMinAngle() / 180.0 * CV_PI, dlg.getMinRadius(), dlg.getMaxRadius());
		canvas.update();
	}
}

void MainWindow::onDetectLines() {
	LineOptionDialog dlg;
	if (dlg.exec()) {
		canvas.detectLines(dlg.getNumIterations(), dlg.getMinPoints(), dlg.getMaxError(), dlg.getClusterEpsilon(), dlg.getMinLength());
		canvas.update();
	}
}