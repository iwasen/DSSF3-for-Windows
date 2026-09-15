#include "stdafx.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>
#include "wavefile.h"
#include "common.h"

#define FORMTYPE_WAVE (DWORD)((DWORD)'W' | ((DWORD)'A' << 8) | ((DWORD)'V' << 16) | ((DWORD)'E' << 24))

CWaveFile::~CWaveFile(void)
{
	Close();
}

BOOL CWaveFile::Create(LPCTSTR pFileName, const PCMWAVEFORMAT *pWaveFormat)
{
	if (!m_oWaveFile.Open(pFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
		return FALSE;

	m_nDataSize = 0;
	m_bWriteDataSize = FALSE;

	m_oPcmWaveFormat = *pWaveFormat;

	m_oWaveFileHdr.riffID = FOURCC_RIFF;
	m_oWaveFileHdr.riffSize = 36;
	m_oWaveFileHdr.formType = FORMTYPE_WAVE;
	m_oWaveFileHdr.fmtID = mmioFOURCC('f', 'm', 't', ' ');
	m_oWaveFileHdr.fmtSize = sizeof(PCMWAVEFORMAT);
	m_oWaveFileHdr.waveFormat = m_oPcmWaveFormat;
	m_oWaveFileHdr.dataID = mmioFOURCC('d', 'a', 't', 'a');
	m_oWaveFileHdr.dataSize = 0;

	m_oWaveFile.Write(&m_oWaveFileHdr, sizeof(WAVEFILEHDR));

	return TRUE;
}

void CWaveFile::Write(const double *pData, int nSamplesNum)
{
	if (m_oWaveFile.m_hFile != CFile::hFileNull) {
		int nDataSize = (m_oPcmWaveFormat.wBitsPerSample / 8) * nSamplesNum;
		BYTE *pWaveBuf = new BYTE[nDataSize];
		CopyWaveFromDouble(pWaveBuf, pData, NULL, nSamplesNum, m_oPcmWaveFormat.wBitsPerSample);

		m_oWaveFile.Write(pWaveBuf, nDataSize);
		delete [] pWaveBuf;

		m_nDataSize += nDataSize;
		m_bWriteDataSize = TRUE;
	}
}

void CWaveFile::Close()
{
	if (m_oWaveFile.m_hFile != CFile::hFileNull) {
		if (m_bWriteDataSize) {
			m_oWaveFileHdr.riffSize = 36 + m_nDataSize;
			m_oWaveFileHdr.dataSize = m_nDataSize;
			m_oWaveFile.SeekToBegin();
			m_oWaveFile.Write(&m_oWaveFileHdr, sizeof(WAVEFILEHDR));
		}
		m_oWaveFile.Close();
	}
}

BOOL CWaveFile::Open(LPCTSTR pFileName, PCMWAVEFORMAT *pWaveFormat, DWORD *pDataSize)
{
	if (!m_oWaveFile.Open(pFileName, CFile::modeRead | CFile::shareDenyNone))
		return FALSE;

	if (!ReadHeader(&m_oPcmWaveFormat, pDataSize)) {
		m_oWaveFile.Close();
		return FALSE;
	}

	*pWaveFormat = m_oPcmWaveFormat;
	m_bWriteDataSize = FALSE;

	return TRUE;
}

BOOL CWaveFile::ReadHeader(LPPCMWAVEFORMAT pWaveFormat, DWORD *pDataSize)
{
	struct	{
		FOURCC	ckID;
		DWORD	ckSize;
		DWORD	formType;
	} riff;
	WAVEFORMATEXTENSIBLE waveFormatExtensible;

	if (m_oWaveFile.Read(&riff, sizeof(riff)) != sizeof(riff))
		return FALSE;

	if (riff.ckID != FOURCC_RIFF)
		return FALSE;

	if (riff.formType != FORMTYPE_WAVE)
		return FALSE;

	if (!ReadChunk(mmioFOURCC('f', 'm', 't', ' '), &waveFormatExtensible, sizeof(WAVEFORMATEXTENSIBLE)))
		return FALSE;

	memcpy(pWaveFormat, &waveFormatExtensible.Format, sizeof(PCMWAVEFORMAT));
	if (waveFormatExtensible.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
		if (waveFormatExtensible.SubFormat == KSDATAFORMAT_SUBTYPE_PCM) {
			pWaveFormat->wf.wFormatTag = WAVE_FORMAT_PCM;
		} else if (waveFormatExtensible.SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
			pWaveFormat->wf.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
		}
	}

	if (pWaveFormat->wf.wFormatTag != WAVE_FORMAT_PCM && pWaveFormat->wf.wFormatTag != WAVE_FORMAT_IEEE_FLOAT)
		return FALSE;

	if (!SearchChunk(mmioFOURCC('d', 'a', 't', 'a'), pDataSize))
		return FALSE;

	return TRUE;
}

BOOL CWaveFile::ReadChunk(DWORD ckID, void *pData, DWORD nSize)
{
	DWORD	ckSize;

	if (!SearchChunk(ckID, &ckSize))
		return FALSE;

	DWORD nRead = min(ckSize, nSize);

	if (m_oWaveFile.Read(pData, nRead) != nRead)
		return FALSE;

	if (ckSize > nSize)
		m_oWaveFile.Seek(ckSize - nSize, CFile::current);

	return TRUE;
}

BOOL CWaveFile::SearchChunk(DWORD ckID, DWORD *ckSize)
{
	struct {
		FOURCC	ckID;
		DWORD	ckSize;
	} chunk;
	DWORD	size;

	for (;;) {
		if (m_oWaveFile.Read(&chunk, sizeof(chunk)) != sizeof(chunk))
			return FALSE;

		if (chunk.ckID == ckID) {
			*ckSize = chunk.ckSize;
			break;
		}

		size = (chunk.ckSize + 1) & ~0x01L;
		m_oWaveFile.Seek(size, CFile::current);
	}

	return TRUE;
}

void CWaveFile::Read(double *pData, int nSamplesNum)
{
	if (m_oWaveFile.m_hFile != CFile::hFileNull) {
		DWORD nDataSize = (m_oPcmWaveFormat.wBitsPerSample / 8) * nSamplesNum;
		BYTE *pWaveBuf = new BYTE[nDataSize];
		m_oWaveFile.Read(pWaveBuf, nDataSize);
		CopyWaveToDouble(pWaveBuf, pData, NULL, nSamplesNum, m_oPcmWaveFormat.wBitsPerSample);
		delete [] pWaveBuf;
	}
}
