#pragma once

class CSpline {
public:
	CSpline();
	~CSpline();

	void MakeTable(const double x[], const double y[], int n);
	double Spline(double t);

protected:
	double *m_x, *m_y, *m_z;
	int m_n;

	void FreeBuffers();
};
