#ifndef VTKWIDGET_H
#define VTKWIDGET_H

#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkDoubleArray.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>

#include <QObject>
#include <QString>
#include <QVTKWidget.h>

#include <cmath>
#include <string>
#include <vector>
#include <io.h>

#include <Windows.h>

#include "pfh.h"
#include "utility.h"

using namespace std;

class VtkWidget : public QVTKWidget
{
	Q_OBJECT

public:
	VtkWidget(QWidget *parent);
	~VtkWidget();

private:
	bool isVisible[28];
	bool isValid[28];
	bool isShowFeature;
	bool showFeature[5];
	string showFeatureList[5];
	// 0 - distal 1 - mesial 2 - fossa 3 - incisal 4 - cusp
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkSTLReader> reader[28];
	vtkSmartPointer<vtkPolyData> polydata[28];
	vtkSmartPointer<vtkPolyDataMapper> mapper[28];
	vtkSmartPointer<vtkActor> actor[28];
	
	vtkSmartPointer<vtkDoubleArray> xAxis[28];
	vtkSmartPointer<vtkDoubleArray> yAxis[28];
	vtkSmartPointer<vtkDoubleArray> zAxis[28];

	vtkSmartPointer<vtkDoubleArray> xCor[28];
	vtkSmartPointer<vtkDoubleArray> yCor[28];
	vtkSmartPointer<vtkDoubleArray> zCor[28];

	double xCorMin[28], xCorMax[28];
	double yCorMin[28], yCorMax[28];
	double zCorMin[28], zCorMax[28];

	vtkSmartPointer<vtkDoubleArray> normalX[28];
	vtkSmartPointer<vtkDoubleArray> normalY[28];
	vtkSmartPointer<vtkDoubleArray> normalZ[28];
	double maxLen[28];

	vtkSmartPointer<vtkDoubleArray> massCenter[28];

private:
	void refreshVisible();
	void getNewCor(int id);
	void getFiles(string path, vector<string>& files);
	void getPFH(int i, int j, vtkSmartPointer<vtkDoubleArray> &histogram);
	void getNormal(int i);

public slots:
	void setToothVisible(int i);
	void showAllTeeth();
	void hideAllTeeth();
	void showRange(string filename);
	void genFeature();
	void showhideFeature();
	void modifyFeature(int id);
};

#endif
