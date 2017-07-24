#include "VtkWidget.h"

VtkWidget::VtkWidget(QWidget *parent) {
	QString base = MODEL_PATH;
	renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->SetBackground(.6, .6, .6);

	// Draw Mesh
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 7; ++j) {
			int id = i * 7 + j;
			QString path = base + QString::number(i + 1) + QString::number(j + 1) + ".stl";

			ifstream inFile(path.toStdString().c_str());
			if (!inFile) {
				isValid[id] = false;
				continue;
			}
			inFile.close();

			isVisible[id] = true;
			isValid[id] = true;

			reader[id] = vtkSmartPointer<vtkSTLReader>::New();
			reader[id]->SetFileName(path.toStdString().c_str());
			reader[id]->Update();

			polydata[id] = vtkSmartPointer<vtkPolyData>::New();
			polydata[id]->DeepCopy(reader[id]->GetOutput());

			mapper[id] = vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper[id]->SetInputConnection(reader[id]->GetOutputPort());

			actor[id] = vtkSmartPointer<vtkActor>::New();
			actor[id]->SetMapper(mapper[id]);

			renderer->AddActor(actor[id]);
		}
	}

	this->GetRenderWindow()->AddRenderer(renderer);
	this->GetRenderWindow()->Render();

	for (int i = 0; i < 28; ++i) {
		xAxis[i] = vtkSmartPointer<vtkDoubleArray>::New();
		yAxis[i] = vtkSmartPointer<vtkDoubleArray>::New();
		zAxis[i] = vtkSmartPointer<vtkDoubleArray>::New();

		xCor[i] = vtkSmartPointer<vtkDoubleArray>::New();
		yCor[i] = vtkSmartPointer<vtkDoubleArray>::New();
		zCor[i] = vtkSmartPointer<vtkDoubleArray>::New();

		massCenter[i] = vtkSmartPointer<vtkDoubleArray>::New();

		getCenterOfMass(polydata[i], massCenter[i]);
		getXYZ(polydata[i], xAxis[i], yAxis[i], zAxis[i]);
	}
	autoCheckPCA(polydata, massCenter, isValid, xAxis, yAxis, zAxis);
	for (int i = 0; i < 28; ++i) {
		getNewCor(i);
	}
}

VtkWidget::~VtkWidget() { }

void VtkWidget::refreshVisible() {
	renderer->RemoveAllViewProps();
	for (int i = 0; i < 28; ++i) {
		if (isVisible[i]) {
			renderer->AddActor(actor[i]);
		}
	}
	this->GetRenderWindow()->Render();
}

void VtkWidget::showRange(string filename) {
	refreshVisible();
	if (filename.length() == 0) {
		return;
	}

	double cen[3];
	double r[3];

	string path = POS_FEATURE_PATH + string("\\") + filename + string(".txt");
	ifstream inFile(path.c_str());
	if (!inFile) {
		return;
	}
	for (int i = 0; i < 3; ++i) {
		inFile >> cen[i];
		inFile >> r[i];
	}
	inFile.close();

	int id = (filename.at(0) - '1') * 7 + (filename.at(1) - '1');

	if (!isVisible[id]) {
		return;
	}

	for (int i = 0; i < polydata[id]->GetNumberOfPoints(); ++i) {
		double p[3];
		polydata[id]->GetPoint(i, p);
		double corP[3] = { xCor[id]->GetValue(i), yCor[id]->GetValue(i), zCor[id]->GetValue(i) };
		if ((corP[0] - cen[0]) * (corP[0] - cen[0]) / (r[0] * r[0]) + (corP[1] - cen[1]) * (corP[1] - cen[1]) / (r[1] * r[1]) + (corP[2] - cen[2]) * (corP[2] - cen[2]) / (r[2] * r[2]) < 1) {
			vtkSmartPointer<vtkSphereSource> pointSource = vtkSmartPointer<vtkSphereSource>::New();
			pointSource->SetCenter(p[0], p[1], p[2]);
			pointSource->SetRadius(.5);
			vtkSmartPointer<vtkPolyDataMapper> pointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			pointMapper->SetInputConnection(pointSource->GetOutputPort());
			vtkSmartPointer<vtkActor> pointActor = vtkSmartPointer<vtkActor>::New();
			pointActor->SetMapper(pointMapper);
			pointActor->GetProperty()->SetColor(255, 0, 0);
			renderer->AddActor(pointActor);
		}
	}
	this->GetRenderWindow()->Render();
}

void VtkWidget::setToothVisible(int i) {
	if (isVisible[i]) {
		isVisible[i] = false;
	} else {
		isVisible[i] = true;
	}
	refreshVisible();
}

void VtkWidget::showAllTeeth() {
	for (int i = 0; i < 28; ++i) {
		isVisible[i] = true;
	}
	refreshVisible();
}

void VtkWidget::hideAllTeeth() {
	for (int i = 0; i < 28; ++i) {
		isVisible[i] = false;
	}
	refreshVisible();
}

void VtkWidget::getNewCor(int id) {
	if (!isValid) {
		return;
	}

	vtkSmartPointer<vtkDoubleArray> l1 = vtkSmartPointer<vtkDoubleArray>::New();
	vtkSmartPointer<vtkDoubleArray> l2 = vtkSmartPointer<vtkDoubleArray>::New();
	vtkSmartPointer<vtkDoubleArray> l3 = vtkSmartPointer<vtkDoubleArray>::New();

	getMatrix(xAxis[id], yAxis[id], zAxis[id], l1, l2, l3);

	for (int i = 0; i < polydata[id]->GetNumberOfPoints(); ++i) {
		double p[3];
		polydata[id]->GetPoint(i, p);

		double a = l1->GetValue(0) * (p[0] - massCenter[id]->GetValue(0)) + l1->GetValue(1) * (p[1] - massCenter[id]->GetValue(1)) + l1->GetValue(2) * (p[2] - massCenter[id]->GetValue(2));
		double b = l2->GetValue(0) * (p[0] - massCenter[id]->GetValue(0)) + l2->GetValue(1) * (p[1] - massCenter[id]->GetValue(1)) + l2->GetValue(2) * (p[2] - massCenter[id]->GetValue(2));
		double c = l3->GetValue(0) * (p[0] - massCenter[id]->GetValue(0)) + l3->GetValue(1) * (p[1] - massCenter[id]->GetValue(1)) + l3->GetValue(2) * (p[2] - massCenter[id]->GetValue(2));

		xCor[id]->InsertNextValue(a);
		yCor[id]->InsertNextValue(b);
		zCor[id]->InsertNextValue(c);
	}
}
