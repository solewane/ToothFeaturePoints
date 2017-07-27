#ifndef VIEWER_H
#define VIEWER_H

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QCheckBox>
#include <QSignalMapper>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QScrollArea>
#include <QLabel>

#include <QtGui/QMainWindow>

#include <string>
#include <vector>
#include <io.h>

#include "VtkWidget.h"

#include "ui_viewer.h"

#include "utility.h"

using namespace std;

class viewer : public QMainWindow
{
	Q_OBJECT

public:
	viewer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~viewer();

private:
	vector<string> filenames;
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
	QVBoxLayout *radioLayout;
	QScrollArea *scrollArea;
	QWidget *scrollWidget;
	QVBoxLayout *scrollLayout;
	QButtonGroup *radioGroup;
	QSignalMapper *radioMapper;
	QCheckBox *featureCheckbox[5];
	QVBoxLayout *featureLayout;
	QSignalMapper *featureMapper;
	QPushButton *genFeatureBtn;
	QPushButton *showPointBtn;
	QLabel *outputLabel;
	QHBoxLayout *genLayout;
	bool isGened;

private:
	Ui::viewerClass ui;

public slots:
	void showAllTeeth();
	void hideAllTeeth();
	void showValidRange(int id);
	void generateFeature();

private:
	void getFiles(string path, vector<string>& files);

signals:
	void showCertainRange(string);
	void vtkGenFeature();
};

#endif // VIEWER_H
