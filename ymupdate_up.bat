pause

xcopy /D /Y win32\EngRel\DSS.exe w:\ymec\ymupdate\DSSF5E32
xcopy /D /Y win32\EngRel\NMS.exe w:\ymec\ymupdate\DSSF5E32
xcopy /D /Y win32\EngRel\RTA.exe w:\ymec\ymupdate\DSSF5E32
xcopy /D /Y win32\EngRel\SAS.exe w:\ymec\ymupdate\DSSF5E32
xcopy /D /Y ReadMe\Eng\ReadMe.txt w:\ymec\ymupdate\DSSF5E32

xcopy /D /Y win32\JpnRel\DSS.exe w:\ymec\ymupdate\DSSF5J32
xcopy /D /Y win32\JpnRel\NMS.exe w:\ymec\ymupdate\DSSF5J32
xcopy /D /Y win32\JpnRel\RTA.exe w:\ymec\ymupdate\DSSF5J32
xcopy /D /Y win32\JpnRel\SAS.exe w:\ymec\ymupdate\DSSF5J32
xcopy /D /Y ReadMe\Jpn\ReadMe.txt w:\ymec\ymupdate\DSSF5J32

xcopy /D /Y x64\EngRel\DSS.exe w:\ymec\ymupdate\DSSF5E64
xcopy /D /Y x64\EngRel\NMS.exe w:\ymec\ymupdate\DSSF5E64
xcopy /D /Y x64\EngRel\RTA.exe w:\ymec\ymupdate\DSSF5E64
xcopy /D /Y x64\EngRel\SAS.exe w:\ymec\ymupdate\DSSF5E64
xcopy /D /Y ReadMe\Eng\ReadMe.txt w:\ymec\ymupdate\DSSF5E64

xcopy /D /Y x64\JpnRel\DSS.exe w:\ymec\ymupdate\DSSF5J64
xcopy /D /Y x64\JpnRel\NMS.exe w:\ymec\ymupdate\DSSF5J64
xcopy /D /Y x64\JpnRel\RTA.exe w:\ymec\ymupdate\DSSF5J64
xcopy /D /Y x64\JpnRel\SAS.exe w:\ymec\ymupdate\DSSF5J64
xcopy /D /Y ReadMe\Jpn\ReadMe.txt w:\ymec\ymupdate\DSSF5J64

@echo off
echo:
echo ヘルプファイルをアップロードしない場合はここで中断してください
echo:
pause > nul
@echo on

xcopy /D /Y dssf3e_ra.pdf w:\ymec\ymupdate\DSSF5_COMMON
xcopy /D /Y dssf3j_ra.pdf w:\ymec\ymupdate\DSSF5_COMMON
xcopy /D /Y NMS_J.chm w:\ymec\ymupdate\DSSF5_COMMON
xcopy /D /Y RTA_E.chm w:\ymec\ymupdate\DSSF5_COMMON
xcopy /D /Y RTA_J.chm w:\ymec\ymupdate\DSSF5_COMMON
xcopy /D /Y SAS_J.chm w:\ymec\ymupdate\DSSF5_COMMON
