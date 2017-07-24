#include "utility.h"

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

void getXYZ(vtkSmartPointer<vtkPolyData> &polydata, vtkDoubleArray *x, vtkDoubleArray *y, vtkDoubleArray *z) {
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

void autoCheckPCA(vtkSmartPointer<vtkPolyData> polydata[28], vtkSmartPointer<vtkDoubleArray> center[28], bool isValid[28], 
	vtkSmartPointer<vtkDoubleArray> xAxis[28], vtkSmartPointer<vtkDoubleArray> yAxis[28], vtkSmartPointer<vtkDoubleArray> zAxis[28]) {
	double yArr[3] = { center[4]->GetValue(0) + center[11]->GetValue(0) - center[0]->GetValue(0) - center[7]->GetValue(0), 
		center[4]->GetValue(1) + center[11]->GetValue(1) - center[0]->GetValue(1) - center[7]->GetValue(1), 
		center[4]->GetValue(2) + center[11]->GetValue(2) - center[0]->GetValue(2) - center[7]->GetValue(2) };
	double zArr[3] = { center[0]->GetValue(0) - center[14]->GetValue(0), center[0]->GetValue(1) - center[14]->GetValue(1), center[0]->GetValue(2) - center[14]->GetValue(2) };
	double normY = vtkMath::Norm(yArr);
	double normZ = vtkMath::Norm(zArr);
	for (int i = 0; i < 3; ++i) {
		yArr[i] /= -normY;
		zArr[i] /= normZ;
	}
	for (int i = 0; i < 28; ++i) {
		if (!isValid[i]) {
			continue;
		}
		int quadrant = i / 7 + 1;
		int id = i % 7 + 1;
		double tmpX[3] = { xAxis[i]->GetValue(0), xAxis[i]->GetValue(1), xAxis[i]->GetValue(2) };
		double product = vtkMath::Dot(tmpX, zArr);
		if ((quadrant <= 2 && product > 0) || (quadrant > 2 && product < 0)) {
			for (int j = 0; j < 3; ++j)	{
				xAxis[i]->SetValue(j, -xAxis[i]->GetValue(j));
			}
		}
	}

	for (int i = 0; i < 28; ++i) {
		if (!isValid[i]) {
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
		if (!isValid[leftId])	{
			leftId = i;
		}
		if (!isValid[rightId])	{
			rightId = i;
		}

		double standard[3] = { center[rightId]->GetValue(0) - center[leftId]->GetValue(0), 
			center[rightId]->GetValue(1) - center[leftId]->GetValue(1), 
			center[rightId]->GetValue(2) - center[leftId]->GetValue(2) };
		double tmpY[3] = { yAxis[i]->GetValue(0), yAxis[i]->GetValue(1), yAxis[i]->GetValue(2) };
		double tmpZ[3] = { zAxis[i]->GetValue(0), zAxis[i]->GetValue(1), zAxis[i]->GetValue(2) };
		double product1 = vtkMath::Dot(tmpY, standard) / (vtkMath::Norm(standard) * vtkMath::Norm(tmpY));
		double product2 = vtkMath::Dot(tmpZ, standard) / (vtkMath::Norm(standard) * vtkMath::Norm(tmpZ));
		if (abs(product1) > abs(product2)) {
			for (int j = 0; j < 3; ++j)	{
				double tmp = yAxis[i]->GetValue(j);
				yAxis[i]->SetValue(j, zAxis[i]->GetValue(j));
				zAxis[i]->SetValue(j, tmp);
			}
		}
	}

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 7; ++j) {
			int id = i * 7 + j;
			if (!isValid[i])	{
				continue;
			}
			double tmpY[3] = { yAxis[id]->GetValue(0), yAxis[id]->GetValue(1), yAxis[id]->GetValue(2) };
			if (j == 0) {
				double product = vtkMath::Dot(tmpY, yArr);
				if (product < 0) {
					for (int k = 0; k < 3; ++k)	{
						yAxis[id]->SetValue(k, -yAxis[id]->GetValue(k));
					}
				}
			} else {
				int lastYid = id - 1;
				if (!isValid[id - 1])	{
					lastYid = id - 2;
				}
				double lastY[3] = { yAxis[lastYid]->GetValue(0), yAxis[lastYid]->GetValue(1), yAxis[lastYid]->GetValue(2) };
				double product = vtkMath::Dot(tmpY, lastY);
				if (product < 0) {
					for (int k = 0; k < 3; ++k)	{
						yAxis[id]->SetValue(k, -yAxis[id]->GetValue(k));
					}
				}
			}
		}
	}

	for (int i = 0; i < 28; ++i) {
		if (!isValid[i]) {
			continue;
		}
		double tmpX[3] = { xAxis[i]->GetValue(0), xAxis[i]->GetValue(1), xAxis[i]->GetValue(2) };
		double tmpY[3] = { yAxis[i]->GetValue(0), yAxis[i]->GetValue(1), yAxis[i]->GetValue(2) };
		double tmp[3];
		vtkMath::Cross(tmpX, tmpY, tmp);
		for (int j = 0; j < 3; ++j)	{
			zAxis[i]->SetValue(j, tmp[j]);
		}
	}
}

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
