#include "StdAfx.h"
#include "DigitalFilter.h"
#include <math.h>

#define opt_be 0x00001			/* Bessel characteristic          */
#define opt_bu 0x00002			/* Butterworth characteristic     */
#define opt_ch 0x00004			/* Chebyshev characteristic       */
#define opt_re 0x00008			/* Resonator              */
#define opt_pi 0x00010			/* proportional-integral          */

#define opt_lp 0x00020			/* lowpass                */
#define opt_hp 0x00040			/* highpass               */
#define opt_bp 0x00080			/* bandpass               */
#define opt_bs 0x00100			/* bandstop               */
#define opt_ap 0x00200			/* allpass                */

#define opt_a  0x00400			/* alpha value            */
#define opt_l  0x00800			/* just list filter parameters    */
#define opt_o  0x01000			/* order of filter            */
#define opt_p  0x02000			/* specified poles only           */
#define opt_w  0x04000			/* don't pre-warp             */
#define opt_z  0x08000			/* use matched z-transform        */
#define opt_Z  0x10000			/* additional zero            */

struct pzrep {
	complex poles[MAXPZ], zeros[MAXPZ];
	int numpoles, numzeros;
};

static pzrep splane, zplane;
static int order;
static double raw_alpha1, raw_alpha2, raw_alphaz;
static complex dc_gain, fc_gain, hf_gain;
static uint options;
static double warped_alpha1, warped_alpha2, chebrip, qfactor;
static bool infq;
static uint polemask;
static double xcoeffs[MAXPZ + 1], ycoeffs[MAXPZ + 1];
static double akm[3][MAXPZ], bkm[3][MAXPZ];

static c_complex bessel_poles[] = {
	{-1.00000000000e+00, 0.00000000000e+00}, {-1.10160133059e+00, 6.36009824757e-01},
	{-1.32267579991e+00, 0.00000000000e+00}, {-1.04740916101e+00, 9.99264436281e-01},
	{-1.37006783055e+00, 4.10249717494e-01}, {-9.95208764350e-01, 1.25710573945e+00},
	{-1.50231627145e+00, 0.00000000000e+00}, {-1.38087732586e+00, 7.17909587627e-01},
	{-9.57676548563e-01, 1.47112432073e+00}, {-1.57149040362e+00, 3.20896374221e-01},
	{-1.38185809760e+00, 9.71471890712e-01}, {-9.30656522947e-01, 1.66186326894e+00},
	{-1.68436817927e+00, 0.00000000000e+00}, {-1.61203876622e+00, 5.89244506931e-01},
	{-1.37890321680e+00, 1.19156677780e+00}, {-9.09867780623e-01, 1.83645135304e+00},
	{-1.75740840040e+00, 2.72867575103e-01}, {-1.63693941813e+00, 8.22795625139e-01},
	{-1.37384121764e+00, 1.38835657588e+00}, {-8.92869718847e-01, 1.99832584364e+00},
	{-1.85660050123e+00, 0.00000000000e+00}, {-1.80717053496e+00, 5.12383730575e-01},
	{-1.65239648458e+00, 1.03138956698e+00}, {-1.36758830979e+00, 1.56773371224e+00},
	{-8.78399276161e-01, 2.14980052431e+00}, {-1.92761969145e+00, 2.41623471082e-01},
	{-1.84219624443e+00, 7.27257597722e-01}, {-1.66181024140e+00, 1.22110021857e+00},
	{-1.36069227838e+00, 1.73350574267e+00}, {-8.65756901707e-01, 2.29260483098e+00},
};

static void make_filter();
static void setdefaults();
static void compute_s(), choosepole(complex), prewarp(), normalize(), compute_z_blt();
static complex blt(complex);
static void compute_z_mzt();
static void compute_notch(), compute_apres();
static complex reflect(complex);
static void compute_bpres(), add_extra_zero();
static void expandpoly(), expand(complex[], int, complex[]), multin(complex, int, complex[]);

static complex eval(complex[], int, complex);
static double Xsqrt(double);

static void ToCascade();

void CDigitalFilter::InitIIR(EDFShape eShape, EDFBand eBand, int nOrder, double fSamplingFreq, double fFreq1, double fFreq2, double fRipple)
{
	options = 0;

	switch (eShape) {
	case DF_Butterworth:
		options |= opt_bu;
		break;
	case DF_Chebyshev:
		options |= opt_ch;
		break;
	case DF_Bessel:
		options |= opt_be;
		break;
	}

	switch (eBand) {
	case DF_LPF:
		options |= opt_lp;
		break;
	case DF_HPF:
		options |= opt_hp;
		break;
	case DF_BPF:
		options |= opt_bp;
		break;
	case DF_BRF:
		options |= opt_bs;
		break;
	}

	order = nOrder;
	raw_alpha1 = fFreq1 / fSamplingFreq;
	raw_alpha2 = fFreq2 / fSamplingFreq;
	chebrip = -fRipple;

	make_filter();

	ToCascade();
}

void CDigitalFilter::ResetIIR()
{
	memset(m_aDelay, 0, sizeof(m_aDelay));
}

void CDigitalFilter::ExecIIR(double *pData, int nSize)
{
	int i, j;
	double fGain = hypot2((options & opt_bs) ? dc_gain : fc_gain);

	for (i = 0; i < nSize; i++) {
		for (j = zplane.numpoles; j > 0; j--)
			m_aDelay[j] = m_aDelay[j - 1];

		double fa = pData[i] / fGain;
		for (j = 1; j <= zplane.numpoles; j++)
			fa += ycoeffs[zplane.numpoles - j] * m_aDelay[j];
		m_aDelay[0] = fa;

		double fb = 0;
		for (j = 0; j <= zplane.numzeros; j++)
			fb += xcoeffs[zplane.numzeros - j] * m_aDelay[j];

		pData[i] = fb;
	}
}

void CDigitalFilter::ExecIIR2(double *pData, int nSize)
{
	int i, j;

	for (i = 0; i < nSize; i++) {
		double fc = pData[i];
		for (j = 0; j < (zplane.numpoles + 1) / 2; j++) {
			m_aDelay2[2][j] = m_aDelay2[1][j];
			m_aDelay2[1][j] = m_aDelay2[0][j];

			double fa = fc;
			fa += akm[0][j] * m_aDelay2[1][j];
			fa += akm[1][j] * m_aDelay2[2][j];
			m_aDelay2[0][j] = fa;

			double fb = 0;
			fb += bkm[0][j] * m_aDelay2[0][j];
			fb += bkm[1][j] * m_aDelay2[1][j];
			fb += bkm[2][j] * m_aDelay2[2][j];

			fc = fb;
		}

		pData[i] = fc;
	}
}

static void error(...)
{
	CException *e = new CInvalidArgException;
	throw e;
}

complex evaluate(complex topco[], int nz, complex botco[], int np, complex z)
{
	return eval(topco, nz, z) / eval(botco, np, z);
}

static complex eval(complex coeffs[], int npz, complex z)
{
	complex sum = complex(0.0);
	for (int i = npz; i >= 0; i--)
		sum = (sum * z) + coeffs[i];
	return sum;
}

complex csqrt(complex x)
{
	double r = hypot2(x);
	complex z = complex(Xsqrt(0.5 * (r + x.re)),
						Xsqrt(0.5 * (r - x.re)));
	if (x.im < 0.0)
		z.im = -z.im;
	return z;
}

static double Xsqrt(double x)
{
	return (x >= 0.0) ? sqrt(x) : 0.0;
}

complex cexp(complex z)
{
	return exp(z.re) * expj(z.im);
}

complex expj(double theta)
{
	return complex(cos(theta), sin(theta));
}

complex operator *(complex z1, complex z2)
{
	return complex(z1.re * z2.re - z1.im * z2.im, z1.re * z2.im + z1.im * z2.re);
}

complex operator /(complex z1, complex z2)
{
	double mag = (z2.re * z2.re) + (z2.im * z2.im);
	return complex(((z1.re * z2.re) + (z1.im * z2.im)) / mag, ((z1.im * z2.re) - (z1.re * z2.im)) / mag);
}


void make_filter()
{
	setdefaults();
	if (options & opt_re) {
		if (options & opt_bp)
			compute_bpres();	/* bandpass resonator     */
		if (options & opt_bs)
			compute_notch();	/* bandstop resonator (notch) */
		if (options & opt_ap)
			compute_apres();	/* allpass resonator      */
	} else {
		if (options & opt_pi) {
			prewarp();
			splane.poles[0] = 0.0;
			splane.zeros[0] = -TWOPI * warped_alpha1;
			splane.numpoles = splane.numzeros = 1;
		} else {
			compute_s();
			prewarp();
			normalize();
		}
		if (options & opt_z)
			compute_z_mzt();
		else
			compute_z_blt();
	}
	if (options & opt_Z)
		add_extra_zero();
	expandpoly();
}

static void setdefaults()
{
	unless(options & opt_p) polemask = ~0;	/* use all poles */
	unless(options & (opt_bp | opt_bs)) raw_alpha2 = raw_alpha1;
}

static void compute_s()
{								/* compute S-plane poles for prototype LP filter */
	splane.numpoles = 0;
	if (options & opt_be) {		/* Bessel filter */
		int p = (order * order) / 4;	/* ptr into table */
		if (order & 1)
			choosepole(bessel_poles[p++]);
		for (int i = 0; i < order / 2; i++) {
			choosepole(bessel_poles[p]);
			choosepole(cconj(bessel_poles[p]));
			p++;
		}
	}
	if (options & (opt_bu | opt_ch)) {	/* Butterworth filter */
		for (int i = 0; i < 2 * order; i++) {
			double theta = (order & 1) ? (i * M_PI) / order : ((i + 0.5) * M_PI) / order;
			choosepole(expj(theta));
		}
	}
	if (options & opt_ch) {		/* modify for Chebyshev (p. 136 DeFatta et al.) */
		if (chebrip >= 0.0) {
			error("Chebyshev ripple is %g dB; must be .lt. 0.0", chebrip);
		}
		double rip = pow(10.0, -chebrip / 10.0);
		double eps = sqrt(rip - 1.0);
		double y = asinh(1.0 / eps) / (double) order;
		if (y <= 0.0) {
			error("Chebyshev y=%g; must be .gt. 0.0", y);
		}
		for (int i = 0; i < splane.numpoles; i++) {
			splane.poles[i].re *= sinh(y);
			splane.poles[i].im *= cosh(y);
		}
	}
}

static void choosepole(complex z)
{
	if (z.re < 0.0) {
		if (polemask & 1)
			splane.poles[splane.numpoles++] = z;
		polemask >>= 1;
	}
}

static void prewarp()
{								/* for bilinear transform, perform pre-warp on alpha values */
	if (options & (opt_w | opt_z)) {
		warped_alpha1 = raw_alpha1;
		warped_alpha2 = raw_alpha2;
	} else {
		warped_alpha1 = tan(M_PI * raw_alpha1) / M_PI;
		warped_alpha2 = tan(M_PI * raw_alpha2) / M_PI;
	}
}

static void normalize()
{								/* called for trad, not for -Re or -Pi */
	double w1 = TWOPI * warped_alpha1;
	double w2 = TWOPI * warped_alpha2;
	/* transform prototype into appropriate filter type (lp/hp/bp/bs) */
	switch (options & (opt_lp | opt_hp | opt_bp | opt_bs)) {
	case opt_lp:
		{
			for (int i = 0; i < splane.numpoles; i++)
				splane.poles[i] = splane.poles[i] * w1;
			splane.numzeros = 0;
			break;
		}

	case opt_hp:
		{
			int i;
			for (i = 0; i < splane.numpoles; i++)
				splane.poles[i] = w1 / splane.poles[i];
			for (i = 0; i < splane.numpoles; i++)
				splane.zeros[i] = 0.0;	/* also N zeros at (0,0) */
			splane.numzeros = splane.numpoles;
			break;
		}

	case opt_bp:
		{
			double w0 = sqrt(w1 * w2), bw = w2 - w1;
			int i;
			for (i = 0; i < splane.numpoles; i++) {
				complex hba = 0.5 * (splane.poles[i] * bw);
				complex temp = csqrt(1.0 - sqr(w0 / hba));
				splane.poles[i] = hba * (1.0 + temp);
				splane.poles[splane.numpoles + i] = hba * (1.0 - temp);
			}
			for (i = 0; i < splane.numpoles; i++)
				splane.zeros[i] = 0.0;	/* also N zeros at (0,0) */
			splane.numzeros = splane.numpoles;
			splane.numpoles *= 2;
			break;
		}

	case opt_bs:
		{
			double w0 = sqrt(w1 * w2), bw = w2 - w1;
			int i;
			for (i = 0; i < splane.numpoles; i++) {
				complex hba = 0.5 * (bw / splane.poles[i]);
				complex temp = csqrt(1.0 - sqr(w0 / hba));
				splane.poles[i] = hba * (1.0 + temp);
				splane.poles[splane.numpoles + i] = hba * (1.0 - temp);
			}
			for (i = 0; i < splane.numpoles; i++) {	/* also 2N zeros at (0, +-w0) */
				splane.zeros[i] = complex(0.0, +w0);
				splane.zeros[splane.numpoles + i] = complex(0.0, -w0);
			}
			splane.numpoles *= 2;
			splane.numzeros = splane.numpoles;
			break;
		}
	}
}

static void compute_z_blt()
{								/* given S-plane poles & zeros, compute Z-plane poles & zeros, by bilinear transform */
	int i;
	zplane.numpoles = splane.numpoles;
	zplane.numzeros = splane.numzeros;
	for (i = 0; i < zplane.numpoles; i++)
		zplane.poles[i] = blt(splane.poles[i]);
	for (i = 0; i < zplane.numzeros; i++)
		zplane.zeros[i] = blt(splane.zeros[i]);
	while (zplane.numzeros < zplane.numpoles)
		zplane.zeros[zplane.numzeros++] = -1.0;
}

static complex blt(complex pz)
{
	return (2.0 + pz) / (2.0 - pz);
}

static void compute_z_mzt()
{								/* given S-plane poles & zeros, compute Z-plane poles & zeros, by matched z-transform */
	int i;
	zplane.numpoles = splane.numpoles;
	zplane.numzeros = splane.numzeros;
	for (i = 0; i < zplane.numpoles; i++)
		zplane.poles[i] = cexp(splane.poles[i]);
	for (i = 0; i < zplane.numzeros; i++)
		zplane.zeros[i] = cexp(splane.zeros[i]);
}

static void compute_notch()
{								/* compute Z-plane pole & zero positions for bandstop resonator (notch filter) */
	compute_bpres();			/* iterate to place poles */
	double theta = TWOPI * raw_alpha1;
	complex zz = expj(theta);	/* place zeros exactly */
	zplane.zeros[0] = zz;
	zplane.zeros[1] = cconj(zz);
}

static void compute_apres()
{								/* compute Z-plane pole & zero positions for allpass resonator */
	compute_bpres();			/* iterate to place poles */
	zplane.zeros[0] = reflect(zplane.poles[0]);
	zplane.zeros[1] = reflect(zplane.poles[1]);
}

static complex reflect(complex z)
{
	double r = hypot2(z);
	return z / sqr(r);
}

static void compute_bpres()
{								/* compute Z-plane pole & zero positions for bandpass resonator */
	zplane.numpoles = zplane.numzeros = 2;
	zplane.zeros[0] = 1.0;
	zplane.zeros[1] = -1.0;
	double theta = TWOPI * raw_alpha1;	/* where we want the peak to be */
	if (infq) {					/* oscillator */
		complex zp = expj(theta);
		zplane.poles[0] = zp;
		zplane.poles[1] = cconj(zp);
	} else {					/* must iterate to find exact pole positions */
		complex topcoeffs[MAXPZ + 1];
		expand(zplane.zeros, zplane.numzeros, topcoeffs);
		double r = exp(-theta / (2.0 * qfactor));
		double thm = theta, th1 = 0.0, th2 = M_PI;
		bool cvg = false;
		for (int i = 0; i < 50 && !cvg; i++) {
			complex zp = r * expj(thm);
			zplane.poles[0] = zp;
			zplane.poles[1] = cconj(zp);
			complex botcoeffs[MAXPZ + 1];
			expand(zplane.poles, zplane.numpoles, botcoeffs);
			complex g = evaluate(topcoeffs, zplane.numzeros, botcoeffs, zplane.numpoles, expj(theta));
			double phi = g.im / g.re;	/* approx to atan2 */
			if (phi > 0.0)
				th2 = thm;
			else
				th1 = thm;
			if (fabs(phi) < EPS)
				cvg = true;
			thm = 0.5 * (th1 + th2);
		}
		unless(cvg) error("warning: failed to converge");
	}
}

static void add_extra_zero()
{
	if (zplane.numzeros + 2 > MAXPZ) {
		error("too many zeros; can't do -Z");
	}
	double theta = TWOPI * raw_alphaz;
	complex zz = expj(theta);
	zplane.zeros[zplane.numzeros++] = zz;
	zplane.zeros[zplane.numzeros++] = cconj(zz);
	while (zplane.numpoles < zplane.numzeros)
		zplane.poles[zplane.numpoles++] = 0.0;	/* ensure causality */
}

static void expandpoly()
{								/* given Z-plane poles & zeros, compute top & bot polynomials in Z, and then recurrence relation */
	complex topcoeffs[MAXPZ + 1], botcoeffs[MAXPZ + 1];
	int i;
	expand(zplane.zeros, zplane.numzeros, topcoeffs);
	expand(zplane.poles, zplane.numpoles, botcoeffs);
	dc_gain = evaluate(topcoeffs, zplane.numzeros, botcoeffs, zplane.numpoles, 1.0);
	double theta = TWOPI * 0.5 * (raw_alpha1 + raw_alpha2);	/* "jwT" for centre freq. */
	fc_gain = evaluate(topcoeffs, zplane.numzeros, botcoeffs, zplane.numpoles, expj(theta));
	hf_gain = evaluate(topcoeffs, zplane.numzeros, botcoeffs, zplane.numpoles, -1.0);
	for (i = 0; i <= zplane.numzeros; i++)
		xcoeffs[i] = +(topcoeffs[i].re / botcoeffs[zplane.numpoles].re);
	for (i = 0; i <= zplane.numpoles; i++)
		ycoeffs[i] = -(botcoeffs[i].re / botcoeffs[zplane.numpoles].re);
}

static void expand(complex pz[], int npz, complex coeffs[])
{								/* compute product of poles or zeros as a polynomial of z */
	int i;
	coeffs[0] = 1.0;
	for (i = 0; i < npz; i++)
		coeffs[i + 1] = 0.0;
	for (i = 0; i < npz; i++)
		multin(pz[i], npz, coeffs);
	/* check computed coeffs of z^k are all real */
	for (i = 0; i < npz + 1; i++) {
		if (fabs(coeffs[i].im) > EPS) {
			error("coeff of z^%d is not real; poles/zeros are not complex conjugates", i);
		}
	}
}

static void multin(complex w, int npz, complex coeffs[])
{								/* multiply factor (z-w) into coeffs */
	complex nw = -w;
	for (int i = npz; i >= 1; i--)
		coeffs[i] = (nw * coeffs[i]) + coeffs[i - 1];
	coeffs[0] = nw * coeffs[0];
}

static void ToCascade()
{
	int m, pos;
	int i, j;
	double min, tmp_a0, tmp_a1, tmp_b0, tmp_b1, tmp_b2;
	BOOL bFlag[MAXPZ];

	memset(bFlag, 0, sizeof(bFlag));

	m = 0;
	for (i = 0; i < zplane.numpoles; i++) {
		if (!bFlag[i]) {
			for (j = i + 1; j < zplane.numpoles; j++) {
				if (fabs(zplane.poles[i].re - zplane.poles[j].re) < 1.0E-14 && fabs(zplane.poles[i].im + zplane.poles[j].im) < 1.0E-14)
					break;
			}

			if (fabs(zplane.poles[j].im) > 1.0E-16 && j < zplane.numpoles)    // 複素極のステージ
			{
				akm[0][m] = (2.0 * zplane.poles[i]).re;              // a1m
				akm[1][m] = -(zplane.poles[i].re * zplane.poles[i].re + zplane.poles[i].im * zplane.poles[i].im);                 // a2m
				bkm[0][m] = 1.0;                            // b0m
				bkm[1][m] = -(zplane.zeros[i] + zplane.zeros[j]).re;   // b1m
				bkm[2][m] = (zplane.zeros[i] * zplane.zeros[j]).re;      // b2m
				bFlag[i] = TRUE;
				bFlag[j] = TRUE;
			}
			else                                // 実極のステージ
			{
				if (j == zplane.numpoles)               // 奇数次の場合
				{
					akm[0][m] = zplane.poles[i].re;                  // a1m
					akm[1][m] = 0.0;                            // a2m = 0
					bkm[0][m] = 1.0;                            // b0m
					bkm[1][m] = -zplane.zeros[i].re;                // b1m
					bkm[2][m] = 0.0;                            // b2m = 0
					bFlag[i] = TRUE;
				}
				else                            // 偶数次の場合
				{
					akm[0][m] = (zplane.poles[i] + zplane.poles[j]).re;      // a1m
					akm[1][m] = -(zplane.poles[i] * zplane.poles[j]).re;       // a2m
					bkm[0][m] = 1.0;                            // b0m
					bkm[1][m] = -(zplane.zeros[i] + zplane.zeros[j]).re;   // b1m
					bkm[2][m] = (zplane.zeros[i] * zplane.zeros[j]).re;      // b2m
					bFlag[i] = TRUE;
					bFlag[j] = TRUE;
				}

			}
			m++;
		}
	}

	for (int i = 0; i < (zplane.numpoles + 1) / 2 - 1; i++)
	{
		pos = i;
		min = fabs(akm[1][i]);
		for (int j = i; j < (zplane.numpoles + 1) / 2; j++)
		{
			if (fabs(akm[1][j]) <= min)
			{
				pos = j;
				min = fabs(akm[1][j]);
				tmp_a0 = akm[0][j];
				tmp_a1 = akm[1][j];
				tmp_b0 = bkm[0][j];
				tmp_b1 = bkm[1][j];
				tmp_b2 = bkm[2][j];
			}
		}
		akm[0][pos] = akm[0][i];
		akm[1][pos] = akm[1][i];
		bkm[0][pos] = bkm[0][i];
		bkm[1][pos] = bkm[1][i];
		bkm[2][pos] = bkm[2][i];
		akm[0][i] = tmp_a0;
		akm[1][i] = tmp_a1;
		bkm[0][i] = tmp_b0;
		bkm[1][i] = tmp_b1;
		bkm[2][i] = tmp_b2;
	}

	for (int j = 0; j < (zplane.numpoles + 1) / 2; j++) {
		double a, an, bn;
		if (options & opt_bp) {
//			a = -asin(cgam);
//			an = cos(a) * (-akm[1][j] - 1);
//			bn = cos(a) * (bkm[2][j] - bkm[0][j]);
		} else {
			if (options & opt_hp)
				a = -1.0;
			else
				a = 1.0;
			an = 1 - a * akm[0][j] - akm[1][j];
			bn = 1 + a * bkm[1][j] + bkm[2][j];
		}
		double cn = an / bn;
		bkm[0][j] *= cn;
		bkm[1][j] *= cn;
		bkm[2][j] *= cn;
	}
}
