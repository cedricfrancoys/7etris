Name "7etris"

OutFile "7etris setup.exe"
InstallDir "$PROGRAMFILES\7etris"
Page directory
Page InstFiles

Section
	SetOutPath $INSTDIR
	File "7etris.exe"
	File "glut32.dll"
	File "glut.h"
	File "tetris.cpp"
	File "tetris_utils.cpp"	
SectionEnd

Function .onInstSuccess
	CreateShortCut "$DESKTOP\7etris.lnk" "$INSTDIR\7etris.exe"
	MessageBox MB_YESNO "Application successfully installed to $INSTDIR. $\r$\nLaunch game now?" IDNO NoRun
	Exec '"$INSTDIR\7etris.exe"'
	NoRun:
FunctionEnd

