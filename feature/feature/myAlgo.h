#ifndef MYALGO_H
#define MYALGO_H

#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
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

#include <iostream>

#include "MeshModel.h"

#define MATRIX_PATH "E:\\data\\matrices\\"
#define FEATURE_PATH "E:\\data\\feature_points\\"
#define RANGE_PATH "E:\\data\\pos_feature\\"

class MeshModel;

void getCenterOfMass(vtkSmartPointer<vtkPolyData> &polydata, vtkDoubleArray *center);
void getXYZ(vtkSmartPointer<vtkPolyData> &polydata, vtkDoubleArray *x, vtkDoubleArray *y, vtkDoubleArray *z);
void autoCheckPCA(MeshModel m[28]);
void getMatrix(vtkSmartPointer<vtkDoubleArray> &x, vtkSmartPointer<vtkDoubleArray> &y, vtkSmartPointer<vtkDoubleArray> &z, 
	vtkSmartPointer<vtkDoubleArray> &l1, vtkSmartPointer<vtkDoubleArray> &l2, vtkSmartPointer<vtkDoubleArray> &l3);
double getMaxLen(vtkSmartPointer<vtkSTLReader> &reader, vtkSmartPointer<vtkPolyData> &polydata);

#endif
