#pragma once

extern double Regress1(const double *pData, int nData, double start, double step);
extern void Regress2(const double *pData, int nData, double start, double step, double *reg0, double *reg1);
extern void Regress2x(const double *pData0, const double *pData1, int nData, double *reg0, double *reg1);
