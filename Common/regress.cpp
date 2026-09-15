// Regress.cpp : インプリメンテーション ファイル
//

/***********************************************************
	regress.cpp -- 回帰分析
***********************************************************/

#include "stdafx.h"
#include "regress.h"

#ifndef SCALAR
	#define SCALAR double
#endif
#define EPS   1e-6    /* 許容誤差 */

typedef SCALAR *vector, **matrix;

static vector new_vector(int n)
{
	return new SCALAR[n];
}

static matrix new_matrix(int nrow, int ncol)
{
	matrix a = new SCALAR *[nrow + 1];
	if (a == NULL) return NULL;  /* 記憶領域不足 */
	for (int i = 0; i < nrow; i++) {
		a[i] = new SCALAR[ncol];
		if (a[i] == NULL) {
			while (--i >= 0) delete [] a[i];
			delete [] a;  return NULL;  /* 記憶領域不足 */
		}
	}
	a[nrow] = NULL;  /* 行の数を自動判断するための工夫 */
	return a;
}

static void free_vector(vector v)
{
	delete [] v;
}

static void free_matrix(matrix a)
{
	matrix b = a;
	while (*b != NULL) delete [] *b++;
	delete [] a;
}

static double innerproduct(int n, vector u, vector v)
{
	int i;

	double s = 0;
	int n5 = n % 5;
	for (i = 0; i < n5; i++) s += u[i]*v[i];
	for (i = n5; i < n; i += 5)
		s += u[i]*v[i] + u[i+1]*v[i+1] + u[i+2]*v[i+2] + u[i+3]*v[i+3] + u[i+4]*v[i+4];
	return s;
}

static int lsq(int n, int m, matrix x, vector b, int *col, vector normsq)  /* 最小2乗法 */
{
	int i, j, k, r;
	double s, t, u;
	vector v, w;

	for (j = 0; j < m; j++)
		normsq[j] = innerproduct(n, x[j], x[j]);
	r = 0;
	for (k = 0; k < m; k++) {
		if (normsq[k] == 0) continue;
		v = x[k];  u = innerproduct(n - r, &v[r], &v[r]);
		if (u / normsq[k] < EPS * EPS) continue;
		x[r] = v;  col[r] = k;
		u = sqrt(u);  if (v[r] < 0) u = -u;
		v[r] += u;  t = 1 / (v[r] * u);
		for (j = k + 1; j <= m; j++) {
			w = x[j];
			s = t * innerproduct(n - r, &v[r], &w[r]);
			for (i = r; i < n; i++) w[i] -= s * v[i];
		}
		v[r] = -u;
		r++;
	}
	for (j = r - 1; j >= 0; j--) {
		s = x[m][j];
		for (i = j + 1; i < r; i++) s -= x[i][j] * b[i];
		b[j] = s / x[j][j];
	}
	return r;  /* rank */
}

double Regress1(const double *pData, int nData, double start, double step)
{
	double b[1], normsq[1];
	int col[1];

	matrix x = new_matrix(2, nData);

	double t = start;
	for (int i = 0; i < nData; i++) {
		x[0][i] = t;
		x[1][i] = pData[i];
		t += step;
	}

	lsq(nData, 1, x, b, col, normsq);

	free_matrix(x);

	return b[0];
}
/*
double Regress1x(const double *pData0, const double *pData1, int nData)
{
	matrix x;
	double b[1], normsq[1];
	int col[1];
	int i;

	x = new_matrix(2, nData);

	for (i = 0; i < nData; i++) {
		x[0][i] = pData0[i];
		x[1][i] = pData1[i];
	}

	lsq(nData, 1, x, b, col, normsq);

	free_matrix(x);

	return b[0];
}
*/
void Regress2(const double *pData, int nData, double start, double step, double *reg0, double *reg1)
{
	double b[2], normsq[2];
	int col[2];

	matrix x = new_matrix(3, nData);

	double t = start;
	for (int i = 0; i < nData; i++) {
		x[0][i] = 1;
		x[1][i] = t;
		x[2][i] = pData[i];
		t += step;
	}

	lsq(nData, 2, x, b, col, normsq);

	free_matrix(x);

	if (reg0 != NULL)
		*reg0 = b[0];

	if (reg1 != NULL)
		*reg1 = b[1];
}

void Regress2x(const double *pData0, const double *pData1, int nData, double *reg0, double *reg1)
{
	double b[2], normsq[2];
	int col[2];

	matrix x = new_matrix(3, nData);

	for (int i = 0; i < nData; i++) {
		x[0][i] = 1;
		x[1][i] = pData0[i];
		x[2][i] = pData1[i];
	}

	lsq(nData, 2, x, b, col, normsq);

	free_matrix(x);

	*reg0 = b[0];
	*reg1 = b[1];
}
