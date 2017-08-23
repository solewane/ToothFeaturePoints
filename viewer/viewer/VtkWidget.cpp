#include "VtkWidget.h"

VtkWidget::VtkWidget(QWidget *parent) {
	QString base = MODEL_PATH;
	renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->SetBackground(.6, .6, .6);
	isShowFeature = false;
	showFeatureList[0] = "distal";
	showFeatureList[1] = "mesial";
	showFeatureList[2] = "fossa";
	showFeatureList[3] = "incisal";
	showFeatureList[4] = "cusp";
	for (int i = 0; i < 5; ++i) {
		showFeature[i] = true;
	}

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

		xCorMin[i] = xCorMax[i] = yCorMin[i] = yCorMax[i] = zCorMin[i] = zCorMax[i] = 0;

		massCenter[i] = vtkSmartPointer<vtkDoubleArray>::New();

		getCenterOfMass(polydata[i], massCenter[i]);
		getXYZ(polydata[i], xAxis[i], yAxis[i], zAxis[i]);
	}
	autoCheckPCA(polydata, massCenter, isValid, xAxis, yAxis, zAxis);
	for (int i = 0; i < 28; ++i) {
		getNewCor(i);
	}
	for (int i = 0; i < 28; ++i) {
		normalX[i] = vtkSmartPointer<vtkDoubleArray>::New();
		normalY[i] = vtkSmartPointer<vtkDoubleArray>::New();
		normalZ[i] = vtkSmartPointer<vtkDoubleArray>::New();
		maxLen[i] = 0;
		getNormal(i);
		maxLen[i] = getMaxLen(reader[i], polydata[i]);
	}
}

VtkWidget::~VtkWidget() { }

void VtkWidget::getNormal(int i) {
	vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalGenerator->SetInputData(polydata[i]);
	normalGenerator->ComputePointNormalsOn();
	normalGenerator->ComputeCellNormalsOff();
	normalGenerator->Update();
	vtkSmartPointer<vtkPolyData> polydataWithNormal = normalGenerator->GetOutput();
	vtkDataArray* normalsGeneric = polydataWithNormal->GetPointData()->GetNormals();

	int numPoints = polydata[i]->GetNumberOfPoints();
	normalX[i]->Reset();
	normalY[i]->Reset();
	normalZ[i]->Reset();
	for (int j = 0; j < numPoints; ++j)	{
		double vtkGenericNormal[3];
		normalsGeneric->GetTuple(j, vtkGenericNormal);

		normalX[i]->InsertNextValue(vtkGenericNormal[0]);
		normalY[i]->InsertNextValue(vtkGenericNormal[1]);
		normalZ[i]->InsertNextValue(vtkGenericNormal[2]);
	}
}

void VtkWidget::refreshVisible() {
	renderer->RemoveAllViewProps();
	for (int i = 0; i < 28; ++i) {
		if (isVisible[i]) {
			renderer->AddActor(actor[i]);
		}
	}
	if (isShowFeature) {
		// Read feature points from file
		string inPath = TMP_PATH + string("feature_points.txt");
		ifstream inFile(inPath);
		vector<string> featureNames;
		getFiles(POS_FEATURE_PATH, featureNames);
		vector<int> featurePoints(featureNames.size(), 0);

		while(!inFile.eof()) {
			string featureName;
			inFile >> featureName;
			if (inFile.eof()) {
				break;
			}
			int featureId;
			inFile >> featureId;
			for (int i = 0; i < featureNames.size(); ++i) {
				if (featureNames[i] == featureName) {
					featurePoints[i] = featureId;
				}
			}
		}
		for (int i = 0; i < featureNames.size(); ++i) {
			bool isInShowList = false;
			for (int j = 0; j < 5; ++j) {
				if (showFeature[j] && featureNames[i].find(showFeatureList[j]) != string::npos) {
					isInShowList = true;
				}
			}
			if (!isInShowList) {
				continue;
			}
			int id = (featureNames[i].at(0) - '1') * 7 + (featureNames[i].at(1) - '1');
			if (isValid[id] && isVisible[id]) {
				
				double p[3];
				polydata[id]->GetPoint(featurePoints[i], p);

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

	double rate = 1.0;
	bool isFound = false;
	while (!isFound) {
		for (int i = 0; i < polydata[id]->GetNumberOfPoints(); ++i) {
			double p[3];
			polydata[id]->GetPoint(i, p);
			double corP[3] = { xCor[id]->GetValue(i), yCor[id]->GetValue(i), zCor[id]->GetValue(i) };
			double corQ[3] = { (corP[0] - xCorMin[id]) / (xCorMax[id] - xCorMin[id]), (corP[1] - yCorMin[id]) / (yCorMax[id] - yCorMin[id]), (corP[2] - zCorMin[id]) / (zCorMax[id] - zCorMin[id]) };
			double a = (corQ[0] - cen[0]) * (corQ[0] - cen[0]) / (r[0] * r[0] * rate * rate);
			double b = (corQ[1] - cen[1]) * (corQ[1] - cen[1]) / (r[1] * r[1] * rate * rate);
			double c = (corQ[2] - cen[2]) * (corQ[2] - cen[2]) / (r[2] * r[2] * rate * rate);
			if (a + b + c < 1) {
				isFound = true;
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
		rate = rate * 1.2;
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

		if (a < xCorMin[id] || xCorMin[id] == 0) {
			xCorMin[id] = a;
		}
		if (a > xCorMax[id] || xCorMax[id] == 0) {
			xCorMax[id] = a;
		}

		if (b < yCorMin[id] || yCorMin[id] == 0) {
			yCorMin[id] = b;
		}
		if (b > yCorMax[id] || yCorMax[id] == 0) {
			yCorMax[id] = b;
		}

		if (c < zCorMin[id] || zCorMin[id] == 0) {
			zCorMin[id] = c;
		}
		if (c > zCorMax[id] || zCorMax[id] == 0) {
			zCorMax[id] = c;
		}

		xCor[id]->InsertNextValue(a);
		yCor[id]->InsertNextValue(b);
		zCor[id]->InsertNextValue(c);
	}
}

void VtkWidget::genFeature() {
	vector<string> featureNames;
	getFiles(POS_FEATURE_PATH, featureNames);
	string outPath = TMP_PATH + string("feature_points.txt");
	ofstream featureFile(outPath);
	for (int i = 0; i < featureNames.size(); ++i) {
		int id = (featureNames[i].at(0) - '1') * 7 + (featureNames[i].at(1) - '1');
		if (isValid[id]) {
			double cen[3];
			double r[3];

			string path = POS_FEATURE_PATH + string("\\") + featureNames[i] + string(".txt");
			ifstream inFile(path.c_str());
			for (int j = 0; j < 3; ++j) {
				inFile >> cen[j];
				inFile >> r[j];
			}
			inFile.close();

			bool isFound = false;
			double rate = 1.0;
			vector<int> validPoints;
			while (!isFound) {
				for (int j = 0; j < polydata[id]->GetNumberOfPoints(); ++j) {
					double p[3];
					polydata[id]->GetPoint(j, p);
					double corP[3] = { xCor[id]->GetValue(j), yCor[id]->GetValue(j), zCor[id]->GetValue(j) };
					double corQ[3] = { (corP[0] - xCorMin[id]) / (xCorMax[id] - xCorMin[id]), (corP[1] - yCorMin[id]) / (yCorMax[id] - yCorMin[id]), (corP[2] - zCorMin[id]) / (zCorMax[id] - zCorMin[id]) };
					double a = (corQ[0] - cen[0]) * (corQ[0] - cen[0]) / (r[0] * r[0] * rate * rate);
					double b = (corQ[1] - cen[1]) * (corQ[1] - cen[1]) / (r[1] * r[1] * rate * rate);
					double c = (corQ[2] - cen[2]) * (corQ[2] - cen[2]) / (r[2] * r[2] * rate * rate);
					if (a + b + c < 1) {
						isFound = true;
						validPoints.push_back(j);
					}
				}
				rate = rate * 1.2;
			}
			string outPath = TMP_PATH + featureNames[i] + ".txt";
			ofstream outFile(outPath);
			for (int j = 0; j < validPoints.size(); ++j) {
				outFile << validPoints[j] << " ";
				vtkSmartPointer<vtkDoubleArray> histogram = vtkSmartPointer<vtkDoubleArray>::New();
				getPFH(id, validPoints[j], histogram);
				for (int k = 0; k < histogram->GetNumberOfTuples(); ++k) {
					outFile << histogram->GetValue(k) << " ";
				}
				outFile << endl;
			}
			outFile.close();
			
			vector<double> last_poss;
			vector<double> last_mu;
			double max_mu = -1.0;
			double min_mu = -1.0;

			// Get Last Poss
			if (false) {
				//
			} else {
				for (int j = 0; j < validPoints.size(); ++j) {
					double corP[3] = { xCor[id]->GetValue(validPoints[j]), yCor[id]->GetValue(validPoints[j]), zCor[id]->GetValue(validPoints[j]) };
					double corQ[3] = { (corP[0] - xCorMin[id]) / (xCorMax[id] - xCorMin[id]), (corP[1] - yCorMin[id]) / (yCorMax[id] - yCorMin[id]), (corP[2] - zCorMin[id]) / (zCorMax[id] - zCorMin[id]) };
					double a = (corQ[0] - cen[0]) * (corQ[0] - cen[0]) / (r[0] * r[0] * rate * rate);
					double b = (corQ[1] - cen[1]) * (corQ[1] - cen[1]) / (r[1] * r[1] * rate * rate);
					double c = (corQ[2] - cen[2]) * (corQ[2] - cen[2]) / (r[2] * r[2] * rate * rate);
					double mu = a + b + c;
					last_mu.push_back(mu);
					if (max_mu == -1.0 || mu > max_mu) {
						max_mu = mu;
					}
					if (min_mu == -1.0 || mu < min_mu) {
						min_mu = mu;
					}
				}
				for (int j = 0; j < validPoints.size(); ++j) {
					last_mu[j] = (last_mu[j] - min_mu) / (max_mu - min_mu);
					double current_last_poss = exp(0 - last_mu[j]);
					last_poss.push_back(current_last_poss);
				}
			}

			// Output to File
			double max_poss = 0;
			int max_p_id = 0;
			for (int j = 0; j < validPoints.size(); ++j) {
				double possTmp = last_poss[j];
				if (max_poss == 0 || possTmp > max_poss) {
					max_poss = possTmp;
					max_p_id = validPoints[j];
				}
			}
			featureFile << featureNames[i] << " " << max_p_id << endl;
		}
	}
	featureFile.close();
	/*
	#pragma omp parallel for
	for (int i = 0; i < featureNames.size(); ++i) {
		int id = (featureNames[i].at(0) - '1') * 7 + (featureNames[i].at(1) - '1');
		if (isValid[id]) {
			string command = string("python ..\\..\\train\\predict.py ") + TMP_PATH + featureNames[i] + string(".txt ") + featureNames[i] + string(" ") + TMP_PATH;
			WinExec(command.c_str(), SW_HIDE);
		}
	}
	string outPath = TMP_PATH + string("feature_points.txt");
	ofstream outFile(outPath);
	for (int i = 0; i < featureNames.size(); ++i) {
		int id = (featureNames[i].at(0) - '1') * 7 + (featureNames[i].at(1) - '1');
		if (isValid[id]) {
			string path = TMP_PATH + featureNames[i] + string("_out.txt");
			ifstream inFile(path.c_str());
			int max_p_id = -1;
			vector<int> p_id;
			vector<double> front_poss;
			vector<double> last_poss;
			vector<double> possibility;
			double max_poss = 0;

			double minTmp = 1, maxTmp = -1;
			if (!inFile) {
				
			} else {
				while (!inFile.eof()) {
					int tmpId;
					double tmpPoss;
					inFile >> tmpId;
					if (inFile.eof()) {
						break;
					}
					inFile >> tmpPoss;
					p_id.push_back(tmpId);
					last_poss.push_back(tmpPoss);
					if (tmpPoss > maxTmp) {
						maxTmp = tmpPoss;
					}
					if (tmpPoss < minTmp) {
						minTmp = tmpPoss;
					}
				}
				inFile.close();
			}

			int n = p_id.size();
			for (int j = 0; j < n; ++j) {
				if (maxTmp == minTmp) {
					last_poss[j] = 1;
				}
				last_poss[j] = (last_poss[j] - minTmp) / (maxTmp - minTmp);
				//last_poss[j] = 1;
			}

			string featureName = featureNames[i];
			double mu = 0, x = 0, z = 0, xWTF = 0;
			double sigma = 0;
			double t = 1e-6;
			int useGaussian = 0; // 1 - use Z 2 - use X

			// [1, 2, 3, 4][1, 2, 3]_center_[distal, mesial]
			if ((featureName.at(1) - '1') < 3 && (featureName.find("distal") != -1 || featureName.find("mesial") != -1)) {
				useGaussian = 1;

				if (featureName.find("distal") != -1) {
					if (featureName.at(0) == '1' || featureName.at(0) == '3') {
						mu = zCorMax[id];
					} else {
						mu = zCorMin[id];
					}
				} else {
					if (featureName.at(0) == '1' || featureName.at(0) == '3') {
						mu = zCorMin[id];
					} else {
						mu = zCorMax[id];
					}
				}
				xWTF = (zCorMin[id] + zCorMax[id]) / 2;
			}

			// [1, 2, 3, 4][1_incisal, 2_cusp]
			if ((featureName.at(1) == '1' && featureName.find("incisal") != -1) || (featureName.at(1) == '2' && featureName.find("cusp") != -1)) {
				useGaussian = 1;
				mu = (zCorMin[id] + zCorMax[id]) / 2;
				xWTF = max(zCorMax[id], zCorMin[id]);
			}

			// [1, 2, 3, 4][3_cusp]
			if (featureName.at(1) == '2' && featureName.find("cusp") != -1) {
				useGaussian = 2;
				mu = xCorMax[id];
				xWTF = (xCorMax[id] + xCorMin[id]) / 2;
			}

			sigma = abs(xWTF - mu) / sqrt(2 * log(1 / t));

			for (int j = 0; j < n; ++j) {
				double frontTmp = 1;
				x = xCor[id]->GetValue(p_id[j]);
				z = zCor[id]->GetValue(p_id[j]);
				if (useGaussian == 1) {
					frontTmp = exp(0 - ((z - mu) * (z - mu)) / (2 * sigma * sigma));
				} else if (useGaussian == 2) {
					frontTmp = exp(0 - ((x - mu) * (x - mu)) / (2 * sigma * sigma));
				}
				front_poss.push_back(frontTmp);
			}

			for (int j = 0; j < n; ++j) {
				double possTmp = front_poss[j] * last_poss[j];
				possibility.push_back(possTmp);
				if (max_poss == 0 || possTmp > max_poss) {
					max_poss = possTmp;
					max_p_id = p_id[j];
				}
			}
			outFile << featureName << " " << max_p_id << endl;
		}
	}
	outFile.close();
	*/
}

void VtkWidget::getFiles(string path, vector<string>& files) {
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

void VtkWidget::getPFH(int i, int j, vtkSmartPointer<vtkDoubleArray> &histogram) {
	histogram->Reset();
	int rb = 3;
	int rd = 3;
	pfh(polydata[i], j, rd, rb, histogram, normalX[i], normalY[i], normalZ[i], maxLen[i]);
}

void VtkWidget::showhideFeature() {
	if (isShowFeature) {
		isShowFeature = false;
	} else {
		isShowFeature = true;
	}
	refreshVisible();
}

void VtkWidget::modifyFeature(int id) {
	if (showFeature[id]) {
		showFeature[id] = false;
	} else {
		showFeature[id] = true;
	}
	refreshVisible();
}
