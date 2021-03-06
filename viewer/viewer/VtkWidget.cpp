#include "VtkWidget.h"

VtkWidget::VtkWidget(QWidget *parent) {
	QString base = MODEL_PATH;
	renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->SetBackground(.8, .8, .8);
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

		if (isValid[i])	{
			getCenterOfMass(polydata[i], massCenter[i]);
			getXYZ(polydata[i], xAxis[i], yAxis[i], zAxis[i]);
		}
	}
	autoCheckPCA(polydata, massCenter, isValid, xAxis, yAxis, zAxis);
	for (int i = 0; i < 28; ++i) {
		if (isValid[i])	{
			getNewCor(i);
		}
	}
	for (int i = 0; i < 28; ++i) {
		normalX[i] = vtkSmartPointer<vtkDoubleArray>::New();
		normalY[i] = vtkSmartPointer<vtkDoubleArray>::New();
		normalZ[i] = vtkSmartPointer<vtkDoubleArray>::New();
		maxLen[i] = 0;
		if (isValid[i]) {
			getNormal(i);
			maxLen[i] = getMaxLen(reader[i], polydata[i]);
		}
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
			// showAxis(i);
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

void VtkWidget::showAxis(int id) {
	int len = 20;
	vtkSmartPointer<vtkLineSource> xLineSource = vtkSmartPointer<vtkLineSource>::New();
	xLineSource->SetPoint1(massCenter[id]->GetValue(0), massCenter[id]->GetValue(1), massCenter[id]->GetValue(2));
	xLineSource->SetPoint2(massCenter[id]->GetValue(0) + xAxis[id]->GetValue(0) * len, massCenter[id]->GetValue(1) + xAxis[id]->GetValue(1) * len, massCenter[id]->GetValue(2) + xAxis[id]->GetValue(2) * len);
	vtkSmartPointer<vtkPolyDataMapper> xLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	xLineMapper->SetInputConnection(xLineSource->GetOutputPort());
	vtkSmartPointer<vtkActor> xLineActor = vtkSmartPointer<vtkActor>::New();
	xLineActor->SetMapper(xLineMapper);
	xLineActor->GetProperty()->SetColor(255, 0, 0);
	xLineActor->GetProperty()->SetLineWidth(2);
	renderer->AddActor(xLineActor);

	vtkSmartPointer<vtkLineSource> yLineSource = vtkSmartPointer<vtkLineSource>::New();
	yLineSource->SetPoint1(massCenter[id]->GetValue(0), massCenter[id]->GetValue(1), massCenter[id]->GetValue(2));
	yLineSource->SetPoint2(massCenter[id]->GetValue(0) + yAxis[id]->GetValue(0) * len, massCenter[id]->GetValue(1) + yAxis[id]->GetValue(1) * len, massCenter[id]->GetValue(2) + yAxis[id]->GetValue(2) * len);
	vtkSmartPointer<vtkPolyDataMapper> yLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	yLineMapper->SetInputConnection(yLineSource->GetOutputPort());
	vtkSmartPointer<vtkActor> yLineActor = vtkSmartPointer<vtkActor>::New();
	yLineActor->SetMapper(yLineMapper);
	yLineActor->GetProperty()->SetColor(0, 255, 0);
	yLineActor->GetProperty()->SetLineWidth(2);
	renderer->AddActor(yLineActor);

	vtkSmartPointer<vtkLineSource> zLineSource = vtkSmartPointer<vtkLineSource>::New();
	zLineSource->SetPoint1(massCenter[id]->GetValue(0), massCenter[id]->GetValue(1), massCenter[id]->GetValue(2));
	zLineSource->SetPoint2(massCenter[id]->GetValue(0) + zAxis[id]->GetValue(0) * len, massCenter[id]->GetValue(1) + zAxis[id]->GetValue(1) * len, massCenter[id]->GetValue(2) + zAxis[id]->GetValue(2) * len);
	vtkSmartPointer<vtkPolyDataMapper> zLineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	zLineMapper->SetInputConnection(zLineSource->GetOutputPort());
	vtkSmartPointer<vtkActor> zLineActor = vtkSmartPointer<vtkActor>::New();
	zLineActor->SetMapper(zLineMapper);
	zLineActor->GetProperty()->SetColor(0, 0, 255);
	zLineActor->GetProperty()->SetLineWidth(2);
	renderer->AddActor(zLineActor);

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
	// Get CNN Result
	/*
	vector<int> cnnPos(32, 0);
	for (int i = 0; i < 4; ++i) {
		for (int j = 5; j < 7; ++j) {
			if (!isValid[i * 7 + j]) {
				continue;
			}
			stringstream imageStream;
			imageStream << TMP_PATH << i * 7 + j << ".bmp";
			outputToImage(imageStream.str(), i * 7 + j);
			stringstream idStream;
			idStream << i * 7 + j;
			string command = string("python ..\\..\\cnn\\predict.py ") + idStream.str() + " " + TMP_PATH;
			WinExec(command.c_str(), SW_HIDE);
			string predictFile = TMP_PATH + idStream.str() + string(".txt");
			ifstream inFile(predictFile);
			int a, b, c, d;
			inFile >> a >> b >> c >> d;
			cnnPos[i * 8 + (j - 5) * 4] = a;
			cnnPos[i * 8 + (j - 5) * 4 + 1] = b;
			cnnPos[i * 8 + (j - 5) * 4 + 2] = c;
			cnnPos[i * 8 + (j - 5) * 4 + 3] = d;
			inFile.close();
		}
	}*/
	int usingLocalInfoFeaturesNum = 6;
	string usingLocalInfoFeatures[] = { "4_center_distal", "4_center_mesial", "4_cusp", "5_center_distal", "5_center_mesial", "5_cusp" };
	int usingCNNNum = 4;
	string usingCNNFeatures[] = { "6_center_distal", "6_center_mesial", "7_center_distal", "7_center_mesial" };
	vector<string> featureNames;
	getFiles(POS_FEATURE_PATH, featureNames);
	string outFeaturePath = TMP_PATH + string("feature_points.txt");
	ofstream featureFile(outFeaturePath);
	//ofstream featurePosFile("E:\\validate\\16.txt");
	for (int i = 0; i < featureNames.size(); ++i) {
		int id = (featureNames[i].at(0) - '1') * 7 + (featureNames[i].at(1) - '1');
		if (isValid[id]) {
			// Get Valid Points Using position information
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

			vector<double> last_poss;
			vector<double> local_poss;
			vector<double> front_poss;
			vector<double> cnn_poss;
			vector<double> last_mu;
			double max_mu = -1.0;
			double min_mu = -1.0;

			// Get Last Poss
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
				front_poss.push_back(0.5);
				local_poss.push_back(0.5);
				cnn_poss.push_back(1.0);
				last_poss.push_back(current_last_poss);
			}
			
			// Get Local Poss
			bool usingLocalInfo = false;
			for (int j = 0; j < usingLocalInfoFeaturesNum; ++j) {
				if (featureNames[i].substr(1) == usingLocalInfoFeatures[j]) {
					usingLocalInfo = true;
				}
			}
			if (usingLocalInfo) {
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
				string command = string("python ..\\..\\train\\predict.py ") + TMP_PATH + featureNames[i] + string(".txt ") + featureNames[i] + string(" ") + TMP_PATH;
				WinExec(command.c_str(), SW_HIDE);
				string inLocalPath = TMP_PATH + featureNames[i] + string("_out.txt");
				ifstream inLocalFile(inLocalPath.c_str());
				double local_poss_max = -1.0, local_poss_min = -1.0;
				if (inLocalFile) {
					while (!inLocalFile.eof()) {
						int tmpId;
						double tmpPoss;
						inFile >> tmpId;
						if (inFile.eof()) {
							break;
						}
						inFile >> tmpPoss;
						int p_id = -1;
						for (int j = 0; j < validPoints.size(); ++j) {
							if (validPoints[j] == tmpId) {
								local_poss[j] = tmpPoss;
								if (tmpPoss > local_poss_max || local_poss_max == -1.0) {
									local_poss_max = tmpPoss;
								}
								if (tmpPoss < local_poss_min || local_poss_min == -1.0) {
									local_poss_min = tmpPoss;
								}
							}
						}
					}
					inFile.close();
				}
				if (local_poss_max != -1.0 && local_poss_min != -1.0) {
					for (int j = 0; j < validPoints.size(); ++j) {
						local_poss[j] = (local_poss[j] - local_poss_min) / (local_poss_max - local_poss_min) * 0.5 + 0.5;
					}
				}
			}

			// Get Front Poss
			string featureName = featureNames[i];
			double mu = 0, x = 0, z = 0, xWTF = 0;
			double sigma = 0;
			double t = 0.25;
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
			if (featureName.at(1) == '3' && featureName.find("cusp") != -1) {
				useGaussian = 2;
				mu = xCorMax[id];
				xWTF = (xCorMax[id] + xCorMin[id]) / 2;
			}

			// [1, 2, 3, 4][6, 7][_buccal_][distal, mesial][_cusp]
			if ((featureName.at(1) == '6' || featureName.at(1) == '7') && featureName.find("cusp") != -1) {
				useGaussian = 2;
				mu = xCorMax[id];
				xWTF = (xCorMax[id] + xCorMin[id]) / 2;
			}

			sigma = abs(xWTF - mu) / sqrt(2 * log(1 / t));

			for (int j = 0; j < validPoints.size(); ++j) {
				double frontTmp = 1;
				x = xCor[id]->GetValue(validPoints[j]);
				z = zCor[id]->GetValue(validPoints[j]);
				if (useGaussian == 1) {
					frontTmp = exp(0 - ((z - mu) * (z - mu)) / (2 * sigma * sigma));
				} else if (useGaussian == 2) {
					frontTmp = exp(0 - ((x - mu) * (x - mu)) / (2 * sigma * sigma));
				}
				front_poss[j] = frontTmp;
			}

			// Get CNN Poss
			/*
			bool usingCNN = false;
			for (int j = 0; j < usingCNNNum; ++j) {
				if (featureNames[i].substr(1) == usingCNNFeatures[j]) {
					usingCNN = true;
				}
			}
			if (usingCNN) {
				int cnnX = 0, cnnY = 0;
				if (featureNames[i].find("distal") != -1) {
					cnnX = cnnPos[(id / 7) * 8 + (id % 7 - 5) * 4];
					cnnY = cnnPos[(id / 7) * 8 + (id % 7 - 5) * 4 + 1];
				} else {
					cnnX = cnnPos[(id / 7) * 8 + (id % 7 - 5) * 4 + 2];
					cnnY = cnnPos[(id / 7) * 8 + (id % 7 - 5) * 4 + 3];
				}
				double minR = -1.0, maxR = -1.0;
				int xSize = 64;
				int ySize = 64;
				int xRealSize = 0, yRealSize = 0;
				if ((yCorMax[id] - yCorMin[id]) > (zCorMax[id] - zCorMin[id])) {
					xRealSize = xSize * 0.75;
					yRealSize = xSize * (zCorMax[id] - zCorMin[id]) / (yCorMax[id] - yCorMin[id]) * 0.75;
				} else {
					xRealSize = ySize * (yCorMax[id] - yCorMin[id]) / (zCorMax[id] - zCorMin[id]) * 0.75;
					yRealSize = ySize * 0.75;
				}
				for (int j = 0; j < validPoints.size(); ++j) {
					int currX = (yCor[id]->GetValue(validPoints[j]) - yCorMin[id]) / (yCorMax[id] - yCorMin[id]) * xRealSize + (xSize - xRealSize) / 2;
					int currY = (zCor[id]->GetValue(validPoints[j]) - zCorMin[id]) / (zCorMax[id] - zCorMin[id]) * yRealSize + (ySize - yRealSize) / 2;
					double currR = sqrt(double((currX - cnnX) * (currX - cnnX) + (currY - cnnY) * (currY - cnnY)));
					if (minR == -1.0 || currR < minR) {
						minR = currR;
					}
					if (maxR == -1.0 || currR > maxR) {
						maxR = currR;
					}
				}
				for (int j = 0; j < validPoints.size(); ++j) {
					int currX = (yCor[id]->GetValue(validPoints[j]) - yCorMin[id]) / (yCorMax[id] - yCorMin[id]) * xRealSize + (xSize - xRealSize) / 2;
					int currY = (zCor[id]->GetValue(validPoints[j]) - zCorMin[id]) / (zCorMax[id] - zCorMin[id]) * yRealSize + (ySize - yRealSize) / 2;
					double currR = (sqrt(double((currX - cnnX) * (currX - cnnX) + (currY - cnnY) * (currY - cnnY))) - minR) / (maxR - minR);
					cnn_poss[j] = exp(0 - currR * currR);
				}
			}*/

			// Output to File
			double max_poss = 0;
			int max_p_id = 0;
			for (int j = 0; j < validPoints.size(); ++j) {
				double possTmp = front_poss[j] * last_poss[j];
				if (usingLocalInfo) {
					possTmp *= local_poss[j];
				}
				/*if (usingCNN) {
					possTmp *= cnn_poss[j];
				}*/
				if (max_poss == 0 || possTmp > max_poss) {
					max_poss = possTmp;
					max_p_id = validPoints[j];
				}
			}
			featureFile << featureNames[i] << " " << max_p_id << endl;
			/*
			double tmpOriginPos[3], tmpPos[3];
			polydata[id]->GetPoint(max_p_id, tmpOriginPos);
			string matrix_path = "E:\\data\\matrices\\matrix_16.txt";
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
			double n[4];
			for (int j = 0; j < 4; ++j) {
				n[j] = m->GetElement(j, 0) * tmpOriginPos[0] + m->GetElement(j, 1) * tmpOriginPos[1] + m->GetElement(j, 2) * tmpOriginPos[2] + m->GetElement(j, 3);
			}
			for (int j = 0; j < 3; ++j) {
				tmpPos[j] = n[j] / n[3];
			}
			featurePosFile << featureNames[i] << " " << tmpPos[0] << " " << tmpPos[1] << " " << tmpPos[2] << endl;*/
		}
	}
	featureFile.close();
	//featurePosFile.close();
}

void VtkWidget::outputToImage(string filePath, int id) {
	if (!isValid[id]) {
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
	for (int i = 0; i < polydata[id]->GetNumberOfPoints(); ++i) {
		int xPos = (yCor[id]->GetValue(i) - yCorMin[id]) / (yCorMax[id] - yCorMin[id]) * xRealSize + (xSize - xRealSize) / 2;
		int yPos = (zCor[id]->GetValue(i) - zCorMin[id]) / (zCorMax[id] - zCorMin[id]) * yRealSize + (ySize - yRealSize) / 2;
		for (int x = xPos; x <= xPos + 1; ++x) {
			for (int y = yPos; y <= yPos + 1; ++y) {
				int xCurrPos = x >= xSize ? (xSize - 1) : x;
				int yCurrPos = y >= ySize ? (ySize - 1) : y;
				if (xCor[id]->GetValue(i) <= 0) {
					continue;
				}
				height[xCurrPos][yCurrPos] = xCor[id]->GetValue(i) > height[xCurrPos][yCurrPos] ? xCor[id]->GetValue(i) : height[xCurrPos][yCurrPos];
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
