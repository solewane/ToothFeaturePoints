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

#include <string>

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

	vtkSmartPointer<vtkDoubleArray> massCenter[28];

private:
	void refreshVisible();
	void getNewCor(int id);

public slots:
	void setToothVisible(int i);
	void showAllTeeth();
	void hideAllTeeth();
	void showRange(string filename);
};

#endif
