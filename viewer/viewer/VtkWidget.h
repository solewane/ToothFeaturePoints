#ifndef VTKWIDGET_H
#define VTKWIDGET_H

#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkParametricEllipsoid.h>

#include <QObject>
#include <QString>
#include <QVTKWidget.h>

class VtkWidget : public QVTKWidget
{
	Q_OBJECT

public:
	VtkWidget(QWidget *parent);
	~VtkWidget();

private:
	bool isVisible[28];
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkSTLReader> reader[28];
	vtkSmartPointer<vtkPolyData> polydata[28];
	vtkSmartPointer<vtkPolyDataMapper> mapper[28];
	vtkSmartPointer<vtkActor> actor[28];
	
	// Range
	double range[28][3];

private:
	void refreshVisible();
	void showRange();

public slots:
	void setToothVisible(int i);
	void showAllTeeth();
	void hideAllTeeth();
};

#endif
