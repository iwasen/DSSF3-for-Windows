#pragma once

class CRFFT
{
public:
	CRFFT();
	~CRFFT();

	void fft(int n, double *fp);
	void ifft(int n, double *fp);

private:
	int last_n;		// ‘O‰ñŒÄo‚µ‚Ìn
	double *a;
	double *w;
	int *ip;

	void fft_sub(int n, double *fp, int isgn);
	void alloc_work(int n);

//	static void cdft(int n, int isgn, double *a, int *ip, double *w);
	static void rdft(int n, int isgn, double *a, int *ip, double *w);
	static void makewt(int nw,	int *ip, double *w);
	static void makect(int nc, int *ip, double *c);
	static void bitrv2(int n, int *ip, double *a);
	static void bitrv2conj(int n, int *ip,	double *a);
	static void cftfsub(int n,	double *a, const double *w);
	static void cftbsub(int n,	double *a, const double *w);
	static void cft1st(int n, double *a, const double *w);
	static void cftmdl(int n, int l, double *a, const double *w);
	static void rftfsub(int n, double *a, int nc, const double *c);
	static void rftbsub(int n, double *a, int nc, const double *c);
};
