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

	string matrix_path = MATRIX_PATH;
	matrix_path.append("matrix_");
	int start_pos = folderPath.find("stl\\");
	int end_pos = folderPath.find("\\teeth");
	matrix_path.append(folderPath.substr(start_pos + 4, end_pos - start_pos - 4));
	matrix_path.append(".txt");
	ifstream matrixFile(matrix_path.c_str());
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

void MeshModel::outputToFilePFH(string inPath, string folderPath) {
	if (!isValid) {
		return;
	}
	/*
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
	outputFile.close();*/
	
	string m_id = inPath.substr(inPath.find("stl\\") + 4, inPath.find("\\teeth") - inPath.find("stl\\") - 4);
	// Get Features
	vector<string> files;
	getFiles(RANGE_PATH, files);
	for (int i = 0; i < files.size(); ++i) {
		string featureName = files[i];
		// Get Feature Points
		vector<int> exactPoints;
		vector<int> nearPoints;
		bool isFound = getFeaturePoints(m_id, featureName, exactPoints, nearPoints);
		if (!isFound) {
			continue;
		}
		// Get Points in Range
		vector<int> rangePoints;
		int rangeNum = getRangePoints(featureName, rangePoints);
		// Output PFH
		string outPath = folderPath;
		outPath.append("\\");
		outPath.append(featureName);
		outPath.append(".txt");
		ofstream outFile(outPath, ios::app);
		for (int j = 0; j < rangeNum; ++j) {
			// Calculate Weight
			double w = 0;
			if (find(nearPoints.begin(), nearPoints.end(), rangePoints[j]) != nearPoints.end()) {
				w = 0.5;
			}
			if (exactPoints[0] == rangePoints[j]) {
				w = 1;
			}
			// Get PFH
			vtkSmartPointer<vtkDoubleArray> histogram = vtkSmartPointer<vtkDoubleArray>::New();
			getPFH(rangePoints[j], histogram);
			// Output
			outFile << w << " ";
			for (int k = 0; k < histogram->GetNumberOfTuples(); ++k) {
				outFile << histogram->GetValue(k) << " ";
			}
			outFile << endl;
		}
		outFile.close();
	}
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

bool MeshModel::getFeaturePoints(string m_id, string feature, vector<int> &exactPoints, vector<int> &nearPoints) {
	string featurePath = FEATURE_PATH;
	featurePath.append(m_id);
	featurePath.append(".txt");
	ifstream inFile(featurePath);
	char tmp[200];
	double p[3];
	bool isFound = false;
	while (inFile.getline(tmp, 200)) {
		if (string(tmp).find(feature) == 0) {
			string line(tmp);
			istringstream iss(line.substr(line.find(": ") + 2));
			for (int i = 0; i < 2; ++i) {
				iss >> p[i];
				string comma;
				iss >> comma;
			}
			iss >> p[2];
			isFound = true;
			break;
		}
	}
	inFile.close();
	if (!isFound) {
		return false;
	}
	double minDis = -1;
	int currPoint = -1;
	// Exact Points
	for (int i = 0; i < polydata->GetNumberOfPoints(); ++i) {
		double q[3];
		polydata->GetPoint(i, q);
		if (minDis < 0 || vtkMath::Distance2BetweenPoints(p, q) < minDis) {
			minDis = vtkMath::Distance2BetweenPoints(p, q);
			currPoint = i;
		}
	}
	exactPoints.push_back(currPoint);
	// Near Points
	vtkSmartPointer<vtkIdList> neighborCells = vtkSmartPointer<vtkIdList>::New();
	polydata->GetPointCells(currPoint, neighborCells);
	int numCells = neighborCells->GetNumberOfIds();
	for (int j = 0; j < numCells; ++j)	{
		vtkSmartPointer<vtkIdList> pointIdList = vtkSmartPointer<vtkIdList>::New();
		polydata->GetCellPoints(neighborCells->GetId(j), pointIdList);
		if (pointIdList->GetId(0) != currPoint) {
			if (find(nearPoints.begin(), nearPoints.end(), pointIdList->GetId(0)) == nearPoints.end()) {
				nearPoints.push_back(pointIdList->GetId(0));
			}
		} else {
			if (find(nearPoints.begin(), nearPoints.end(), pointIdList->GetId(1)) == nearPoints.end()) {
				nearPoints.push_back(pointIdList->GetId(1));
			}
		}
	}
	return true;
}

int MeshModel::getRangePoints(string feature, vector<int> &rangePoints) {
	string filePath = RANGE_PATH;
	filePath.append(feature);
	filePath.append(".txt");
	ifstream inFile(filePath);
	double mu[3];
	double r[3];
	for (int i = 0; i < 3; ++i) {
		inFile >> mu[i];
		inFile >> r[i];
	}
	for (int i = 0; i < polydata->GetNumberOfPoints(); ++i) {
		double p[3];
		p[0] = (xCor->GetValue(i) - xCorMin) / (xCorMax - xCorMin);
		p[1] = (yCor->GetValue(i) - yCorMin) / (yCorMax - yCorMin);
		p[2] = (zCor->GetValue(i) - zCorMin) / (zCorMax - zCorMin);
		double rate = 1.0;
		if ((p[0] > mu[0] - r[0] * rate) && (p[0] < mu[0] + r[0] * rate) && (p[1] > mu[1] - r[1] * rate) && (p[1] < mu[1] + r[1] * rate) && (p[2] > mu[2] - r[2] * rate) && (p[2] < mu[2] + r[2] * rate)) {
			rangePoints.push_back(i);
		}
	}
	inFile.close();
	return rangePoints.size();
}

void MeshModel::getFiles(string path, vector<string>& files) {
	long hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) != -1) {
		do {
			if((fileinfo.attrib &  _A_SUBDIR)) {
				if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			} else {
				string tmp = fileinfo.name;
				if ((tmp.at(0) - '1') * 7 + (tmp.at(1) - '1') == id) {
					files.push_back(tmp.substr(0, tmp.find(".txt")));
				}
			}
		} while(_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

void MeshModel::outputToFileNewCor(string inPath, string outPath) {
	if (!isValid) {
		return;
	}

	ofstream outputFile(outPath);

	ifstream inFile(inPath);
	double a, b, c, a1, b1, c1;
	char d;

	vtkSmartPointer<vtkDoubleArray> l1 = vtkSmartPointer<vtkDoubleArray>::New();
	vtkSmartPointer<vtkDoubleArray> l2 = vtkSmartPointer<vtkDoubleArray>::New();
	vtkSmartPointer<vtkDoubleArray> l3 = vtkSmartPointer<vtkDoubleArray>::New();

	getMatrix(xAxis, yAxis, zAxis, l1, l2, l3);

	while (!inFile.eof()) {
		inFile >> a >> b >> c >> d;
		// cout << a << " " << b << " " << c << " " << endl;

		a1 = l1->GetValue(0) * (a - center->GetValue(0)) + l1->GetValue(1) * (b - center->GetValue(1)) + l1->GetValue(2) * (c - center->GetValue(2));
		b1 = l2->GetValue(0) * (a - center->GetValue(0)) + l2->GetValue(1) * (b - center->GetValue(1)) + l2->GetValue(2) * (c - center->GetValue(2));
		c1 = l3->GetValue(0) * (a - center->GetValue(0)) + l3->GetValue(1) * (b - center->GetValue(1)) + l3->GetValue(2) * (c - center->GetValue(2));

		// cout << xCorMin << " " << xCorMax << endl << yCorMin << " " << yCorMax << endl << zCorMin << " " << zCorMax << endl;
		// cout << a1 << "\t" << b1 << "\t" << c1 << endl;

		outputFile << (a1 - xCorMin) / (xCorMax - xCorMin) << " " << (b1 - yCorMin) / (yCorMax - yCorMin) << " " << (c1 - zCorMin) / (zCorMax - zCorMin) << endl;
		break;
	}

	inFile.close();
	outputFile.close();
}
