!addincludedir "NSIS dirs\Include"
!addplugindir "NSIS dirs\Plugins"
!include nsDialogs.nsh
!include LogicLib.nsh
!include Sections.nsh
!include UAC.nsh

RequestExecutionLevel user
SetCompressor /SOLID /FINAL lzma

!define TEMP1 $R0
!define TEMP2 $R1
!define TEMP3 $R2

Var Install_Dir

!ifndef MAJOR_VER
    !define MAJOR_VER "1"
    !define MINOR_VER "6"
    !define MAINT_VER "0"
!endif
!define 0.0 "${MAJOR_VER}.${MINOR_VER}"
!define 0.0.0 "${MAJOR_VER}.${MINOR_VER}.${MAINT_VER}"


; ###########################################################################################################
; Don't forget to set the BUILD_NUMBER if this is built manually
; ###########################################################################################################
!ifndef FILES_ROOT
    !define FILES_ROOT "."
    !define BUILD_NUMBER "0000"
    !define INSTALL_OUTPUT "GninEsa-${0.0}-data-r${BUILD_NUMBER}.exe"
!endif
!define PRODUCT_VERSION "Data-r${BUILD_NUMBER}"
; ###########################################################################################################


!define PRODUCT_NAME "GninE:SA ${0.0}"
!define PRODUCT_NAME_NO_VER "GninE:SA"

!define PRODUCT_PUBLISHER "GninE"
!define PRODUCT_WEB_SITE "https://www.gnine.com"

; MUI 1.67 compatible ------
!include "MUI.nsh"
;!include "ReplaceSubStr.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON		"GninE.ico"
!define MUI_UNICON		"GninE.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; License page
!insertmacro MUI_PAGE_LICENSE					"eula.txt"

; Components page
!insertmacro MUI_PAGE_COMPONENTS

; Game directory page
!define MUI_DIRECTORYPAGE_VARIABLE				$INSTDIR
!insertmacro MUI_PAGE_DIRECTORY

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!insertmacro MUI_PAGE_FINISH


; Language files
!insertmacro MUI_LANGUAGE							"English"
LangString DESC_Section1 ${LANG_ENGLISH}			"The data files required to run GninE."


Function .OnInstFailed
	;UAC::Unload ;Must call unload!
FunctionEnd

Function .onInit
	Call DoRightsElevation

	; Try to find previously saved GninE:SA install path
	ReadRegStr $Install_Dir HKLM "SOFTWARE\GninE: San Andreas All\${0.0}" "Last Install Location"
	${If} $Install_Dir == "" 
		ReadRegStr $Install_Dir HKLM "SOFTWARE\GninE: San Andreas ${0.0}" "Last Install Location"
	${EndIf}
	${If} $Install_Dir == "" 
		strcpy $Install_Dir "$PROGRAMFILES\GninE San Andreas ${0.0}"
	${EndIf}
	strcpy $INSTDIR $Install_Dir
	
	InitPluginsDir
	;File /oname=$PLUGINSDIR\serialdialog.ini "serialdialog.ini"
FunctionEnd

Function .onInstSuccess
	WriteRegStr HKLM "SOFTWARE\GninE: San Andreas All\${0.0}" "Last Install Location" $INSTDIR

	;UAC::Unload ;Must call unload!
FunctionEnd



InstType "Data files only"
InstType /NOCUSTOM


Name "${PRODUCT_NAME_NO_VER} ${PRODUCT_VERSION}"
OutFile "${INSTALL_OUTPUT}"

InstallDirRegKey HKLM "SOFTWARE\GninE: San Andreas All\${0.0}" "Last Install Location"
ShowInstDetails show

Section "Data files" SEC01
	SectionIn 1 RO ; section is required

	WriteRegStr HKLM "SOFTWARE\GninE: San Andreas All\${0.0}" "Last Install Location" $INSTDIR

	SetOverwrite on

	SetOutPath "$INSTDIR\GninE"
	File "${FILES_ROOT}\GninE San Andreas\GninE\d3dx9_42.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\D3DCompiler_42.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\bass.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\basswma.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\bassmidi.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\bassflac.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\bass_aac.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\bass_ac3.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\bass_fx.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\bassopus.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\basswebm.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\bassmix.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\tags.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\sa.dat"
	File "${FILES_ROOT}\GninE San Andreas\GninE\XInput9_1_0_GninE.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\vea.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\vog.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\vvo.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\vvof.dll"
	
	File "${FILES_ROOT}\GninE San Andreas\GninE\d3dcompiler_43.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\d3dcompiler_47.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\libEGL.dll"
	File "${FILES_ROOT}\GninE San Andreas\GninE\libGLESv2.dll"

	SetOutPath "$INSTDIR\GninE\config"
	File "${FILES_ROOT}\GninE San Andreas\GninE\config\chatboxpresets.xml"

	SetOutPath "$INSTDIR\skins\Classic"
    File "${FILES_ROOT}\GninE San Andreas\skins\Classic\CGUI.is.xml"
    File "${FILES_ROOT}\GninE San Andreas\skins\Classic\CGUI.lnf.xml"
    File "${FILES_ROOT}\GninE San Andreas\skins\Classic\CGUI.png"
    File "${FILES_ROOT}\GninE San Andreas\skins\Classic\CGUI.xml"
    
    SetOutPath "$INSTDIR\skins\Default"
    File "${FILES_ROOT}\GninE San Andreas\skins\Default\CGUI.is.xml"
    File "${FILES_ROOT}\GninE San Andreas\skins\Default\CGUI.lnf.xml"
    File "${FILES_ROOT}\GninE San Andreas\skins\Default\CGUI.png"
    File "${FILES_ROOT}\GninE San Andreas\skins\Default\CGUI.xml"
                
    SetOutPath "$INSTDIR\skins\Lighter black"
    File "${FILES_ROOT}\GninE San Andreas\skins\Lighter black\CGUI.is.xml"
    File "${FILES_ROOT}\GninE San Andreas\skins\Lighter black\CGUI.lnf.xml"
    File "${FILES_ROOT}\GninE San Andreas\skins\Lighter black\CGUI.png"
    File "${FILES_ROOT}\GninE San Andreas\skins\Lighter black\CGUI.xml"

	SetOutPath "$INSTDIR\skins\Default 2023"
    File "${FILES_ROOT}\GninE San Andreas\skins\Default 2023\CGUI.is.xml"
    File "${FILES_ROOT}\GninE San Andreas\skins\Default 2023\CGUI.lnf.xml"
    File "${FILES_ROOT}\GninE San Andreas\skins\Default 2023\CGUI.png"
    File "${FILES_ROOT}\GninE San Andreas\skins\Default 2023\CGUI.xml"

	SetOutPath "$INSTDIR\skins\GWEN Blue"
    File "${FILES_ROOT}\GninE San Andreas\skins\GWEN Blue\CGUI.is.xml"
    File "${FILES_ROOT}\GninE San Andreas\skins\GWEN Blue\CGUI.lnf.xml"
    File "${FILES_ROOT}\GninE San Andreas\skins\GWEN Blue\CGUI.png"
    File "${FILES_ROOT}\GninE San Andreas\skins\GWEN Blue\CGUI.xml"

	SetOutPath "$INSTDIR\skins\GWEN Orange"
    File "${FILES_ROOT}\GninE San Andreas\skins\GWEN Orange\CGUI.is.xml"
    File "${FILES_ROOT}\GninE San Andreas\skins\GWEN Orange\CGUI.lnf.xml"
    File "${FILES_ROOT}\GninE San Andreas\skins\GWEN Orange\CGUI.png"
    File "${FILES_ROOT}\GninE San Andreas\skins\GWEN Orange\CGUI.xml"
	
	SetOutPath "$INSTDIR\GninE\cgui"
	File "${FILES_ROOT}\GninE San Andreas\GninE\cgui\Falagard.xsd"
	File "${FILES_ROOT}\GninE San Andreas\GninE\cgui\Font.xsd"
	File "${FILES_ROOT}\GninE San Andreas\GninE\cgui\GUIScheme.xsd"
	File "${FILES_ROOT}\GninE San Andreas\GninE\cgui\Imageset.xsd"
	File "${FILES_ROOT}\GninE San Andreas\GninE\cgui\pricedown.ttf"
	File "${FILES_ROOT}\GninE San Andreas\GninE\cgui\sabankgothic.ttf"
	File "${FILES_ROOT}\GninE San Andreas\GninE\cgui\sagothic.ttf"
	File "${FILES_ROOT}\GninE San Andreas\GninE\cgui\saheader.ttf"
	File "${FILES_ROOT}\GninE San Andreas\GninE\cgui\sans.ttf"
	File "${FILES_ROOT}\GninE San Andreas\GninE\cgui\unifont.ttf"

	SetOutPath "$INSTDIR\GninE\cgui\images"
	File "${FILES_ROOT}\GninE San Andreas\GninE\cgui\images\*.png"
	File "${FILES_ROOT}\GninE San Andreas\GninE\cgui\images\*.jpg"

	SetOutPath "$INSTDIR\GninE\cgui\images\radarset"
	File "${FILES_ROOT}\GninE San Andreas\GninE\cgui\images\radarset\*.png"

	SetOutPath "$INSTDIR\GninE\cgui\images\transferset"
	File "${FILES_ROOT}\GninE San Andreas\GninE\cgui\images\transferset\*.png"
	
	SetOutPath "$INSTDIR\GninE\cgui\images\serverbrowser"
	File "${FILES_ROOT}\GninE San Andreas\GninE\cgui\images\serverbrowser\*.png"

	SetOutPath "$INSTDIR\server\mods\deathmatch"
	File "${FILES_ROOT}\GninE San Andreas\server\mods\deathmatch\libmysql.dll"

SectionEnd


!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC01} $(DESC_Section1)
!insertmacro MUI_FUNCTION_DESCRIPTION_END



;====================================================================================
; UAC related functions
;====================================================================================

!macro RightsElevation un
    uac_tryagain:
    !insertmacro UAC_RunElevated
    #MessageBox mb_TopMost "0=$0 1=$1 2=$2 3=$3"
    ${Switch} $0
    ${Case} 0
        ${IfThen} $1 = 1 ${|} Quit ${|} ;we are the outer process, the inner process has done its work, we are done
        ${IfThen} $3 <> 0 ${|} ${Break} ${|} ;we are admin, let the show go on
        ${If} $1 = 3 ;RunAs completed successfully, but with a non-admin user
            MessageBox mb_IconExclamation|mb_TopMost|mb_SetForeground "This ${un}installer requires admin access, try again" /SD IDNO IDOK uac_tryagain IDNO 0
        ${EndIf}
        ;fall-through and die
    ${Case} 1223
        MessageBox mb_IconStop|mb_TopMost|mb_SetForeground "This ${un}installer requires admin privileges, aborting!"
        Quit
    ${Case} 1062
        MessageBox mb_IconStop|mb_TopMost|mb_SetForeground "Logon service not running, aborting!"
        Quit
    ${Default}
        MessageBox mb_IconStop|mb_TopMost|mb_SetForeground "Unable to elevate , error $0"
        Quit
    ${EndSwitch}
!macroend

Function DoRightsElevation
    !insertmacro RightsElevation ""
FunctionEnd

Function un.DoRightsElevation
    !insertmacro RightsElevation "un"
FunctionEnd
