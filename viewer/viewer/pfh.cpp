#include "pfh.h"

void pfh(vtkSmartPointer<vtkPolyData> &polydata, int i, int d, int r, vtkSmartPointer<vtkDoubleArray> &histogram, 
	vtkSmartPointer<vtkDoubleArray> &normalX, vtkSmartPointer<vtkDoubleArray> &normalY, vtkSmartPointer<vtkDoubleArray> &normalZ, double maxLen) {
		// Initialize
		vtkSmartPointer<vtkDoubleArray> f0 = vtkSmartPointer<vtkDoubleArray>::New();
		vtkSmartPointer<vtkDoubleArray> f1 = vtkSmartPointer<vtkDoubleArray>::New();
		vtkSmartPointer<vtkDoubleArray> f2 = vtkSmartPointer<vtkDoubleArray>::New();
		vtkSmartPointer<vtkDoubleArray> f3 = vtkSmartPointer<vtkDoubleArray>::New();

		double f0min = -1, f0max = 1, f1min = 0, f1max = 0, f2min = -1, f2max = 1, f3min = -PI / 2, f3max = PI / 2;

		f1max = 2 * maxLen * r;

		// Get the neighbors
		vtkSmartPointer<vtkIntArray> neighbors = vtkSmartPointer<vtkIntArray>::New();
		double a[3], b[3];

		queue<Pair> q;
		vector<int> vec;
		Pair originP;
		originP.id = i;
		originP.step = 0;
		q.push(originP);
		vec.push_back(i);
		while (true) {
			if (q.size() == 0) {
				break;
			}
			Pair t = q.front();
			q.pop();
			if (t.step > r)	{
				break;
			}
			// Find the neighbors of t
			vtkSmartPointer<vtkIdList> nearest = vtkSmartPointer<vtkIdList>::New();
			vtkSmartPointer<vtkIdList> neighborCells = vtkSmartPointer<vtkIdList>::New();
			polydata->GetPointCells(t.id, neighborCells);
			int numCells = neighborCells->GetNumberOfIds();
			for (int j = 0; j < numCells; ++j)	{
				vtkSmartPointer<vtkIdList> pointIdList = vtkSmartPointer<vtkIdList>::New();
				polydata->GetCellPoints(neighborCells->GetId(j), pointIdList);
				if (pointIdList->GetId(0) != t.id) {
					nearest->InsertNextId(pointIdList->GetId(0));
				} else {
					nearest->InsertNextId(pointIdList->GetId(1));
				}
			}
			// Delete the Duplicate
			vector<int> tmpVec;
			for (int j = 0; j < nearest->GetNumberOfIds(); ++j) {
				if (find(tmpVec.begin(), tmpVec.end(), nearest->GetId(j)) == tmpVec.end()) {
					tmpVec.push_back(nearest->GetId(j));
				}
			}
			nearest->Reset();
			for (int j = 0; j < tmpVec.size(); ++j) {
				nearest->InsertNextId(tmpVec[j]);
			}
			// Add the neighbors to the queue
			for (int j = 0; j < nearest->GetNumberOfIds(); ++j)	{
				if (find(vec.begin(), vec.end(), nearest->GetId(j)) != vec.end()) {
					continue;
				}
				Pair newP;
				newP.id = nearest->GetId(j);
				newP.step = t.step + 1;
				q.push(newP);
				vec.push_back(nearest->GetId(j));
			}
		}
		for (int j = 0; j < vec.size(); ++j) {
			neighbors->InsertNextValue(vec[j]);
		}

		// Initialize the histogram
		histogram->Reset();
		for (int j = 0; j < d * d * d * d; ++j) {
			histogram->InsertNextValue(0);
		}

		// Main Loop
		int kb = neighbors->GetNumberOfTuples();
		for (int j = 0; j < kb; ++j) {
			for (int k = 0; k < j; ++k)	{
				// Get source and target
				int s = -1, t = -1;
				int neiJ = neighbors->GetValue(j);
				int neiK = neighbors->GetValue(k);
				double nj[3];
				nj[0] = normalX->GetValue(neiJ);
				nj[1] = normalY->GetValue(neiJ);
				nj[2] = normalZ->GetValue(neiJ);
				double nk[3];
				nk[0] = normalX->GetValue(neiK);
				nk[1] = normalY->GetValue(neiK);
				nk[2] = normalZ->GetValue(neiK);
				double tmp[3];
				polydata->GetPoint(neiJ, a);
				polydata->GetPoint(neiK, b);
				vtkMath::Subtract(b, a, tmp);
				double product1 = vtkMath::Dot(nj, tmp);
				vtkMath::Subtract(a, b, tmp);
				double product2 = vtkMath::Dot(nk, tmp);
				if (product1 <= product2) {
					s = neiJ;
					t = neiK;
				} else {
					s = neiK;
					t = neiJ;
				}

				// Get the Darboux Frame
				double u[3], v[3], w[3];
				u[0] = normalX->GetValue(s);
				u[1] = normalY->GetValue(s);
				u[2] = normalZ->GetValue(s);
				double p[3];
				polydata->GetPoint(t, a);
				polydata->GetPoint(s, b);
				vtkMath::Subtract(a, b, p);
				vtkMath::Cross(p, u, v);
				vtkMath::MultiplyScalar(v, 1.0 / vtkMath::Norm(p));
				vtkMath::Cross(u, v, w);

				// Get fi

				double nt[3];
				nt[0] = normalX->GetValue(t);
				nt[1] = normalY->GetValue(t);
				nt[2] = normalZ->GetValue(t);
				double f0tmp = vtkMath::Dot(v, nt);
				double f1tmp = vtkMath::Norm(p);
				double f2tmp = vtkMath::Dot(u, p) / vtkMath::Norm(p);
				double f3tmp = atan2(vtkMath::Dot(w, nt), vtkMath::Dot(u, nt));
				f0->InsertNextValue(f0tmp);
				f1->InsertNextValue(f1tmp);
				f2->InsertNextValue(f2tmp);
				f3->InsertNextValue(f3tmp);
			}
		}

		// Get Histogram
		int size = kb * (kb - 1) / 2;
		for (int j = 0; j < f0->GetNumberOfTuples(); ++j) {
			int idx = 0;
			idx += (int)(((f0->GetValue(j) - f0min) * d) / (f0max - f0min));
			idx += (int)(((f1->GetValue(j) - f1min) * d) / (f1max - f1min)) * d;
			idx += (int)(((f2->GetValue(j) - f2min) * d) / (f2max - f2min)) * d * d;
			idx += (int)(((f3->GetValue(j) - f3min) * d) / (f3max - f3min)) * d * d * d;
			if (idx < 0 || idx >= d * d * d * d) {
				continue;
			}
			histogram->SetValue(idx, histogram->GetValue(idx) + 1.0);
		}

		// Normalize
		if (size != 0) {
			for (int j = 0; j < d * d * d * d; ++j) {
				histogram->SetValue(j, histogram->GetValue(j) / ((double)size));
			}
		}
}
