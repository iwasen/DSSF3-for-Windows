pause
"C:\Program Files (x86)\Inno Setup 6\Compil32.exe" /cc DSSF3J32.iss
"C:\Program Files (x86)\Inno Setup 6\Compil32.exe" /cc DSSF3J64.iss
"C:\Program Files (x86)\Inno Setup 6\Compil32.exe" /cc DSSF3E32.iss
"C:\Program Files (x86)\Inno Setup 6\Compil32.exe" /cc DSSF3E64.iss

copy Installer\* W:\ymec\ystore\admin\download\
scp Installer\* aizawa@ymec.com:/htdocs/ikou_www/ystore/admin/download/

pause