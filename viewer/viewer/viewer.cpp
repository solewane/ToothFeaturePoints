#include "viewer.h"

viewer::viewer(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags) {
	ui.setupUi(this);

	mainWidget = new QWidget(this);
	mainLayout = new QHBoxLayout(this);
	vtkWidget = new VtkWidget(this);
	rightLayout = new QVBoxLayout(this);
	groupLayout = new QGridLayout(this);
	for (int i = 0; i < 28; ++i) {
		checkbox[i] = new QCheckBox(this);
		checkbox[i]->setChecked(true);
	}
	mapper = new QSignalMapper(this);
	showHideLayout = new QHBoxLayout(this);
	showAllBtn = new QPushButton(this);
	hideAllBtn = new QPushButton(this);

	showAllBtn->setText("Show All");
	hideAllBtn->setText("Hide All");

	mainLayout->addWidget(vtkWidget);
	mainLayout->addLayout(rightLayout);
	mainWidget->setLayout(mainLayout);
	for (int i = 0; i < 7; ++i) {
		for (int j = 0; j < 4; ++j) {
			int id = j * 7 + i;
			checkbox[id]->setText(QString::number(j + 1) + QString::number(i + 1));
			groupLayout->addWidget(checkbox[id], i, j, 1, 1);
			connect(checkbox[id], SIGNAL(clicked()), mapper, SLOT(map()));
			mapper->setMapping(checkbox[id], id);
		}
	}
	connect(mapper, SIGNAL(mapped(const int)), vtkWidget, SLOT(setToothVisible(int)));
	rightLayout->addLayout(groupLayout);
	showHideLayout->addWidget(showAllBtn);
	showHideLayout->addWidget(hideAllBtn);
	rightLayout->addLayout(showHideLayout);

	vtkWidget->setFixedSize(800, 600);

	this->setCentralWidget(mainWidget);

	
	connect(showAllBtn, SIGNAL(clicked()), this, SLOT(showAllTeeth()));
	connect(hideAllBtn, SIGNAL(clicked()), this, SLOT(hideAllTeeth()));
	connect(showAllBtn, SIGNAL(clicked()), vtkWidget, SLOT(showAllTeeth()));
	connect(hideAllBtn, SIGNAL(clicked()), vtkWidget, SLOT(hideAllTeeth()));
}

viewer::~viewer() { }

void viewer::showAllTeeth() {
	for (int i = 0; i < 28; ++i) {
		checkbox[i]->setChecked(true);
	}
}

void viewer::hideAllTeeth() {
	for (int i = 0; i < 28; ++i) {
		checkbox[i]->setChecked(false);
	}
}
