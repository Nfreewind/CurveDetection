#pragma once

#include <QDialog>
#include "ui_OptionDialog.h"

class OptionDialog : public QDialog {
	Q_OBJECT

private:
	Ui::OptionDialog ui;

public:
	OptionDialog(QWidget *parent = Q_NULLPTR);
	~OptionDialog();
	
	int getNumIterations();
	int getMinPoints();
	float getMaxErrorRatioToRadius();
	float getClusterEpsilon();
	float getMinAngle();
	float getMinRadius();
	float getMaxRadius();

public slots:
	void onOK();
	void onCancel();
};
