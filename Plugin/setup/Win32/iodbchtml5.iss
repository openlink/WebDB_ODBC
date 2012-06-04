; -- Example3.iss --
; Same as Example1.iss, but creates some registry entries too.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=OpenLink HTML5 WebDB-to-ODBC (Bridge) Plugin
AppVersion=1.0.3
AppCopyright=Copyright (C) 2005-2011 OpenLink Software
AppPublisher=OpenLink Software
AppPublisherURL=http://uda.openlinksw.com
AppSupportURL=http://uda.openlinksw.com
AppUpdatesURL=http://uda.openlinksw.com

DefaultDirName={pf}\OpenLink Software\Extension
DefaultGroupName=OpenLink
OutputBaseFilename=Setup_HTML5_WebDb2ODBC_Plugin
;UninstallDisplayIcon={app}\MyProg.exe
;OutputDir=userdocs:Inno Setup Examples Output

[Files]
Source: "npiodbc.dll"; DestDir: "{app}"; Flags: 32bit replacesameversion ignoreversion

[Icons]
;Name: "{group}\My Program"; Filename: "{app}\MyProg.exe"

; NOTE: Most apps do not need registry entries to be pre-created. If you
; don't know what the registry is or if you need to use it, then chances are
; you don't need a [Registry] section.

[Registry]
; Start "Software\My Company\My Program" keys under HKEY_CURRENT_USER
; and HKEY_LOCAL_MACHINE. The flags tell it to always delete the
; "My Program" keys upon uninstall, and delete the "My Company" keys
; if there is nothing left in them.
Root: HKLM; Subkey: "SOFTWARE\MozillaPlugins";
Root: HKLM; Subkey: "SOFTWARE\MozillaPlugins\@openlinksw.com/npiodbc,version=1.00"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\MozillaPlugins\@openlinksw.com/npiodbc,version=1.00"; ValueType: string; ValueName: "Path"; ValueData: "{app}\npiodbc.dll"
Root: HKLM; Subkey: "SOFTWARE\MozillaPlugins\@openlinksw.com/npiodbc,version=1.00"; ValueType: string; ValueName: "GeckoVersion"; ValueData: "1.0.3"
Root: HKLM; Subkey: "SOFTWARE\MozillaPlugins\@openlinksw.com/npiodbc,version=1.00"; ValueType: string; ValueName: "Version"; ValueData: "1.0.3"
Root: HKLM; Subkey: "SOFTWARE\MozillaPlugins\@openlinksw.com/npiodbc,version=1.00"; ValueType: string; ValueName: "Vendor"; ValueData: "OpenLink Software"
Root: HKLM; Subkey: "SOFTWARE\MozillaPlugins\@openlinksw.com/npiodbc,version=1.00"; ValueType: string; ValueName: "Description"; ValueData: "OpenLink HTML5 WebDB-to-ODBC (Bridge) Plugin ver:1.0.3"
Root: HKLM; Subkey: "SOFTWARE\MozillaPlugins\@openlinksw.com/npiodbc,version=1.00"; ValueType: string; ValueName: "ProductName"; ValueData: "NPIODBC"





