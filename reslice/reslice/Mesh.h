#ifndef MESHMODEL_H
#define MESHMODEL_H

#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <io.h>
#include <vector>
#include <algorithm>

#include "Utility.h"
#include "BmpWriter.h"

using namespace std;

class MeshModel {
public:
	MeshModel();
	~MeshModel() {}

public:
	int id;
	bool isValid;

	// 多面体基本信息
	vtkSmartPointer<vtkSTLReader> reader;		// STL reader
	vtkSmartPointer<vtkPolyData> polydata;		// 多边形数据
	vtkSmartPointer<vtkDoubleArray> center;		// 重心

	// pca主成分分析坐标
	vtkSmartPointer<vtkDoubleArray> xAxis;
	vtkSmartPointer<vtkDoubleArray> yAxis;
	vtkSmartPointer<vtkDoubleArray> zAxis;

	// 每点在pca坐标系的新坐标
	vtkSmartPointer<vtkDoubleArray> xCor;
	vtkSmartPointer<vtkDoubleArray> yCor;
	vtkSmartPointer<vtkDoubleArray> zCor;

	// pca坐标系极值
	double xCorMin, xCorMax;
	double yCorMin, yCorMax;
	double zCorMin, zCorMax;

public:
	void readSTL(string folderPath, string matrixPath, int id);
	void update();
	void outputToImage(string filePath);
	void getNewCor();
	void outputFeature(string featurePath, string filePath);
};

#endif
