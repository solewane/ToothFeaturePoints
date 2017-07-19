/************************************************************************/
/* Written by Tang Renjie in 2017/7                                     */
/*                                                                      */
/* Usage:                                                               */
/* feature.exe input_folder_path output_folder_path argument            */
/* argument:                                                            */
/* 0: output global feature                                             */
/* 1: output partial feature (PFH)                                      */
/************************************************************************/

#include <iostream>

#include "MeshModel.h"
#include "myAlgo.h"
#include "omp.h"

using namespace std;

int main(int argc, char *argv[]) {
	MeshModel model[28];
	for (int i = 0; i < 28; ++i) {
		model[i].readSTL(argv[1], i);
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

	// cout << "Data Prepare finished." << endl;

	if (string(argv[3]) == "1") {
		#pragma omp parallel for
		for (int i = 0; i < 28; ++i) {
			model[i].outputToFilePFH(argv[2]);
		}
	} else if (string(argv[3]) == "0") {
		#pragma omp parallel for
		for (int i = 0; i < 28; ++i) {
			model[i].outputToFilePos(argv[2]);
		}
	}

	// cout << "All Finished." << endl;

	return 0;
}
