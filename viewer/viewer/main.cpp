#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)

#include "viewer.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	viewer w;
	w.show();
	return a.exec();
}
