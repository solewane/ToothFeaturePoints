#include "myAlgo.h"

// 获得多面体重心
void getCenterOfMass(vtkSmartPointer<vtkPolyData> &polydata, vtkDoubleArray *center) {
	vtkSmartPointer<vtkCenterOfMass> centerOfMassFilter = vtkSmartPointer<vtkCenterOfMass>::New();
	centerOfMassFilter->SetInputData(polydata);
	centerOfMassFilter->SetUseScalarsAsWeights(false);
	centerOfMassFilter->Update();
	double massCenter[3];
	centerOfMassFilter->GetCenter(massCenter);
	center->Reset();
	center->InsertNextValue(massCenter[0]);
	center->InsertNextValue(massCenter[1]);
	center->InsertNextValue(massCenter[2]);
}

// 主成分分析得到坐标轴
void getXYZ(vtkSmartPointer<vtkPolyData> &polydata, vtkDoubleArray *x, vtkDoubleArray *y, vtkDoubleArray *z) {
	// 获得数据集
	vtkSmartPointer<vtkDoubleArray> datasetArrX = vtkSmartPointer<vtkDoubleArray>::New();
	datasetArrX->SetNumberOfComponents(1);
	datasetArrX->SetName("X");
	vtkSmartPointer<vtkDoubleArray> datasetArrY = vtkSmartPointer<vtkDoubleArray>::New();
	datasetArrY->SetNumberOfComponents(1);
	datasetArrY->SetName("Y");
	vtkSmartPointer<vtkDoubleArray> datasetArrZ = vtkSmartPointer<vtkDoubleArray>::New();
	datasetArrZ->SetNumberOfComponents(1);
	datasetArrZ->SetName("Z");
	for (int j = 0; j < polydata->GetNumberOfPoints(); ++j) {
		double p[3];
		polydata->GetPoint(j, p);
		datasetArrX->InsertNextValue(p[0]);
		datasetArrY->InsertNextValue(p[1]);
		datasetArrZ->InsertNextValue(p[2]);
	}
	vtkSmartPointer<vtkTable> datasetTable = vtkSmartPointer<vtkTable>::New();
	datasetTable->AddColumn(datasetArrX);
	datasetTable->AddColumn(datasetArrY);
	datasetTable->AddColumn(datasetArrZ);

	// PCA主成分分析
	vtkSmartPointer<vtkPCAStatistics> pca = vtkSmartPointer<vtkPCAStatistics>::New();
	pca->SetInputData(datasetTable);
	pca->SetColumnStatus("X", 1 );
	pca->SetColumnStatus("Y", 1 );
	pca->SetColumnStatus("Z", 1 );
	pca->RequestSelectedColumns();
	pca->SetDeriveOption(true);
	pca->Update();
	vtkSmartPointer<vtkDoubleArray> eigenvalues = vtkSmartPointer<vtkDoubleArray>::New();
	pca->GetEigenvector(0, x);
	pca->GetEigenvector(1, y);
	pca->GetEigenvector(2, z);
}

void getXYZ(MeshModel m[28], vtkDoubleArray *x, vtkDoubleArray *y, vtkDoubleArray *z) {
	// 获得数据集
	vtkSmartPointer<vtkDoubleArray> datasetArrX = vtkSmartPointer<vtkDoubleArray>::New();
	datasetArrX->SetNumberOfComponents(1);
	datasetArrX->SetName("X");
	vtkSmartPointer<vtkDoubleArray> datasetArrY = vtkSmartPointer<vtkDoubleArray>::New();
	datasetArrY->SetNumberOfComponents(1);
	datasetArrY->SetName("Y");
	vtkSmartPointer<vtkDoubleArray> datasetArrZ = vtkSmartPointer<vtkDoubleArray>::New();
	datasetArrZ->SetNumberOfComponents(1);
	datasetArrZ->SetName("Z");
	for (int i = 0; i < 28; ++i) {
		if (!m[i].isValid) {
			continue;
		}
		for (int j = 0; j < m[i].polydata->GetNumberOfPoints(); ++j) {
			double p[3];
			m[i].polydata->GetPoint(j, p);
			datasetArrX->InsertNextValue(p[0]);
			datasetArrY->InsertNextValue(p[1]);
			datasetArrZ->InsertNextValue(p[2]);
		}
	}
	vtkSmartPointer<vtkTable> datasetTable = vtkSmartPointer<vtkTable>::New();
	datasetTable->AddColumn(datasetArrX);
	datasetTable->AddColumn(datasetArrY);
	datasetTable->AddColumn(datasetArrZ);

	// PCA主成分分析
	vtkSmartPointer<vtkPCAStatistics> pca = vtkSmartPointer<vtkPCAStatistics>::New();
	pca->SetInputData(datasetTable);
	pca->SetColumnStatus("X", 1 );
	pca->SetColumnStatus("Y", 1 );
	pca->SetColumnStatus("Z", 1 );
	pca->RequestSelectedColumns();
	pca->SetDeriveOption(true);
	pca->Update();
	vtkSmartPointer<vtkDoubleArray> eigenvalues = vtkSmartPointer<vtkDoubleArray>::New();
	pca->GetEigenvector(0, x);
	pca->GetEigenvector(1, y);
	pca->GetEigenvector(2, z);
}

// 校准pca坐标轴方向
void autoCheckPCA(MeshModel m[28], vtkSmartPointer<vtkDoubleArray> &mainX, vtkSmartPointer<vtkDoubleArray> &mainY, vtkSmartPointer<vtkDoubleArray> &mainZ) {
	// 上下颌 x轴均指向牙缝
	double xArr[3] = { mainX->GetValue(0), mainX->GetValue(1), mainX->GetValue(2) };
	double yArr[3] = { mainY->GetValue(0), mainY->GetValue(1), mainY->GetValue(2) };
	double zArr[3] = { mainZ->GetValue(0), mainZ->GetValue(1), mainZ->GetValue(2) };
	for (int i = 0; i < 28; ++i) {
		if (!m[i].isValid) {
			continue;
		}
		int quadrant = i / 7 + 1;
		int id = i % 7 + 1;
		double tmpX[3] = { m[i].xAxis->GetValue(0), m[i].xAxis->GetValue(1), m[i].xAxis->GetValue(2) };
		double product = vtkMath::Dot(tmpX, zArr);
		if ((quadrant <= 2 && product > 0) || (quadrant > 2 && product < 0)) {
			for (int j = 0; j < 3; ++j)	{
				m[i].xAxis->SetValue(j, -m[i].xAxis->GetValue(j));
			}
		}
	}

	// 得到y轴和z轴
	for (int i = 0; i < 28; ++i) {
		if (!m[i].isValid) {
			continue;
		}
		int quadrant = i / 7 + 1;
		int id = i % 7 + 1;
		int leftId = i - 1;
		int rightId = i + 1;
		if (id == 1) {
			if (quadrant <= 2) {
				leftId = (2 - quadrant) * 7 + 1;
			} else {
				leftId = (6 - quadrant) * 7 + 1;
			}
		}
		if (id == 7) {
			rightId = i;
		}
		if (!m[leftId].isValid)	{
			leftId = i;
		}
		if (!m[rightId].isValid)	{
			rightId = i;
		}

		double standard[3] = { m[rightId].center->GetValue(0) - m[leftId].center->GetValue(0), 
			m[rightId].center->GetValue(1) - m[leftId].center->GetValue(1), 
			m[rightId].center->GetValue(2) - m[leftId].center->GetValue(2) };
		double tmpY[3] = { m[i].yAxis->GetValue(0), m[i].yAxis->GetValue(1), m[i].yAxis->GetValue(2) };
		double tmpZ[3] = { m[i].zAxis->GetValue(0), m[i].zAxis->GetValue(1), m[i].zAxis->GetValue(2) };
		double product1 = vtkMath::Dot(tmpY, standard) / (vtkMath::Norm(standard) * vtkMath::Norm(tmpY));
		double product2 = vtkMath::Dot(tmpZ, standard) / (vtkMath::Norm(standard) * vtkMath::Norm(tmpZ));
		if (abs(product1) > abs(product2)) {
			for (int j = 0; j < 3; ++j)	{
				double tmp = m[i].yAxis->GetValue(j);
				m[i].yAxis->SetValue(j, m[i].zAxis->GetValue(j));
				m[i].zAxis->SetValue(j, tmp);
			}
		}
	}

	// y轴应指向前方
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 7; ++j) {
			int id = i * 7 + j;
			if (!m[id].isValid)	{
				continue;
			}
			double tmpY[3] = { m[id].yAxis->GetValue(0), m[id].yAxis->GetValue(1), m[id].yAxis->GetValue(2) };
			if (j == 0) {
				double product = vtkMath::Dot(tmpY, yArr);
				if (product < 0) {
					for (int k = 0; k < 3; ++k)	{
						m[id].yAxis->SetValue(k, -m[id].yAxis->GetValue(k));
					}
				}
			} else {
				int lastYid = id - 1;
				if (!m[id - 1].isValid)	{
					lastYid = id - 2;
				}
				double lastY[3] = { m[lastYid].yAxis->GetValue(0), m[lastYid].yAxis->GetValue(1), m[lastYid].yAxis->GetValue(2) };
				double product = vtkMath::Dot(tmpY, lastY);
				if (product < 0) {
					for (int k = 0; k < 3; ++k)	{
						m[id].yAxis->SetValue(k, -m[id].yAxis->GetValue(k));
					}
				}
			}
		}
	}

	// 直接计算x轴和y轴叉积得到z轴
	for (int i = 0; i < 28; ++i) {
		if (!m[i].isValid) {
			continue;
		}
		double tmpX[3] = { m[i].xAxis->GetValue(0), m[i].xAxis->GetValue(1), m[i].xAxis->GetValue(2) };
		double tmpY[3] = { m[i].yAxis->GetValue(0), m[i].yAxis->GetValue(1), m[i].yAxis->GetValue(2) };
		double tmp[3];
		vtkMath::Cross(tmpX, tmpY, tmp);
		for (int j = 0; j < 3; ++j)	{
			m[i].zAxis->SetValue(j, tmp[j]);
		}
	}
}

// 计算逆矩阵
void getMatrix(vtkSmartPointer<vtkDoubleArray> &x, vtkSmartPointer<vtkDoubleArray> &y, vtkSmartPointer<vtkDoubleArray> &z, 
	vtkSmartPointer<vtkDoubleArray> &l1, vtkSmartPointer<vtkDoubleArray> &l2, vtkSmartPointer<vtkDoubleArray> &l3) {
	l1->Reset();
	l2->Reset();
	l3->Reset();

	double r = x->GetValue(0) * (y->GetValue(1) * z->GetValue(2) - y->GetValue(2) * z->GetValue(1)) + 
		x->GetValue(1) * (y->GetValue(2) * z->GetValue(0) - y->GetValue(0) * z->GetValue(2)) + 
		x->GetValue(2) * (y->GetValue(0) * z->GetValue(1) - y->GetValue(1) * z->GetValue(0));

	l1->InsertNextValue((y->GetValue(1) * z->GetValue(2) - y->GetValue(2) * z->GetValue(1)) / r); // b2c3 - c2b3
	l1->InsertNextValue((y->GetValue(2) * z->GetValue(0) - y->GetValue(0) * z->GetValue(2)) / r); // c1b3 - b1c3
	l1->InsertNextValue((y->GetValue(0) * z->GetValue(1) - y->GetValue(1) * z->GetValue(0)) / r); // b1c2 - c1b2

	l2->InsertNextValue((z->GetValue(1) * x->GetValue(2) - z->GetValue(2) * x->GetValue(1)) / r); // c2a3 - a2c3
	l2->InsertNextValue((z->GetValue(2) * x->GetValue(0) - z->GetValue(0) * x->GetValue(2)) / r); // a1c3 - c1a3
	l2->InsertNextValue((z->GetValue(0) * x->GetValue(1) - z->GetValue(1) * x->GetValue(0)) / r); // a2c1 - a1c2

	l3->InsertNextValue((x->GetValue(1) * y->GetValue(2) - x->GetValue(2) * y->GetValue(1)) / r); // a2b3 - b2a3
	l3->InsertNextValue((x->GetValue(2) * y->GetValue(0) - x->GetValue(0) * y->GetValue(2)) / r); // b1a3 - a1b3
	l3->InsertNextValue((x->GetValue(0) * y->GetValue(1) - x->GetValue(1) * y->GetValue(0)) / r); // a1b2 - b1a2
}

// 获得最长边长度
double getMaxLen(vtkSmartPointer<vtkSTLReader> &reader, vtkSmartPointer<vtkPolyData> &polydata) {
	double maxLen = 0;
	vtkSmartPointer<vtkExtractEdges> extractEdges = vtkSmartPointer<vtkExtractEdges>::New();
	extractEdges->SetInputConnection(reader->GetOutputPort());
	extractEdges->Update();
	for(int k = 0; k < extractEdges->GetOutput()->GetNumberOfCells(); ++k) {
		vtkSmartPointer<vtkLine> line = vtkLine::SafeDownCast(extractEdges->GetOutput()->GetCell(k));
		int id1 = line->GetPointId(0);
		int id2 = line->GetPointId(1);
		double p1[3];
		double p2[3];
		polydata->GetPoint(id1, p1);
		polydata->GetPoint(id2, p2);
		double len = vtkMath::Distance2BetweenPoints(p1, p2);
		if (len > maxLen || maxLen == 0) {
			maxLen = len;
		}
	}
	maxLen = sqrt(maxLen);
	return maxLen;
}
