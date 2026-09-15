// Keyboard.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "Keyboard.h"

#define COLOR_WHITE_KEY	RGB(255,255,255)
#define COLOR_BLACK_KEY RGB(64,64,64)
#define COLOR_MARKER RGB(0,224,0)

// CKeyboard

CKeyboard::CKeyboard()
{
	m_bKeyPress = FALSE;
	m_bMarker = FALSE;
}

BEGIN_MESSAGE_MAP(CKeyboard, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CKeyboard メッセージ ハンドラ

void CKeyboard::OnPaint()
{
	CPaintDC dc(this);

	int i;

	for (i = 0; i < 52; i++)
		dc.Rectangle(m_KeyRect[i + 36]);

	for (i = 0; i < 36; i++)
		dc.FillSolidRect(m_KeyRect[i], COLOR_BLACK_KEY);

	if (m_bMarker)
		dc.FillSolidRect(m_MarkerRect, COLOR_MARKER);
}

BOOL CKeyboard::OnEraseBkgnd(CDC* pDC)
{
	RECT rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, RGB(255, 255, 255));

	return CWnd::OnEraseBkgnd(pDC);
}

void CKeyboard::OnLButtonDown(UINT nFlags, CPoint point)
{
	NotifyMessage(point);

	m_bKeyPress = TRUE;
	SetCapture();

	CWnd::OnLButtonDown(nFlags, point);
}

void CKeyboard::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bKeyPress)
		NotifyMessage(point);

	CWnd::OnMouseMove(nFlags, point);
}

void CKeyboard::NotifyMessage(CPoint point)
{
	for (int i = 0; i < 88; i++) {
		if (m_KeyRect[i].PtInRect(point)) {
			GetParent()->SendMessage(WM_KEYBOARD, m_nOctave[i], m_nKey[i]);
			break;
		}
	}
}

void CKeyboard::OnLButtonUp(UINT nFlags, CPoint point)
{
	GetParent()->SendMessage(WM_KEYBOARD, 0, 0);

	m_bKeyPress = FALSE;
	ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

void CKeyboard::SetMarker(int nOctave, int nKey)
{
	CDC *pDC = GetDC();

	if (m_bMarker) {
		pDC->FillSolidRect(m_MarkerRect, m_MarkerColor);
		m_bMarker = FALSE;
	}

	for (int i = 0; i < 88; i++) {
		if (m_nOctave[i] == nOctave && m_nKey[i] == nKey) {
			m_MarkerRect.SetRect(m_KeyRect[i].left + 1, m_KeyRect[i].bottom - 8, m_KeyRect[i].right - 1, m_KeyRect[i].bottom - 4);

			switch (nKey) {
			case 1:
			case 3:
			case 6:
			case 8:
			case 10:
				m_MarkerColor = COLOR_BLACK_KEY;
				break;
			default:
				m_MarkerColor = COLOR_WHITE_KEY;
				m_MarkerRect.left++;
				m_MarkerRect.right--;
				break;
			}

			pDC->FillSolidRect(m_MarkerRect, COLOR_MARKER);

			m_bMarker = TRUE;
			break;
		}
	}

	ReleaseDC(pDC);
}

void CKeyboard::PreSubclassWindow()
{
	int i;
	CRect rect;
	GetClientRect(rect);

	int nOctave, nKey, nIndex;
	static const int tWhiteKey[7] = {0, 2, 4, 5, 7, 9, 11};
	static const int tBlackKey[5] = {1, 3, 6, 8, 10};

	double nWhiteKeyWidth = rect.right / 52.0;
	int nWhiteKeyHeight = rect.bottom;
	int nBlackKeyWidth = (int)(nWhiteKeyWidth * 2 / 3);
	int nBlackKeyHeight = rect.bottom * 13 / 20;

	for (i = 0; i < 52; i++) {
		nIndex = i + 36;
		m_KeyRect[nIndex].SetRect((int)(i * nWhiteKeyWidth), 0, (int)((i + 1) * nWhiteKeyWidth + 1), nWhiteKeyHeight);
		m_nOctave[nIndex] = (i + 5) / 7;
		m_nKey[nIndex] = tWhiteKey[(i + 5) % 7];
	}

	static const int key[5] = {1, 2, 4, 5, 6};
	static const int pos[5] = {0, 1, 0, 0, 0};

	for (i = 0; i < 36; i++) {
		nOctave = (i + 4) / 5;
		nKey = (i + 4) % 5;
		double x = (nOctave * 7 + key[nKey] - 5) * nWhiteKeyWidth - nBlackKeyWidth / 2 + pos[nKey];

		nIndex = i;
		m_KeyRect[nIndex].SetRect((int)x, 0, (int)(x + nBlackKeyWidth), nBlackKeyHeight);
		m_nOctave[nIndex] = nOctave;
		m_nKey[nIndex] = tBlackKey[nKey];
	}

//	SetWindowPos(NULL, 0, 0, nWhiteKeyWidth * 52, nWhiteKeyHeight, SWP_NOMOVE | SWP_NOZORDER);

	CWnd::PreSubclassWindow();
}
