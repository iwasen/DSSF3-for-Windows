#pragma once

#define unless(x)   if(!(x))
#define until(x)    while(!(x))

#define TWOPI	    (2.0 * M_PI)
#define EPS	    1e-10
#define MAXORDER    10
#define MAXPZ	    (MAXORDER * 2 + 2)
typedef unsigned int uint;

enum EDFShape {
	DF_Butterworth,
	DF_Chebyshev,
	DF_Bessel
};

enum EDFBand {
	DF_LPF,
	DF_HPF,
	DF_BPF,
	DF_BRF
};

class CDigitalFilter {
public:
	void InitIIR(EDFShape eShape, EDFBand eBand, int nOrder, double fSamplingFreq, double fFreq1, double fFreq2, double fRipple);
	void ResetIIR();
	void ExecIIR(double *pData, int nSize);
	void ExecIIR2(double *pData, int nSize);

protected:
	double m_aDelay[MAXPZ];
	double m_aDelay2[3][MAXPZ / 2 + 1];
};

struct c_complex {
	double re, im;
};

struct complex {
	double re, im;
	 complex(double r, double i = 0.0) {
		re = r;
		im = i;
	} complex() {
	}							/* uninitialized complex */
	complex(c_complex z) {
		re = z.re;
		im = z.im;
	}							/* init from denotation */
};

extern complex csqrt(complex), cexp(complex), expj(double);	/* from complex.C */
extern complex evaluate(complex[], int, complex[], int, complex);	/* from complex.C */

inline double hypot2(complex z)
{
	return ::_hypot(z.im, z.re);
}

inline double atan2(complex z)
{
	return ::atan2(z.im, z.re);
}

inline complex cconj(complex z)
{
	z.im = -z.im;
	return z;
}

inline complex operator *(double a, complex z)
{
	z.re *= a;
	z.im *= a;
	return z;
}

inline complex operator /(complex z, double a)
{
	z.re /= a;
	z.im /= a;
	return z;
}

inline void operator /=(complex & z, double a)
{
	z = z / a;
}

extern complex operator *(complex, complex);
extern complex operator /(complex, complex);

inline complex operator +(complex z1, complex z2)
{
	z1.re += z2.re;
	z1.im += z2.im;
	return z1;
}

inline complex operator -(complex z1, complex z2)
{
	z1.re -= z2.re;
	z1.im -= z2.im;
	return z1;
}

inline complex operator -(complex z)
{
	return 0.0 - z;
}

inline bool operator ==(complex z1, complex z2)
{
	return (z1.re == z2.re) && (z1.im == z2.im);
}

inline complex sqr(complex z)
{
	return z * z;
}

inline double sqr(double x)
{
	return x * x;
}

inline bool onebit(uint m)
{
	return (m != 0) && ((m & m - 1) == 0);
}

inline double asinh(double x)
{
	return log(x + sqrt(1.0 + sqr(x)));
}

inline double fix(double x)
{
	return (x >= 0.0) ? floor(0.5 + x) : -floor(0.5 - x);
}
