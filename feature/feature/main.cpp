#include <iostream>

#include "MeshModel.h"
#include "myAlgo.h"
#include "omp.h"

using namespace std;

int main(int argc, char *argv[]) {
	MeshModel model[28];
	for (int i = 0; i < 28; ++i) {
		model[i].readSTL("fuse", i);
	}

	// 颌部主成分分析坐标轴
	vtkSmartPointer<vtkDoubleArray> mainX = vtkSmartPointer<vtkDoubleArray>::New();
	vtkSmartPointer<vtkDoubleArray> mainY = vtkSmartPointer<vtkDoubleArray>::New();
	vtkSmartPointer<vtkDoubleArray> mainZ = vtkSmartPointer<vtkDoubleArray>::New();
	getXYZ(model, mainX, mainY, mainZ);
	
	for (int i = 0; i < 28; ++i) {
		model[i].update();
	}
	autoCheckPCA(model, mainX, mainY, mainZ);
	for (int i = 0; i < 28; ++i) {
		model[i].getNewCor();
	}

	cout << "Data Prepare finished." << endl;

	#pragma omp parallel for
	for (int i = 0; i < 28; ++i) {
		model[i].outputToFile("output");
		//cout << i << " Finished!" << endl;
	}

	cout << "All Finished." << endl;
}
