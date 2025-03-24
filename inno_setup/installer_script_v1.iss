#define MyAppName "GaitSym5"
#define MyAppVersion "v0.0.2-alpha"
#define MyAppPublisher "WIS"
#define MyAppURL "http://www.animalsimulation.org"
#define MyAppExeName "GaitSym5.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
AppId={2e375e10-899f-4cc9-a6b8-0a984da17989}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
LicenseFile=..\LICENSE
OutputDir=inno_setup
OutputBaseFilename=GaitSym5_v0.0.2-alpha_Setup
Compression=lzma
SolidCompression=yes
PrivilegesRequiredOverridesAllowed=commandline dialog
ArchitecturesInstallIn64BitMode=x64
ArchitecturesAllowed=x64
ChangesAssociations = yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "..\GaitSymQt\build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release\GaitSym5.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\GaitSymQt\build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\GaitSymQt\build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release\generic\*"; DestDir: "{app}\generic"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\GaitSymQt\build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release\iconengines\*"; DestDir: "{app}\iconengines"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\GaitSymQt\build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release\imageformats\*"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\GaitSymQt\build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release\networkinformation\*"; DestDir: "{app}\networkinformation"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\GaitSymQt\build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\GaitSymQt\build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release\styles\*"; DestDir: "{app}\styles"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\GaitSymQt\build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release\tls\*"; DestDir: "{app}\tls"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\GaitSymQt\build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release\translations\*"; DestDir: "{app}\translations"; Flags: ignoreversion recursesubdirs createallsubdirs


Source: "..\LICENSE"; DestDir: "{app}"; Flags: ignoreversion

Source: "..\GaitSymQt\build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release\vc_redist.x64.exe"; DestDir: {tmp}; Flags: deleteafterinstall

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: {tmp}\vc_redist.x64.exe; Parameters: "/q /passive /Q:a /c:""msiexec /q /i vcredist.msi"""; StatusMsg: "Installing VC++ 20xx Redistributables..."
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Registry]
Root: HKCR; SubKey: ".gaitsym"; ValueType: string; ValueData: "GaitSym configuration file"; Flags: uninsdeletekey
Root: HKCR; SubKey: "GaitSym configuration file"; ValueType: string; ValueData: "Defines the model and links to the meshes"; Flags: uninsdeletekey
Root: HKCR; SubKey: "GaitSym configuration file\Shell\Open\Command"; ValueType: string; ValueData: """{app}\GaitSym5.exe"" ""%1"""; Flags: uninsdeletekey
Root: HKCR; Subkey: "GaitSym configuration file\Default Icon"; ValueType: string; ValueData: "{app}\GaitSym5.exe,0"; Flags: uninsdeletevalue


