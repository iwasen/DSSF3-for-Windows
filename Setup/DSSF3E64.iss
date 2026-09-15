[Setup]
AppName=Diagnostic System for Sound Fields
AppVerName=Diagnostic System for Sound Fields Version 5E
AppVersion=5.2.0
AppCopyright=Copyright (C) 2000-2026 Yoshimasa Electronic Inc.
VersionInfoVersion=5.2.0
PrivilegesRequired=admin
DefaultDirName={commonpf}\YMEC\DSSF3E
DefaultGroupName=Acoustic Analyzing System
OutputBaseFilename=DSSF3E64
LicenseFile=..\..\MyDev\RTF\LicenseAgreementE.rtf
UninstallDisplayIcon={app}\RTA.exe
Compression=lzma
SolidCompression=yes
OutputDir=Installer
; "ArchitecturesAllowed=x64" specifies that Setup cannot run on
; anything but x64.
ArchitecturesAllowed=x64
; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
; done in "64-bit mode" on x64, meaning it should use the native
; 64-bit Program Files directory and the 64-bit view of the registry.
ArchitecturesInstallIn64BitMode=x64
AppPublisher=Yoshimasa Electronic Inc.
SetupIconFile=compiler:SetupClassicIcon.ico

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "..\x64\EngRel\DSS.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\x64\EngRel\RTA.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\x64\EngRel\SAS.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\x64\EngRel\NMS.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\x64\EngRel\MMDB.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\x64\EngRel\PcsWin.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\x64\EngRel\RegistE.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\RTA_E.chm"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\dssf3e_ra.pdf"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\x64\EngRel\YmUpdate.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\x64\EngRel\YmUpdate.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Readme\Eng\Readme.txt"; DestDir: "{app}"; Flags: isreadme ignoreversion
Source: "..\SampleEng\ACPARAM.BIN"; DestDir: "{app}\DataBase"
Source: "..\SampleEng\ACPARAM.DBF"; DestDir: "{app}\DataBase"
Source: "..\SampleEng\FOLDER.DBF"; DestDir: "{app}\DataBase"
Source: "..\SampleEng\IMPULSE.BIN"; DestDir: "{app}\DataBase"
Source: "..\SampleEng\IMPULSE.DBF"; DestDir: "{app}\DataBase"
;Source: "..\..\MyDev\FONT\SSerife2.fon"; DestDir: "{fonts}"; FontInstall: onlyifdoesntexist uninsneveruninstall
Source: "..\..\MyDev\DLL32\HardKey.dll"; DestDir: "{app}"; Flags: dontcopy

[Icons]
Name: "{group}\Setting Utility"; Filename: "{app}\DSS.exe"; WorkingDir: "{app}"
Name: "{group}\Realtime Analyzer"; Filename: "{app}\RTA.exe"; WorkingDir: "{app}"
Name: "{group}\Sound Analyzing System"; Filename: "{app}\SAS.exe"; WorkingDir: "{app}"
Name: "{group}\Noise Measurement System"; Filename: "{app}\NMS.exe"; WorkingDir: "{app}"

[Code]
procedure GetHardKey(nAddSeed: Integer; sHardKey: AnsiString);
external 'GetHardKey@files:HardKey.dll stdcall setuponly';

function GetHardKey2(param: String): String;
var
  asHardKey: AnsiString;
  sHardKey: String;
  rc: Boolean;
begin
  rc := RegQueryStringValue(HKLM, 'SoftWare\Yoshimasa Electronic Inc.\Analyzing System for Sound Fields Ver5', 'HardKey', sHardKey);
  if rc = false then begin
    asHardKey := '00000000';
    GetHardKey(0, asHardKey);
    sHardKey := String(asHardKey);
  end;
  Result := sHardKey;
end;

// ***インストール画面を最前面に表示*** ↓↓↓ここから↓↓↓
const
  SWP_NOMOVE       = $0002;
  SWP_NOSIZE       = $0001;
  TOPMOST_FLAGS    = SWP_NOMOVE or SWP_NOSIZE;
  HWND_TOPMOST     = -1;
  HWND_NOTOPMOST   = -2;

procedure SetWindowPos(
  hWnd: HWND;
  hWndInsertAfter: HWND;
  X, Y, cx, cy: Integer;
  uFlags: UINT
); external 'SetWindowPos@user32.dll stdcall';

procedure SetForegroundWindow(hWnd: HWND); external 'SetForegroundWindow@user32.dll stdcall';
procedure ShowWindow(hWnd: HWND; nCmdShow: Integer); external 'ShowWindow@user32.dll stdcall';
procedure BringWindowToTop(hWnd: HWND); external 'BringWindowToTop@user32.dll stdcall';

// より強力な前面復帰処理
procedure BringWizardToFront();
begin
  // 複数の方法を組み合わせて確実に前面に
  ShowWindow(WizardForm.Handle, 9);  // SW_RESTORE
  BringWindowToTop(WizardForm.Handle);
  SetWindowPos(WizardForm.Handle, HWND_TOPMOST, 0, 0, 0, 0, TOPMOST_FLAGS);
  SetForegroundWindow(WizardForm.Handle);
  SetWindowPos(WizardForm.Handle, HWND_NOTOPMOST, 0, 0, 0, 0, TOPMOST_FLAGS);
  
  // 少し待ってから再度実行
  Sleep(100);
  SetForegroundWindow(WizardForm.Handle);
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssInstall then
  begin
    BringWizardToFront();
  end;
end;

// UAC後の復帰をより確実にする
procedure CurPageChanged(CurPageID: Integer);
begin
  // どのページでも前面に保持
  BringWizardToFront();
end;
// ***インストール画面を最前面に表示*** ↑↑↑ここまで↑↑↑

[Registry]
Root: HKLM; Subkey: SoftWare\\Yoshimasa Electronic Inc.\\Analyzing System for Sound Fields Ver5; ValueType: string; ValueName: HardKey; ValueData: {code:GetHardKey2};
Root: HKLM; Subkey: SoftWare\\Yoshimasa Electronic Inc.\\Analyzing System for Sound Fields Ver5; ValueType: dword; ValueName: Version; ValueData: 1;

Root: HKCR; Subkey: ".wavdssf"; ValueType: string; ValueName: ""; ValueData: "Dssf3.AutoRec"; Flags: uninsdeletevalue deletevalue
Root: HKCR; Subkey: "Dssf3.AutoRec"; ValueType: string; ValueName: ""; ValueData: "Dssf3 Automatic Recorded file"; Flags: uninsdeletevalue deletevalue
Root: HKCR; Subkey: "Dssf3.AutoRec\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\RTA.exe"" /A ""%1"""; Flags: uninsdeletevalue deletevalue
