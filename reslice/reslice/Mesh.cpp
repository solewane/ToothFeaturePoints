#include "Mesh.h"

// 构造函数
MeshModel::MeshModel() {
	reader = vtkSmartPointer<vtkSTLReader>::New();
	polydata = vtkSmartPointer<vtkPolyData>::New();
	center = vtkSmartPointer<vtkDoubleArray>::New();

	xAxis = vtkSmartPointer<vtkDoubleArray>::New();
	yAxis = vtkSmartPointer<vtkDoubleArray>::New();
	zAxis = vtkSmartPointer<vtkDoubleArray>::New();

	xCor = vtkSmartPointer<vtkDoubleArray>::New();
	yCor = vtkSmartPointer<vtkDoubleArray>::New();
	zCor = vtkSmartPointer<vtkDoubleArray>::New();

	xCorMin = xCorMax = yCorMin = yCorMax = zCorMin = zCorMax = 0;

	isValid = false;
}

// 读取模型STL文件
void MeshModel::readSTL(string folderPath, string matrixPath, int id) {
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

	// polydata->DeepCopy(reader->GetOutput());

	ifstream matrixFile(matrixPath.c_str());
	vtkSmartPointer<vtkMatrix4x4> m = vtkSmartPointer<vtkMatrix4x4>::New();
	m->Identity();
	if (!matrixFile) {
		cout << "Matrix file error!";
		return;
	}
	while (!matrixFile.eof()) {
		double tmp[16];
		string m_id;
		matrixFile >> m_id;
		if (matrixFile.eof()) {
			break;
		}
		for (int i = 0; i < 16; ++i) {
			matrixFile >> tmp[i];
		}
		if (m_id.at(0) == ('0' + id / 7 + 1) && m_id.at(1) == ('0' + id % 7 + 1)) {
			for (int i = 0; i < 16; ++i) {
				m->SetElement(i / 4, i % 4, tmp[i]);
			}
		}
	}
	matrixFile.close();
	vtkSmartPointer<vtkTransform> transform =
		vtkSmartPointer<vtkTransform>::New();
	transform->SetMatrix(m);
	transform->Update();

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
		vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetTransform(transform);
	transformFilter->SetInputData(reader->GetOutput());
	transformFilter->Update();

	polydata->DeepCopy(transformFilter->GetOutput());
}

// 更新网格模型各项数值
void MeshModel::update() {
	if (!isValid) {
		return;
	}

	// 获得多面体重心
	getCenterOfMass(polydata, center);

	// 主成分分析得到坐标轴
	getXYZ(polydata, xAxis, yAxis, zAxis);
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
		double a = l1->GetValue(0) * (p[0] - center->GetValue(0)) + l1->GetValue(1) * (p[1] - center->GetValue(1)) + l1->GetValue(2) * (p[2] - center->GetValue(2));
		double b = l2->GetValue(0) * (p[0] - center->GetValue(0)) + l2->GetValue(1) * (p[1] - center->GetValue(1)) + l2->GetValue(2) * (p[2] - center->GetValue(2));
		double c = l3->GetValue(0) * (p[0] - center->GetValue(0)) + l3->GetValue(1) * (p[1] - center->GetValue(1)) + l3->GetValue(2) * (p[2] - center->GetValue(2));

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

void MeshModel::outputFeature(string featurePath, string filePath) {
	if (!isValid) {
		return;
	}
	ifstream inFile(featurePath.c_str());
	string line;
	string featureName = "00_";
	featureName[0] = '0' + id / 7 + 1;
	featureName[1] = '0' + id % 7 + 1;
	double centerDistal[3] = { 0 };
	double centerMesial[3] = { 0 };
	double centerDistalPCA[3] = { 0 };
	double centerMesialPCA[3] = { 0 };
	int centerDistalXY[2] = { 0 };
	int centerMesialXY[2] = { 0 };
	while (!inFile.eof()) {
		inFile >> line;
		string a, b, c;
		if (inFile.eof()) {
			break;
		}
		inFile >> a >> b >> c;
		if (line == (featureName + "center_distal:")) {
			centerDistal[0] = stod(a.substr(0, a.find(",")).c_str());
			centerDistal[1] = stod(b.substr(0, b.find(",")).c_str());
			centerDistal[2] = stod(c.substr(0, c.find(",")).c_str());
		} else if (line == featureName + "center_mesial:") {
			centerMesial[0] = stod(a.substr(0, a.find(",")).c_str());
			centerMesial[1] = stod(b.substr(0, b.find(",")).c_str());
			centerMesial[2] = stod(c.substr(0, c.find(",")).c_str());
		}
	}

	vtkSmartPointer<vtkDoubleArray> l1 = vtkSmartPointer<vtkDoubleArray>::New();
	vtkSmartPointer<vtkDoubleArray> l2 = vtkSmartPointer<vtkDoubleArray>::New();
	vtkSmartPointer<vtkDoubleArray> l3 = vtkSmartPointer<vtkDoubleArray>::New();
	getMatrix(xAxis, yAxis, zAxis, l1, l2, l3);
	centerDistalPCA[0] = l1->GetValue(0) * (centerDistal[0] - center->GetValue(0)) + l1->GetValue(1) * (centerDistal[1] - center->GetValue(1)) + l1->GetValue(2) * (centerDistal[2] - center->GetValue(2));
	centerDistalPCA[1] = l2->GetValue(0) * (centerDistal[0] - center->GetValue(0)) + l2->GetValue(1) * (centerDistal[1] - center->GetValue(1)) + l2->GetValue(2) * (centerDistal[2] - center->GetValue(2));
	centerDistalPCA[2] = l3->GetValue(0) * (centerDistal[0] - center->GetValue(0)) + l3->GetValue(1) * (centerDistal[1] - center->GetValue(1)) + l3->GetValue(2) * (centerDistal[2] - center->GetValue(2));

	centerMesialPCA[0] = l1->GetValue(0) * (centerMesial[0] - center->GetValue(0)) + l1->GetValue(1) * (centerMesial[1] - center->GetValue(1)) + l1->GetValue(2) * (centerMesial[2] - center->GetValue(2));
	centerMesialPCA[1] = l2->GetValue(0) * (centerMesial[0] - center->GetValue(0)) + l2->GetValue(1) * (centerMesial[1] - center->GetValue(1)) + l2->GetValue(2) * (centerMesial[2] - center->GetValue(2));
	centerMesialPCA[2] = l3->GetValue(0) * (centerMesial[0] - center->GetValue(0)) + l3->GetValue(1) * (centerMesial[1] - center->GetValue(1)) + l3->GetValue(2) * (centerMesial[2] - center->GetValue(2));

	int xSize = 64;
	int ySize = 64;

	int xRealSize = 0, yRealSize = 0;
	if ((yCorMax - yCorMin) > (zCorMax - zCorMin)) {
		xRealSize = xSize * 0.75;
		yRealSize = xSize * (zCorMax - zCorMin) / (yCorMax - yCorMin) * 0.75;
	} else {
		xRealSize = ySize * (yCorMax - yCorMin) / (zCorMax - zCorMin) * 0.75;
		yRealSize = ySize * 0.75;
	}
	
	centerDistalXY[0] = (centerDistalPCA[1] - yCorMin) / (yCorMax - yCorMin) * xRealSize + (xSize - xRealSize) / 2;
	centerDistalXY[1] = (centerDistalPCA[2] - zCorMin) / (zCorMax - zCorMin) * yRealSize + (ySize - yRealSize) / 2;
	centerMesialXY[0] = (centerMesialPCA[1] - yCorMin) / (yCorMax - yCorMin) * xRealSize + (xSize - xRealSize) / 2;
	centerMesialXY[1] = (centerMesialPCA[2] - zCorMin) / (zCorMax - zCorMin) * yRealSize + (ySize - yRealSize) / 2;

	ofstream outFile(filePath);
	outFile << centerDistalXY[0] << " " << 64 - centerDistalXY[1] << "\n" << centerMesialXY[0] << " " << 64 - centerMesialXY[1] << "\n";
	outFile.close();
}

void MeshModel::outputToImage(string filePath) {
	if (!isValid) {
		return;
	}
	vector<vector<double> > colorTable;
	int xSize = 64;
	int ySize = 64;

	int xRealSize = 0, yRealSize = 0;
	if ((yCorMax - yCorMin) > (zCorMax - zCorMin)) {
		xRealSize = xSize * 0.75;
		yRealSize = xSize * (zCorMax - zCorMin) / (yCorMax - yCorMin) * 0.75;
	} else {
		xRealSize = ySize * (yCorMax - yCorMin) / (zCorMax - zCorMin) * 0.75;
		yRealSize = ySize * 0.75;
	}

	vector<vector<double> > height;
	for (int i = 0; i < xSize; ++i) {
		vector<double> currLine(ySize, 0);
		height.push_back(currLine);
	}
	double hMin = 0, hMax = 0;
	for (int i = 0; i < polydata->GetNumberOfPoints(); ++i) {
		int xPos = (yCor->GetValue(i) - yCorMin) / (yCorMax - yCorMin) * xRealSize + (xSize - xRealSize) / 2;
		int yPos = (zCor->GetValue(i) - zCorMin) / (zCorMax - zCorMin) * yRealSize + (ySize - yRealSize) / 2;
		for (int x = xPos; x <= xPos + 1; ++x) {
			for (int y = yPos; y <= yPos + 1; ++y) {
				int xCurrPos = x >= xSize ? (xSize - 1) : x;
				int yCurrPos = y >= ySize ? (ySize - 1) : y;
				if (xCor->GetValue(i) <= 0) {
					continue;
				}
				height[xCurrPos][yCurrPos] = xCor->GetValue(i) > height[xCurrPos][yCurrPos] ? xCor->GetValue(i) : height[xCurrPos][yCurrPos];
			}
		}
	}
	for (int i = 0; i < xSize; ++i) {
		for (int j = 0; j < ySize; ++j) {
			if (hMax == 0 || height[i][j] > hMax) {
				hMax = height[i][j];
			}
		}
	}
	hMin = hMax;
	while (true) {
		int num = 0;
		for (int i = 0; i < xSize; ++i) {
			for (int j = 0; j < ySize; ++j) {
				if (height[i][j] < 0 || height[i][j] > hMin) {
					num++;
				}
			}
		}
		if ((xSize * ySize - num) * 20 < (xSize * ySize)) {
			break;
		}
		hMin -= 0.05;
	}

	for (int i = 0; i < xSize; ++i) {
		for (int j = 0; j < ySize; ++j) {
			if (height[i][j] >= hMin) {
				int threshold = 0;
				int tmp = (height[i][j] - hMin) * (255 - threshold) / (hMax - hMin);
				height[i][j] = threshold + (tmp > (255 - threshold) ? (255 - threshold) : tmp);
			} else if (height[i][j] > 0) {
				height[i][j] = 1.0;
			}
		}
	}

	for (int i = 0; i < xSize; ++i) {
		for (int j = 0; j < ySize; ++j) {
			if (height[i][j] == 0) {
				int xMin = i > 0 ? (i - 1) : 0;
				int xMax = i < (xSize - 1) ? (i + 1) : (xSize - 1);
				int yMin = j > 0 ? (j - 1) : 0;
				int yMax = j < (ySize - 1) ? (j + 1) : (ySize - 1);
				vector<int> neighbors;
				int currNum = 0;
				int totalNum = (xMax - xMin + 1) * (yMax - yMin + 1);
				for (int k = xMin; k <= xMax; ++k) {
					for (int l = yMin; l <= yMax; ++l) {
						if (height[k][l] > 0) {
							++currNum;
							neighbors.push_back(height[k][l]);
						}
					}
				}
				if (currNum > totalNum * 0.6 && neighbors.size()) {
					height[i][j] = neighbors[neighbors.size() / 2];
				}
			}
		}
	}

	BmpWriter w(xSize, ySize);

	for (int i = 0; i < xSize; ++i) {
		for (int j = 0; j < ySize; ++j) {
			RGB rgb;
			rgb.r = rgb.g = rgb.b = height[i][j];
			w.setPixel(i, j, rgb);
		}
	}
	w.SaveImage((char*)filePath.c_str());
}
