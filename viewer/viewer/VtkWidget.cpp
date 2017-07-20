#include "VtkWidget.h"

VtkWidget::VtkWidget(QWidget *parent) {
	QString base = "E:\\train\\";
	renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->SetBackground(.6, .6, .6);

	// Draw Mesh
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 7; ++j) {
			int id = i * 7 + j;
			QString path = base + QString::number(i + 1) + QString::number(j + 1) + ".stl";

			isVisible[id] = true;

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
		for (int j = 0; j < 3; ++j) {
			range[i][j] = 0;
		}
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

void VtkWidget::showRange() {
	for (int i = 0; i < 28; ++i) {
		if (isVisible[i]) {
			for (int j = 0; j < polydata[i]->GetNumberOfPoints(); ++j) {
				//
			}
		}
	}
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
