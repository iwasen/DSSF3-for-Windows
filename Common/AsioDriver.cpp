#include "StdAfx.h"
#include "AsioDriver.h"
#include "comres.h"

#define ASIODRV_DESC		"description"
#define INPROC_SERVER		"InprocServer32"
#define ASIO_PATH			"software\\asio"
#define COM_CLSID			"clsid"

LPASIODRVSTRUCT CAsioDriver::m_pDrvList;
int CAsioDriver::m_nDrv;

CAsioDriver::CAsioDriver()
{
	m_pAsioDriver = NULL;
	m_nCurIndex = -1;
}

void CAsioDriver::Initialize(void)
{
	HKEY			hkEnum = 0;
	char			keyname[MAXDRVNAMELEN];
	LPASIODRVSTRUCT	pdl;
	LONG 			cr;
	DWORD			index = 0;
	BOOL			fin = FALSE;

	m_nDrv		= 0;
	m_pDrvList	= 0;

	cr = RegOpenKey(HKEY_LOCAL_MACHINE,ASIO_PATH,&hkEnum);
	while (cr == ERROR_SUCCESS) {
		if ((cr = RegEnumKey(hkEnum,index++,(LPSTR)keyname,MAXDRVNAMELEN))== ERROR_SUCCESS) {
			m_pDrvList = newDrvStruct (hkEnum,keyname,0,m_pDrvList);
		}
		else fin = TRUE;
	}
	if (hkEnum) RegCloseKey(hkEnum);

	pdl = m_pDrvList;
	while (pdl) {
		m_nDrv++;
		pdl = pdl->next;
	}

//	if (m_nDrv) CoInitialize(0);	// initialize COM
}

void CAsioDriver::UnInitialize(void)
{
	if (m_nDrv) {
		deleteDrvStruct(m_pDrvList);
//		CoUninitialize();
	}
}

LONG CAsioDriver::findDrvPath (LPSTR clsidstr, LPCSTR dllpath, int dllpathsize)
{
	HKEY			hkEnum,hksub,hkpath;
	char			databuf[512];
	LONG 			cr,rc = -1;
	DWORD			datatype,datasize;
	DWORD			index;
	OFSTRUCT		ofs;
	HFILE			hfile;
	BOOL			found = FALSE;

	CharLowerBuff(clsidstr, (DWORD)strlen(clsidstr));
	if ((cr = RegOpenKey(HKEY_CLASSES_ROOT,COM_CLSID,&hkEnum)) == ERROR_SUCCESS) {
		index = 0;
		while (cr == ERROR_SUCCESS && !found) {
			cr = RegEnumKey(hkEnum,index++,(LPSTR)databuf,512);
			if (cr == ERROR_SUCCESS) {
				CharLowerBuff(databuf, (DWORD)strlen(databuf));
				if (!(strcmp(databuf,clsidstr))) {
					if ((cr = RegOpenKeyEx(hkEnum,(LPCSTR)databuf,0,KEY_READ,&hksub)) == ERROR_SUCCESS) {
						if ((cr = RegOpenKeyEx(hksub,(LPCSTR)INPROC_SERVER,0,KEY_READ,&hkpath)) == ERROR_SUCCESS) {
							datatype = REG_SZ; datasize = (DWORD)dllpathsize;
							cr = RegQueryValueEx(hkpath,0,0,&datatype,(LPBYTE)dllpath,&datasize);
							if (cr == ERROR_SUCCESS) {
								memset(&ofs,0,sizeof(OFSTRUCT));
								ofs.cBytes = sizeof(OFSTRUCT);
								hfile = OpenFile(dllpath,&ofs,OF_EXIST);
								if (hfile) rc = 0;
							}
							RegCloseKey(hkpath);
						}
						RegCloseKey(hksub);
					}
					found = TRUE;	// break out
				}
			}
		}
		RegCloseKey(hkEnum);
	}
	return rc;
}

LPASIODRVSTRUCT CAsioDriver::newDrvStruct (HKEY hkey, LPCSTR keyname,int drvID, LPASIODRVSTRUCT lpdrv)
{
	HKEY	hksub;
	char	databuf[256];
	char	dllpath[MAXPATHLEN];
	WORD	wData[100];
	CLSID	clsid;
	DWORD	datatype,datasize;
	LONG	cr,rc;

	if (!lpdrv) {
		if ((cr = RegOpenKeyEx(hkey,(LPCSTR)keyname,0,KEY_READ,&hksub)) == ERROR_SUCCESS) {
			datatype = REG_SZ; datasize = 256;
			cr = RegQueryValueEx(hksub,COM_CLSID,0,&datatype,(LPBYTE)databuf,&datasize);
			if (cr == ERROR_SUCCESS) {
				rc = findDrvPath (databuf,dllpath,MAXPATHLEN);
				if (rc == 0) {
					lpdrv = new ASIODRVSTRUCT[1];
					if (lpdrv) {
						memset(lpdrv,0,sizeof(ASIODRVSTRUCT));
						lpdrv->drvID = drvID;
						MultiByteToWideChar(CP_ACP,0,(LPCSTR)databuf,-1,(LPWSTR)wData,100);
						if ((cr = CLSIDFromString((LPOLESTR)wData,(LPCLSID)&clsid)) == S_OK) {
							memcpy(&lpdrv->clsid,&clsid,sizeof(CLSID));
						}

						datatype = REG_SZ; datasize = 256;
						cr = RegQueryValueEx(hksub,ASIODRV_DESC,0,&datatype,(LPBYTE)databuf,&datasize);
						if (cr == ERROR_SUCCESS) {
							strcpy_s(lpdrv->drvname,databuf);
						}
						else strcpy_s(lpdrv->drvname,keyname);
					}
				}
			}
			RegCloseKey(hksub);
		}
	}
	else lpdrv->next = newDrvStruct(hkey,keyname,drvID+1,lpdrv->next);

	return lpdrv;
}

void CAsioDriver::deleteDrvStruct (LPASIODRVSTRUCT lpdrv)
{
	IASIO	*iasio;

	if (lpdrv != 0) {
		deleteDrvStruct(lpdrv->next);
		if (lpdrv->asiodrv) {
			iasio = (IASIO *)lpdrv->asiodrv;
			iasio->Release();
		}
		delete lpdrv;
	}
}

LPASIODRVSTRUCT CAsioDriver::getDrvStruct (INT_PTR drvID,LPASIODRVSTRUCT lpdrv)
{
	while (lpdrv) {
		if (lpdrv->drvID == drvID) return lpdrv;
		lpdrv = lpdrv->next;
	}
	return 0;
}

LONG CAsioDriver::asioGetNumDev (VOID)
{
	return (LONG)m_nDrv;
}

LONG CAsioDriver::asioOpenDriver (INT_PTR drvID)
{
	LPASIODRVSTRUCT	lpdrv = 0;
	long			rc;

	if ((lpdrv = getDrvStruct(drvID,m_pDrvList)) != 0) {
		if (!lpdrv->asiodrv) {
			rc = CoCreateInstance(lpdrv->clsid, 0, CLSCTX_INPROC_SERVER, lpdrv->clsid, (LPVOID *)&m_pAsioDriver);
			if (rc == S_OK) {
				lpdrv->asiodrv = m_pAsioDriver;
				m_nCurIndex = drvID;
				return 0;
			}
		}
		else rc = DRVERR_DEVICE_ALREADY_OPEN;
	}
	else rc = DRVERR_DEVICE_NOT_FOUND;

	return rc;
}

LONG CAsioDriver::asioCloseDriver ()
{
	if (m_nCurIndex != -1) {
		LPASIODRVSTRUCT	lpdrv = 0;
		IASIO			*iasio;

		if ((lpdrv = getDrvStruct(m_nCurIndex,m_pDrvList)) != 0) {
			if (lpdrv->asiodrv) {
				iasio = (IASIO *)lpdrv->asiodrv;
				iasio->Release();
				lpdrv->asiodrv = 0;
			}
		}

		m_nCurIndex = -1;
	}

	return 0;
}

LONG CAsioDriver::asioGetDriverName(int drvID, CString &drvname)
{
	LPASIODRVSTRUCT			lpdrv = 0;

	if (!drvname) return DRVERR_INVALID_PARAM;

	if ((lpdrv = getDrvStruct(drvID,m_pDrvList)) != 0) {
		drvname = lpdrv->drvname;
		return 0;
	}
	return DRVERR_DEVICE_NOT_FOUND;
}

long CAsioDriver::getDriverNames(CStringArray &names)
{
	names.SetSize(asioGetNumDev());
	for(long i = 0; i < asioGetNumDev(); i++)
		asioGetDriverName(i, names[i]);
	return asioGetNumDev();
}

ASIOError CAsioDriver::ASIOInit()
{
	m_asioDriverInfo.driverVersion = 0;
	strcpy_s(m_asioDriverInfo.name, "No ASIO Driver");
	if(m_pAsioDriver)	// must be loaded!
	{
		if(!m_pAsioDriver->init(m_asioDriverInfo.sysRef))
		{
			m_pAsioDriver->getErrorMessage(m_asioDriverInfo.errorMessage);
//			m_pAsioDriver = 0;
			return ASE_NotPresent;
		}

		strcpy_s(m_asioDriverInfo.errorMessage, "No ASIO Driver Error");
		m_pAsioDriver->getDriverName(m_asioDriverInfo.name);
		m_asioDriverInfo.driverVersion = m_pAsioDriver->getDriverVersion();
		return ASE_OK;
	}
	return ASE_NotPresent;
}

ASIOError CAsioDriver::ASIOExit(void)
{
	if(m_pAsioDriver)
	{
		asioCloseDriver();
	}
	m_pAsioDriver = 0;
	return ASE_OK;
}

ASIOError CAsioDriver::ASIOStart(void)
{
	if(!m_pAsioDriver)
		return ASE_NotPresent;
	return m_pAsioDriver->start();
}

ASIOError CAsioDriver::ASIOStop(void)
{
	if(!m_pAsioDriver)
		return ASE_NotPresent;
	return m_pAsioDriver->stop();
}

ASIOError CAsioDriver::ASIOGetChannels(long *numInputChannels, long *numOutputChannels)
{
	if(!m_pAsioDriver)
	{
		*numInputChannels = *numOutputChannels = 0;
		return ASE_NotPresent;
	}
	return m_pAsioDriver->getChannels(numInputChannels, numOutputChannels);
}

ASIOError CAsioDriver::ASIOGetLatencies(long *inputLatency, long *outputLatency)
{
	if(!m_pAsioDriver)
	{
		*inputLatency = *outputLatency = 0;
		return ASE_NotPresent;
	}
	return m_pAsioDriver->getLatencies(inputLatency, outputLatency);
}

ASIOError CAsioDriver::ASIOGetBufferSize(long *minSize, long *maxSize, long *preferredSize, long *granularity)
{
	if(!m_pAsioDriver)
	{
		*minSize = *maxSize = *preferredSize = *granularity = 0;
		return ASE_NotPresent;
	}
	return m_pAsioDriver->getBufferSize(minSize, maxSize, preferredSize, granularity);
}

ASIOError CAsioDriver::ASIOCanSampleRate(ASIOSampleRate sampleRate)
{
	if(!m_pAsioDriver)
		return ASE_NotPresent;
	return m_pAsioDriver->canSampleRate(sampleRate);
}

ASIOError CAsioDriver::ASIOGetSampleRate(ASIOSampleRate *currentRate)
{
	if(!m_pAsioDriver)
		return ASE_NotPresent;
	return m_pAsioDriver->getSampleRate(currentRate);
}

ASIOError CAsioDriver::ASIOSetSampleRate(ASIOSampleRate sampleRate)
{
	if(!m_pAsioDriver)
		return ASE_NotPresent;
	return m_pAsioDriver->setSampleRate(sampleRate);
}

ASIOError CAsioDriver::ASIOGetClockSources(ASIOClockSource *clocks, long *numSources)
{
	if(!m_pAsioDriver)
	{
		*numSources = 0;
		return ASE_NotPresent;
	}
	return m_pAsioDriver->getClockSources(clocks, numSources);
}

ASIOError CAsioDriver::ASIOSetClockSource(long reference)
{
	if(!m_pAsioDriver)
		return ASE_NotPresent;
	return m_pAsioDriver->setClockSource(reference);
}

ASIOError CAsioDriver::ASIOGetSamplePosition(ASIOSamples *sPos, ASIOTimeStamp *tStamp)
{
	if(!m_pAsioDriver)
		return ASE_NotPresent;
	return m_pAsioDriver->getSamplePosition(sPos, tStamp);
}

ASIOError CAsioDriver::ASIOGetChannelInfo(ASIOChannelInfo *info)
{
	if(!m_pAsioDriver)
	{
		info->channelGroup = -1;
		info->type = ASIOSTInt16MSB;
		strcpy_s(info->name, "None");
		return ASE_NotPresent;
	}
	return m_pAsioDriver->getChannelInfo(info);
}

ASIOError CAsioDriver::ASIOCreateBuffers(ASIOBufferInfo *bufferInfos, long numChannels,
	long bufferSize, ASIOCallbacks *callbacks)
{
	if(!m_pAsioDriver)
	{
		ASIOBufferInfo *info = bufferInfos;
		for(long i = 0; i < numChannels; i++, info++)
			info->buffers[0] = info->buffers[1] = 0;
		return ASE_NotPresent;
	}
	return m_pAsioDriver->createBuffers(bufferInfos, numChannels, bufferSize, callbacks);
}

ASIOError CAsioDriver::ASIODisposeBuffers(void)
{
	if(!m_pAsioDriver)
		return ASE_NotPresent;
	return m_pAsioDriver->disposeBuffers();
}

ASIOError CAsioDriver::ASIOControlPanel(void)
{
	if(!m_pAsioDriver)
		return ASE_NotPresent;
	return m_pAsioDriver->controlPanel();
}

ASIOError CAsioDriver::ASIOFuture(long selector, void *opt)
{
	if(!m_pAsioDriver)
		return ASE_NotPresent;
	return m_pAsioDriver->future(selector, opt);
}

ASIOError CAsioDriver::ASIOOutputReady(void)
{
	if(!m_pAsioDriver)
		return ASE_NotPresent;
	return m_pAsioDriver->outputReady();
}

void CAsioDriver::DispErrorMessage()
{
	if (m_pAsioDriver != NULL) {
		CString sMsg;
		sMsg.LoadString(IDS_ASIO_OPEN_ERROR);

		char msg[256];
		msg[0] = '\0';
		m_pAsioDriver->getErrorMessage(msg);
		if (msg[0] != '\0')
			sMsg += (CString)"\n" + msg;
		::AfxMessageBox(sMsg, MB_ICONEXCLAMATION);
	}
}
