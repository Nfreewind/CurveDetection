#include "OptionDialog.h"

OptionDialog::OptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.lineEditNumIterations->setText("200000");
	ui.lineEditMinPoints->setText("200");
	ui.lineEditMaxErrorRatioToRadius->setText("0.02");
	ui.lineEditClusterEpsilon->setText("30");
	ui.lineEditMinAngle->setText("90");
	ui.lineEditMinRadius->setText("80");
	ui.lineEditMaxRadius->setText("400");

	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

OptionDialog::~OptionDialog() {
}

int OptionDialog::getNumIterations() {
	return ui.lineEditNumIterations->text().toInt();
}

int OptionDialog::getMinPoints() {
	return ui.lineEditMinPoints->text().toInt();
}

float OptionDialog::getMaxErrorRatioToRadius() {
	return ui.lineEditMaxErrorRatioToRadius->text().toFloat();
}

float OptionDialog::getClusterEpsilon() {
	return ui.lineEditClusterEpsilon->text().toFloat();
}

float OptionDialog::getMinAngle() {
	return ui.lineEditMinAngle->text().toFloat();
}

float OptionDialog::getMinRadius() {
	return ui.lineEditMinRadius->text().toFloat();
}

float OptionDialog::getMaxRadius() {
	return ui.lineEditMaxRadius->text().toFloat();
}

void OptionDialog::onOK() {
	accept();
}

void OptionDialog::onCancel() {
	reject();
}
