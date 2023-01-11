@echo off

rem /Zi create complete debug info, 
rem /Z7 creates c 7.0 debugging info
rem /Zc:__cplusplus makes compilier emit __cplusplus macro with supported version

rem /Oi use intrinsics 
rem /Od Disables optimization, 
rem /O1 creates small code, 
rem /O2 creates fast code (enables /Oi and a bunch of other O flags)

rem /Wall show all warnings, 
rem /W4 show most warnings, 
rem /WX treat all warnings as errors

rem /Gm rebuilds more often if src changes
rem /Ge enables stack probes for functions with local variables, (deprecated... gives warning if used)
rem /Gs buffer overrun checks

rem /MD use dll version of c runtime       (add d for debug version)
rem /MT use static lib version of c rutime (add d for debug version)

rem /GR generates run time typeinfo
rem /EH uses exception handling, EHa- turns it all off
rem /FC use full path of source code files

rem /Fm create mapfile
rem /fsanitize=address helps find memory corrpution bugs, super extra slow though

rem /link start passing flags to linker
rem /OPT:REF remove functions that are never used



set disable_flags=/D_CRT_SECURE_NO_WARNINGS /GR- /EHa- /nologo /wd4201 /wd4505 /wd4100 /wd4189 /wd4456 /wd4324 /wd4005

set DEBUG_BUILD=true

set cflags=/nologo /c /MT /Gm- /FC /W4 /WX /Zc:__cplusplus %disable_flags% 
set lflags=/nologo /opt:ref /incremental:no
set imports=user32.lib Gdi32.lib winmm.lib opengl32.lib Shell32.lib Bcrypt.lib
rem Shell32.lib is for drag and drop files functionality

if %DEBUG_BUILD%==true (
	set cflags=%cflags% /DDEBUG_BUILD=1 /Zi /Od /Gs /Fm 
	set lflags=%lflags% /DEBUG
	@echo DEBUG BUILD
) else (
	set cflags=%cflags% /DDEBUG_BUILD=0 /O2 
	@echo RELEASE BUILD
)

rem set cl on if not already
WHERE cl >nul 2>nul
IF %ERRORLEVEL% NEQ 0 (
  call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall" x64
)


IF NOT EXIST ..\build mkdir ..\build

pushd ..\build
cl   %cflags% /c /Fo ..\code\win32_main.cpp ..\code\game.cpp  
link %lflags% win32_main.obj game.obj %imports%  /OUT:gjk.exe
popd
