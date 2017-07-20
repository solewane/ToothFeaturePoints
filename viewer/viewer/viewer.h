#ifndef VIEWER_H
#define VIEWER_H

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QCheckBox>
#include <QSignalMapper>
#include <QPushButton>

#include <QtGui/QMainWindow>

#include "VtkWidget.h"

#include "ui_viewer.h"

class viewer : public QMainWindow
{
	Q_OBJECT

public:
	viewer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~viewer();

private:
	QWidget *mainWidget;
	QHBoxLayout *mainLayout;
	VtkWidget *vtkWidget;
	QVBoxLayout *rightLayout;
	QGridLayout *groupLayout;
	QCheckBox *checkbox[28];
	QHBoxLayout *showHideLayout;
	QPushButton *showAllBtn;
	QPushButton *hideAllBtn;
	QSignalMapper *mapper;

private:
	Ui::viewerClass ui;

public slots:
	void showAllTeeth();
	void hideAllTeeth();
};

#endif // VIEWER_H
