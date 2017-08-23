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
	getFiles(POS_FEATURE_PATH, filenames);
	radioLayout = new QVBoxLayout(this);
	scrollArea = new QScrollArea(this);
	radioGroup = new QButtonGroup(this);
	radioGroup->setExclusive(true);
	scrollWidget = new QWidget(this);
	scrollLayout = new QVBoxLayout(this);
	radioMapper = new QSignalMapper(this);
	QRadioButton *r = new QRadioButton(this);
	r->setText("None");
	connect(r, SIGNAL(clicked()), radioMapper, SLOT(map()));
	radioMapper->setMapping(r, -1);
	scrollLayout->addWidget(r);
	radioGroup->addButton(r);
	for (int i = 0; i < filenames.size(); ++i) {
		QRadioButton *r = new QRadioButton(this);
		r->setText(QString(filenames[i].c_str()));
		connect(r, SIGNAL(clicked()), radioMapper, SLOT(map()));
		radioMapper->setMapping(r, i);
		scrollLayout->addWidget(r);
		radioGroup->addButton(r);
	}
	connect(radioMapper, SIGNAL(mapped(const int)), this, SLOT(showValidRange(int)));
	scrollWidget->setLayout(scrollLayout);
	scrollArea->setWidget(scrollWidget);
	radioLayout->addWidget(scrollArea);
	featureLayout = new QVBoxLayout(this);
	featureMapper = new QSignalMapper(this);
	for (int i = 0; i < 5; ++i) {
		featureCheckbox[i] = new QCheckBox(this);
		featureLayout->addWidget(featureCheckbox[i]);
		connect(featureCheckbox[i], SIGNAL(clicked()), featureMapper, SLOT(map()));
		featureMapper->setMapping(featureCheckbox[i], i);
		featureCheckbox[i]->setChecked(true);
	}
	connect(featureMapper, SIGNAL(mapped(const int)), vtkWidget, SLOT(modifyFeature(int)));
	isGened = false;
	genFeatureBtn = new QPushButton(this);
	showPointBtn = new QPushButton(this);
	outputLabel = new QLabel(this);
	genLayout = new QHBoxLayout(this);

	showAllBtn->setText("Show All");
	hideAllBtn->setText("Hide All");
	genFeatureBtn->setText("Generate");
	showPointBtn->setText("Show/Hide Feature Points");
	outputLabel->setText("Unready");
	featureCheckbox[0]->setText("distal");
	featureCheckbox[1]->setText("mesial");
	featureCheckbox[2]->setText("fossa");
	featureCheckbox[3]->setText("incisal");
	featureCheckbox[4]->setText("cusp");

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
	rightLayout->addLayout(radioLayout);
	rightLayout->addLayout(featureLayout);
	genLayout->addWidget(genFeatureBtn);
	genLayout->addWidget(outputLabel);
	rightLayout->addLayout(genLayout);
	rightLayout->addWidget(showPointBtn);

	vtkWidget->setMinimumSize(800, 600);
	mainLayout->setStretchFactor(vtkWidget, 3);
	mainLayout->setStretchFactor(rightLayout, 1);

	this->setCentralWidget(mainWidget);

	connect(showAllBtn, SIGNAL(clicked()), this, SLOT(showAllTeeth()));
	connect(hideAllBtn, SIGNAL(clicked()), this, SLOT(hideAllTeeth()));
	connect(showAllBtn, SIGNAL(clicked()), vtkWidget, SLOT(showAllTeeth()));
	connect(hideAllBtn, SIGNAL(clicked()), vtkWidget, SLOT(hideAllTeeth()));
	connect(this, SIGNAL(showCertainRange(string)), vtkWidget, SLOT(showRange(string)));
	connect(genFeatureBtn, SIGNAL(clicked()), this, SLOT(generateFeature()));
	connect(this, SIGNAL(vtkGenFeature()), vtkWidget, SLOT(genFeature()));
	connect(showPointBtn, SIGNAL(clicked()), vtkWidget, SLOT(showhideFeature()));
	// genFeatureBtn->setEnabled(false);
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

void viewer::showValidRange(int id) {
	if (id == -1) {
		emit showCertainRange("");
	}
	emit showCertainRange(filenames[id]);
}

void viewer::getFiles(string path, vector<string>& files) {
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
				files.push_back(tmp.substr(0, tmp.find(".txt")));
			}
		} while(_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

void viewer::generateFeature() {
	emit vtkGenFeature();
	outputLabel->setText("Ready");
}
