#ifndef PFH_H
#define PFH_H

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataNormals.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkIdList.h>
#include <vtkPCAStatistics.h>
#include <vtkTable.h>
#include <vtkLineSource.h>
#include <vtkProperty.h>
#include <vtkSelectEnclosedPoints.h>
#include <vtkPicker.h>
#include <vtkSphereSource.h>
#include <vtkMath.h>
#include <vtkExtractEdges.h>
#include <vtkLine.h>
#include <vtkCenterOfMass.h>

#include <cmath>

#include <queue>
#include <vector>

#define PI 3.1415926536

using namespace std;

struct Pair {
	int id;
	int step;
};

void pfh(vtkSmartPointer<vtkPolyData> &polydata, int i, int d, int r, vtkSmartPointer<vtkDoubleArray> &histogram, 
	vtkSmartPointer<vtkDoubleArray> &normalX, vtkSmartPointer<vtkDoubleArray> &normalY, vtkSmartPointer<vtkDoubleArray> &normalZ, double maxLen);

#endif
