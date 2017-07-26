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

#include "myAlgo.h"
#include "pfh.h"

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

	// 每点法向量
	vtkSmartPointer<vtkDoubleArray> normalX;
	vtkSmartPointer<vtkDoubleArray> normalY;
	vtkSmartPointer<vtkDoubleArray> normalZ;

	// 每点在pca坐标系的新坐标
	vtkSmartPointer<vtkDoubleArray> xCor;
	vtkSmartPointer<vtkDoubleArray> yCor;
	vtkSmartPointer<vtkDoubleArray> zCor;

	// pca坐标系极值
	double xCorMin, xCorMax;
	double yCorMin, yCorMax;
	double zCorMin, zCorMax;

	// 最长边长度
	double maxLen;

public:
	void readSTL(string folderPath, int id);
	void update();
	void getNormal();
	void getNewCor();
	void getPFH(int i, vtkSmartPointer<vtkDoubleArray> &histogram);
	void output();
	void outputToFilePFH(string inPath, string folderPath);
	void outputToFilePos(string folderPath);
	void outputToFileNewCor(string inPath, string outPath);
	bool getFeaturePoints(string m_id, string feature, vector<int> &exactPoints, vector<int> &nearPoints);
	int getRangePoints(string feature, vector<int> &rangePoints);
	void getFiles(string path, vector<string>& files);
};

#endif
