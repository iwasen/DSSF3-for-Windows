//*****************************************************************************************************
//  1. ファイル名
//		DigitalFilter.cpp
//----------------------------------------------------------------------------------------------------
//  2. 機能
//		デジタルフィルタクラスの実装
//----------------------------------------------------------------------------------------------------
//  3. 備考
//----------------------------------------------------------------------------------------------------
//  4. 履歴
//		2007.12.17 S.Aizawa 新規作成
//*****************************************************************************************************

#include "StdAfx.h"
#include "DigitalFilter.h"

// ベッセルフィルタ用定数
const complex<double> CDigitalFilter::m_aBesselTable[] = {
	complex<double>(-1.00000000000e+00, 0.00000000000e+00), complex<double>(-1.10160133059e+00, 6.36009824757e-01),
	complex<double>(-1.32267579991e+00, 0.00000000000e+00), complex<double>(-1.04740916101e+00, 9.99264436281e-01),
	complex<double>(-1.37006783055e+00, 4.10249717494e-01), complex<double>(-9.95208764350e-01, 1.25710573945e+00),
	complex<double>(-1.50231627145e+00, 0.00000000000e+00), complex<double>(-1.38087732586e+00, 7.17909587627e-01),
	complex<double>(-9.57676548563e-01, 1.47112432073e+00), complex<double>(-1.57149040362e+00, 3.20896374221e-01),
	complex<double>(-1.38185809760e+00, 9.71471890712e-01), complex<double>(-9.30656522947e-01, 1.66186326894e+00),
	complex<double>(-1.68436817927e+00, 0.00000000000e+00), complex<double>(-1.61203876622e+00, 5.89244506931e-01),
	complex<double>(-1.37890321680e+00, 1.19156677780e+00), complex<double>(-9.09867780623e-01, 1.83645135304e+00),
	complex<double>(-1.75740840040e+00, 2.72867575103e-01), complex<double>(-1.63693941813e+00, 8.22795625139e-01),
	complex<double>(-1.37384121764e+00, 1.38835657588e+00), complex<double>(-8.92869718847e-01, 1.99832584364e+00),
	complex<double>(-1.85660050123e+00, 0.00000000000e+00), complex<double>(-1.80717053496e+00, 5.12383730575e-01),
	complex<double>(-1.65239648458e+00, 1.03138956698e+00), complex<double>(-1.36758830979e+00, 1.56773371224e+00),
	complex<double>(-8.78399276161e-01, 2.14980052431e+00), complex<double>(-1.92761969145e+00, 2.41623471082e-01),
	complex<double>(-1.84219624443e+00, 7.27257597722e-01), complex<double>(-1.66181024140e+00, 1.22110021857e+00),
	complex<double>(-1.36069227838e+00, 1.73350574267e+00), complex<double>(-8.65756901707e-01, 2.29260483098e+00),
};

//*****************************************************************************************************
//  1. 関数名
//		CDigitalFilter::InitIIR
//----------------------------------------------------------------------------------------------------
//  2. 機能
//		IIRフィルタの初期化
//----------------------------------------------------------------------------------------------------
//  3. パラメータ説明
//		EDFShape	eShape				[I] フィルタの特性（Butterworth,Chebyshev,Bessel）
//		EDFBand		eBand				[I] フィルタの種類（LPF,HPF,BPF,BRF）
//		int			nOrder				[I] フィルタの次数（2～20）
//		double		fSamplingFreq		[I] サンプリング周波数
//		double		fFreq1				[I] 遮断周波数１
//		double		fFreq2				[I] 遮断周波数２
//		double		fRipple				[I] Chebyshevフィルタの通過域リプル
//----------------------------------------------------------------------------------------------------
//  4. 戻り値
//		無し
//*****************************************************************************************************
void CDigitalFilter::InitIIR(EDFShape eShape, EDFBand eBand, int nOrder, double fSamplingFreq, double fFreq1, double fFreq2, double fRipple)
{
	m_eShape = eShape;
	m_eBand = eBand;
	m_fSamplingFreq = fSamplingFreq;

	switch (eBand) {
	case EDF_LPF:
	case EDF_HPF:
		fFreq2 = 0;
		break;
	case EDF_BPF:
	case EDF_BSF:
		if (fFreq1 < fFreq2)
			swap(fFreq1, fFreq2);
		break;
	}

	double fScale, fDelta;
	if (m_eShape == EDF_Chebyshev) {
		fScale = (nOrder & 1) == 0 ? pow(10.0, fRipple / 20.0) : 1.0;
		fDelta = pow(10.0, fRipple / 20.0);
	} else {
		fScale = 1.0;
		fDelta = 0;
	}

	double fa, ang;
	if (m_eBand == EDF_HPF) {
		ang = fFreq1 * M_PI / fSamplingFreq;
		fa = fSamplingFreq / 2.0;
	} else {
		ang = (fFreq1 - fFreq2) * M_PI / fSamplingFreq;
		fa = fFreq1;
	}

	const double wc = tan(ang);
	const double cgam = cos(M_PI * (fa + fFreq2) / fSamplingFreq) / cos(ang);
	fa = 2.0 * M_PI * fFreq1 / fSamplingFreq;
	const double cbp = (cgam - cos(fa)) / sin(fa);

	int np, nz;
	Plane(nOrder, fDelta, np, nz);
	AddZero(Bilinear(np, nz, cgam, tan(ang), cbp, wc));

	ToCascade(cgam, fScale);

	memset(m_aDelay, 0, sizeof(m_aDelay));
}

//*****************************************************************************************************
//  1. 関数名
//		CDigitalFilter::ExecIIR
//----------------------------------------------------------------------------------------------------
//  2. 機能
//		IIRフィルタの実行
//----------------------------------------------------------------------------------------------------
//  3. パラメータ説明
//		double		pDataIn				[I] 入力データ
//		double		pDataOut			[O] 出力データ
//		int			nSize				[I] データ個数
//----------------------------------------------------------------------------------------------------
//  4. 戻り値
//		無し
//*****************************************************************************************************
void CDigitalFilter::ExecIIR(const double *pDataIn, double *pDataOut, int nSize)
{
	for (int i = 0; i < nSize; i++)
		*pDataOut++ = ExecIIR(*pDataIn++);
}

double CDigitalFilter::ExecIIR(double fData)
{
	for (int i = 0; i < (m_nZOrder + 1) / 2; i++) {
		m_aDelay[2][i] = m_aDelay[1][i];
		m_aDelay[1][i] = m_aDelay[0][i];

		double fa = fData;
		fa += akm[0][i] * m_aDelay[1][i];
		fa += akm[1][i] * m_aDelay[2][i];
		m_aDelay[0][i] = fa;

		double fb = 0;
		fb += bkm[0][i] * m_aDelay[0][i];
		fb += bkm[1][i] * m_aDelay[1][i];
		fb += bkm[2][i] * m_aDelay[2][i];

		fData = fb;
	}

	return fData;
}

double CDigitalFilter::GetLevel(double fFreq) const
{
	complex<double> cH = 1.0;
	fFreq = fFreq / m_fSamplingFreq;

	for (int i = 0; i < (m_nZOrder + 1) / 2; i++) {
		complex<double> cZ1 = exp(complex<double>(0, -2 * M_PI * fFreq));
		complex<double> cZ2 = exp(complex<double>(0, -4 * M_PI * fFreq));
		cH *= (bkm[0][i] + bkm[1][i] * cZ1 + bkm[2][i] * cZ2) / (1.0 - akm[0][i] * cZ1 - akm[1][i] * cZ2);
	}

	return abs(cH);
}

// ButterworthとChebyshevの共通処理
void CDigitalFilter::ButChe(int order, int np, double a, double b)
{
	double theta = (order & 1) ? 0.0 : M_PI / (2.0 * order);
	for (int j = 0; j < np; j++) {
		s_pz[j] = complex<double>(-a * cos(theta), b * sin(theta));
		theta = theta + M_PI / order;
	}
}

// Butterworthフィルタの極を計算
void CDigitalFilter::Butterworth(int order, int np)
{
	ButChe(order, np, 1.0, 1.0);
}

// Chebyshevフィルタの極を計算
void CDigitalFilter::Chebyshev(int order, double delta, int np)
{
	double x = sqrt(delta * delta - 1.0);
	if (m_eShape == EDF_Chebyshev)
		x = 1.0 / x;

	const double alpha = log(x + sqrt(x * x + 1.0)) / order;

	ButChe(order, np, sinh(alpha), cosh(alpha));
}

// Besselフィルタの極を計算
void CDigitalFilter::Bessel(int order, int np)
{
	int p = (order * order) / 4;
	for (int i = 0; i < np; i++)
		s_pz[i] = m_aBesselTable[p++];
}

// S平面上でフィルタを設計
void CDigitalFilter::Plane(int order, double delta, int &np, int &nz)
{
	for (int j = 0; j < A_SIZE; j++)
		s_pz[j] = 0.0;

	np = (order + 1) / 2;
	switch (m_eShape) {
	case EDF_Butterworth:
		Butterworth(order, np);
		break;
	case EDF_Chebyshev:
		Chebyshev(order, delta, np);
		break;
	case EDF_Bessel:
		Bessel(order, np);
		break;
	}

	nz = 0;

	if (m_eBand == EDF_HPF || m_eBand == EDF_BSF) {
		for (int j = 0; j < np; j++)
			s_pz[j] = 1.0 / s_pz[j];

		nz = np;

		if (m_eBand == EDF_BSF)
			nz = nz + order / 2;

		for (int j = 0; j < nz; j++)
			s_pz[j + np] = 0.0;
	}
}

// 双一次ｚ変換
int CDigitalFilter::Bilinear(int np, int nz, double cgam, double c, double cbp, double wc)
{
	complex<double> r, root, cx, ca, cb, b4ac;
	double xc;
	int nc, j, jt;

	xc = wc;
	for (int i = 0; i < A_SIZE; i++)
		z_pz[i] = 0.0;
	nc = np;
	jt = -1;
	j = 0;

	for (int i_pz = 0; i_pz < 2; i_pz++) {
		do {
			r = s_pz[j++];

			if (m_eBand == EDF_LPF || m_eBand == EDF_HPF) {
				z_pz[++jt] = (1.0 + xc * r) / (1.0 - xc * r);
				if (imag(r) != 0.0) {
					jt++;
					z_pz[jt] = conj(z_pz[jt - 1]);
				}
			} else {
				cx = r * (m_eShape == EDF_Chebyshev ? cbp : c);
				ca = 1.0 - cx;
				cb = -2.0 * cgam;
				b4ac = sqrt(cb * cb - 4.0 * (1.0 - cx * cx));
				root = (b4ac - cb) / (2.0 * ca);

				z_pz[++jt] = root;
				if (imag(root) != 0.0)
					z_pz[++jt] = conj(root);
				if ((imag(r) != 0.0) || (imag(root) == 0)) {
					root = -(b4ac + cb) / (2.0 * ca);
					z_pz[++jt] = root;
					if (imag(root) != 0.0)
						z_pz[++jt] = conj(root);
				}
			}
		} while (--nc > 0);

		if (i_pz == 0) {
			m_nZOrder = jt + 1;
			if (nz <= 0)
				return (jt);
		}
		nc = nz;
	}
	return (jt);
}

// 残りの零点を付加
void CDigitalFilter::AddZero(int jt)
{
	while (2 * m_nZOrder - 1 > jt) {
		if (m_eBand != EDF_HPF)
			z_pz[++jt] = -1.0;

		if (m_eBand == EDF_HPF || m_eBand == EDF_BPF)
			z_pz[++jt] = 1.0;
	}
}

// 縦続形の係数に変換
void CDigitalFilter::ToCascade(double cgam, double fScale)
{
	int m, jz, pos;
	double min, tmp_a0 = 0, tmp_a1 = 0, tmp_b0 = 0, tmp_b1 = 0, tmp_b2 = 0;

	m = 0;
	for (int j = 0; j < m_nZOrder; j++) {
		jz = j + m_nZOrder;
		if (fabs(imag(z_pz[j])) > 1.0E-16) {
			akm[0][m] = real(2.0 * z_pz[j]);
			akm[1][m] = -norm(z_pz[j]);
			bkm[0][m] = 1.0;
			bkm[1][m] = -real(z_pz[jz] + z_pz[jz + 1]);
			bkm[2][m] = real(z_pz[jz] * z_pz[jz + 1]);
			j++;
		} else {
			if (m_nZOrder % 2 != 0) {
				akm[0][m] = real(z_pz[j]);
				akm[1][m] = 0.0;
				bkm[0][m] = 1.0;
				bkm[1][m] = real(-z_pz[jz]);
				bkm[2][m] = 0.0;
			} else {
				akm[0][m] = real(z_pz[j] + z_pz[j + 1]);
				akm[1][m] = -real(z_pz[j] * z_pz[j + 1]);
				bkm[0][m] = 1.0;
				bkm[1][m] = -real(z_pz[jz] + z_pz[jz + 1]);
				bkm[2][m] = real(z_pz[jz] * z_pz[jz + 1]);
				j++;
			}

		}
		m++;
	}

	for (int i = 0; i < (m_nZOrder + 1) / 2 - 1; i++) {
		pos = i;
		min = fabs(akm[1][i]);
		for (int j = i; j < (m_nZOrder + 1) / 2; j++) {
			if (fabs(akm[1][j]) <= min) {
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

	for (int j = 0; j < (m_nZOrder + 1) / 2; j++) {
		double gain;
		if (m_eBand == EDF_BPF) {
			complex<double> z = exp(complex<double>(0, -acos(cgam)));
			complex<double> z2 = z * z;
			complex<double> an = 1.0 + -akm[0][j] * z + -akm[1][j] * z2;
			complex<double> bn = 1.0 + bkm[1][j] * z + bkm[2][j] * z2;
			gain = abs(an / bn);
		} else {
			double a;
			if (m_eBand == EDF_HPF)
				a = -1.0;
			else
				a = 1.0;
			const double an = 1.0 - a * akm[0][j] - akm[1][j];
			const double bn = 1.0 + a * bkm[1][j] + bkm[2][j];
			gain = an / bn;
		}

		if (j == 0)
			gain /= fScale;

		bkm[0][j] *= gain;
		bkm[1][j] *= gain;
		bkm[2][j] *= gain;
	}
}
