#include <iostream>
#include <cstring>

#include "Mesh.h"

using namespace std;

int main(int argc, char *argv[]) {
	cout << argv[1] << endl;
	vector<string> files;
	getFiles(argv[1], files);
	for (int toothId = 0; toothId < files.size(); ++toothId) {
		string currFilename = files[toothId] + "\\teeth_stl\\";
		string matrixFilename = "E:\\data\\matrices\\matrix_" + files[toothId].substr(files[toothId].find("stl\\") + 4) + ".txt";
		cout << "- " << currFilename << "\t" << matrixFilename << endl;
		MeshModel model[28];
		for (int i = 0; i < 28; ++i) {
			model[i].readSTL(currFilename, matrixFilename, i);
		}

		for (int i = 0; i < 28; ++i) {
			model[i].update();
		}

		autoCheckPCA(model);

		for (int i = 0; i < 28; ++i) {
			model[i].getNewCor();
		}

		int modelToReslice[] = { 5, 6, 12, 13, 19, 20, 26, 27 };
		for (int i = 0; i < 8; ++i) {
			cout << "--- Tooth " << modelToReslice[i] << endl;
			stringstream s, t, u;
			s << "E:\\data\\img\\" << idToTooth(modelToReslice[i]) << "\\" << toothId << ".bmp";
			t << "E:\\data\\feature_points\\" + files[toothId].substr(files[toothId].find("stl\\") + 4) + ".txt";
			u << "E:\\data\\img\\" << idToTooth(modelToReslice[i]) << "\\" << toothId << ".txt";
			model[modelToReslice[i]].outputToImage(s.str());
			model[modelToReslice[i]].outputFeature(t.str(), u.str());
		}
	}
	return 0;
}
