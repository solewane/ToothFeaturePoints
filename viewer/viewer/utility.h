#ifndef UTILITY_H
#define UTILITY_H

#define POS_FEATURE_PATH "E:\\data\\pos_feature"
#define MODEL_PATH "E:\\train\\"
//#define MODEL_PATH "E:\\data\\stl\\16\\teeth_stl\\"
#define TMP_PATH "E:\\tmp\\"

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>
#include <vtkPCAStatistics.h>
#include <vtkTable.h>
#include <vtkMath.h>
#include <vtkCenterOfMass.h>
#include <vtkSTLReader.h>
#include <vtkExtractEdges.h>
#include <vtkLine.h>

void getXYZ(vtkSmartPointer<vtkPolyData> &polydata, vtkDoubleArray *x, vtkDoubleArray *y, vtkDoubleArray *z);
void autoCheckPCA(vtkSmartPointer<vtkPolyData> polydata[28], vtkSmartPointer<vtkDoubleArray> center[28], bool isValid[28], 
	vtkSmartPointer<vtkDoubleArray> xAxis[28], vtkSmartPointer<vtkDoubleArray> yAxis[28], vtkSmartPointer<vtkDoubleArray> zAxis[28]);
void getCenterOfMass(vtkSmartPointer<vtkPolyData> &polydata, vtkDoubleArray *center);
void getMatrix(vtkSmartPointer<vtkDoubleArray> &x, vtkSmartPointer<vtkDoubleArray> &y, vtkSmartPointer<vtkDoubleArray> &z, 
	vtkSmartPointer<vtkDoubleArray> &l1, vtkSmartPointer<vtkDoubleArray> &l2, vtkSmartPointer<vtkDoubleArray> &l3);
double getMaxLen(vtkSmartPointer<vtkSTLReader> &reader, vtkSmartPointer<vtkPolyData> &polydata);

#endif
