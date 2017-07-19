#include "MeshModel.h"

// 构造函数
MeshModel::MeshModel() {
	reader = vtkSmartPointer<vtkSTLReader>::New();
	polydata = vtkSmartPointer<vtkPolyData>::New();
	center = vtkSmartPointer<vtkDoubleArray>::New();

	xAxis = vtkSmartPointer<vtkDoubleArray>::New();
	yAxis = vtkSmartPointer<vtkDoubleArray>::New();
	zAxis = vtkSmartPointer<vtkDoubleArray>::New();

	normalX = vtkSmartPointer<vtkDoubleArray>::New();
	normalY = vtkSmartPointer<vtkDoubleArray>::New();
	normalZ = vtkSmartPointer<vtkDoubleArray>::New();

	xCor = vtkSmartPointer<vtkDoubleArray>::New();
	yCor = vtkSmartPointer<vtkDoubleArray>::New();
	zCor = vtkSmartPointer<vtkDoubleArray>::New();

	xCorMin = xCorMax = yCorMin = yCorMax = zCorMin = zCorMax = 0;
	maxLen = 0;
	isValid = false;
}

// 读取模型STL文件
void MeshModel::readSTL(string folderPath, int id) {
	this->id = id;
	stringstream stream;
	stream << folderPath << "\\" << (id / 7 + 1) << (id % 7 + 1) << ".stl";

	ifstream inFile(stream.str().c_str());
	if (!inFile) {
		return;
	}
	isValid = true;
	inFile.close();

	reader->SetFileName(stream.str().c_str());
	reader->Update();

	polydata->DeepCopy(reader->GetOutput());
}

// 更新网格模型各项数值
void MeshModel::update() {
	if (!isValid) {
		return;
	}

	// 获得多面体重心
	getCenterOfMass(polydata, center);

	// 获得各点处法向量
	getNormal();

	// 获得最长边长度
	maxLen = getMaxLen(reader, polydata);

	// 主成分分析得到坐标轴
	getXYZ(polydata, xAxis, yAxis, zAxis);
}

// 获得多面体各点处法向量
void MeshModel::getNormal() {
	vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalGenerator->SetInputData(polydata);
	normalGenerator->ComputePointNormalsOn();
	normalGenerator->ComputeCellNormalsOff();
	normalGenerator->Update();
	vtkSmartPointer<vtkPolyData> polydataWithNormal = normalGenerator->GetOutput();
	vtkDataArray* normalsGeneric = polydataWithNormal->GetPointData()->GetNormals();

	int numPoints = polydata->GetNumberOfPoints();
	normalX->Reset();
	normalY->Reset();
	normalZ->Reset();
	for (int i = 0; i < numPoints; ++i)	{
		double vtkGenericNormal[3];
		normalsGeneric->GetTuple(i, vtkGenericNormal);

		normalX->InsertNextValue(vtkGenericNormal[0]);
		normalY->InsertNextValue(vtkGenericNormal[1]);
		normalZ->InsertNextValue(vtkGenericNormal[2]);
	}
}

// 获得多面体各点在新坐标系下的坐标
void MeshModel::getNewCor() {
	if (!isValid) {
		return;
	}

	vtkSmartPointer<vtkDoubleArray> l1 = vtkSmartPointer<vtkDoubleArray>::New();
	vtkSmartPointer<vtkDoubleArray> l2 = vtkSmartPointer<vtkDoubleArray>::New();
	vtkSmartPointer<vtkDoubleArray> l3 = vtkSmartPointer<vtkDoubleArray>::New();

	getMatrix(xAxis, yAxis, zAxis, l1, l2, l3);

	for (int i = 0; i < polydata->GetNumberOfPoints(); ++i) {
		double p[3];
		polydata->GetPoint(i, p);

		double a = l1->GetValue(0) * p[0] + l1->GetValue(1) * p[1] + l1->GetValue(2) * p[2];
		double b = l2->GetValue(0) * p[0] + l2->GetValue(1) * p[1] + l2->GetValue(2) * p[2];
		double c = l3->GetValue(0) * p[0] + l3->GetValue(1) * p[1] + l3->GetValue(2) * p[2];

		xCor->InsertNextValue(a);
		yCor->InsertNextValue(b);
		zCor->InsertNextValue(c);

		if (a < xCorMin || xCorMin == 0) {
			xCorMin = a;
		}
		if (a > xCorMax || xCorMax == 0) {
			xCorMax = a;
		}

		if (b < yCorMin || yCorMin == 0) {
			yCorMin = b;
		}
		if (b > yCorMax || yCorMax == 0) {
			yCorMax = b;
		}

		if (c < zCorMin || zCorMin == 0) {
			zCorMin = c;
		}
		if (c > zCorMax || zCorMax == 0) {
			zCorMax = c;
		}
	}
}

// 获得PFH向量
void MeshModel::getPFH(int i, vtkSmartPointer<vtkDoubleArray> &histogram) {
	histogram->Reset();
	int rb = 3;	// 邻域半径
	int rd = 3; // 直方图维度
	pfh(polydata, i, rd, rb, histogram, normalX, normalY, normalZ, maxLen);
}

// 输出调试信息
void MeshModel::output() {
	cout << endl;
}

void MeshModel::outputToFilePFH(string folderPath) {
	if (!isValid) {
		return;
	}

	stringstream stream;
	stream << folderPath << "\\" << (id / 7 + 1) << (id % 7 + 1) << ".txt";
	ofstream outputFile(stream.str().c_str());
	for (int i = 0; i < polydata->GetNumberOfPoints(); ++i) {
		vtkSmartPointer<vtkDoubleArray> histogram = vtkSmartPointer<vtkDoubleArray>::New();
		getPFH(i, histogram);
		for (int j = 0; j < histogram->GetNumberOfTuples(); ++j) {
			outputFile << histogram->GetValue(j) << " ";
		}
		outputFile << endl;
	}
	outputFile.close();
}

void MeshModel::outputToFilePos(string folderPath) {
	if (!isValid) {
		return;
	}

	stringstream stream;
	stream << folderPath << "\\" << (id / 7 + 1) << (id % 7 + 1) << ".txt";
	ofstream outputFile(stream.str().c_str());
	for (int i = 0; i < polydata->GetNumberOfPoints(); ++i) {
		outputFile << xCor->GetValue(i) << " ";
		outputFile << yCor->GetValue(i) << " ";
		outputFile << zCor->GetValue(i) << " ";
		outputFile << endl;
	}
	outputFile.close();
}
