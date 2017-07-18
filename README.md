# ToothFeaturePoints
An algorithm to get feature points from a mesh model

## Usage

### Generate Feature
feature.exe input_folder_path output_folder_path argument
argument:
- 0: output global feature
- 1: output partial feature

### Train data
python svr.py
files:
- train_in.txt & train_out.txt : train set
- data.txt : data set
- predict.txt : output the predict

## Compilation

### Feature
- Visual Studio 2010
- VTK 6.1.0

### Train
- Python 3.6.2
- NumPy 1.13.1
- SciPy 0.19.1
- scikit-learn 0.18.2
