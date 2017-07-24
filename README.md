# ToothFeaturePoints
An algorithm to get feature points from a mesh model

## Usage

### Viewer
viewer.exe

settings in Utility.h

### Generate Feature
feature.exe input_folder_path output_folder_path argument [tooth_id fp_file_path]

argument:

- 0: output global feature
- 1: output partial feature
- 2: output the new Coordinate for the feature point in pca system

### Extract Position information
python extract.py

Put executable file feature.exe in execute/

Set the arguments manually in extract.py

### Train data
python svr.py

files:

- train_in.txt & train_out.txt : train set
- data.txt : data set
- predict.txt : output the predict

## Compilation Environment

### Viewer
- Visual Studio 2010
- Qt 4.8.0
- VTK 6.1.0

### Feature
- Visual Studio 2010
- VTK 6.1.0

### Extract
- Python 3.6.2

### Train
- Python 3.6.2
- NumPy 1.13.1
- SciPy 0.19.1
- scikit-learn 0.18.2
