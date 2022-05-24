@echo off
set PLATFORM=windows
set VARYINGDEF="Resources\Shaders\varying.def.sc"
set INCLUDE=Resources\Shaders\Include\

if not exist Resources\Shaders\bin\ mkdir Resources\Shaders\bin

setlocal enabledelayedexpansion

if [%1]==[] (
    echo Command requires type parameter
    goto:End
)

for /R %%f in (Resources\Shaders\*.sc) do (
    call :check %%f %1
)
goto:End

@REM Check file for exclusion
:check [%1 - param, %2 - param]
    set replaced=%1
    set replaced=%replaced:varying=%
    set replaced=%replaced:bin=%

    if x%replaced%==x%1 call :process %1 %2
    goto:eof

@REM Compile shaders and output to Shaders\bin\[shaderPath]
:process [%1 - param, %2 - param]
    set profile=%2
    set output=%1
    set output=%output:Shaders=Shaders\bin\!profile!%
    for %%a in (%output%) do (
        set output=%%~dpa
        set file=%%~nxa
        set fileName=%%~na
    )
    if not x%file:vs=%==x%file% (set type=vertex) else (
        if not x%file:fs=%==x%file% (set type=fragment) else (
            if not x%file:cs=%==x%file% (set type=compute) else (
                echo Unable to determine type of shader at %1
                echo x%file:vs=%==x%file%
                echo x%file:fs=%==x%file%
                echo x%file:cs=%==x%file%
                goto:eof
    )))

    if not exist %output% mkdir %output%

    if not x%profile:s_=%==x%profile% (
        if %type%==vertex set profile=v%profile%
        if %type%==fragment set profile=p%profile%
        if %type%==compute set profile=c%profile%
    )

    echo Compiling %file%
    shadercRelease -f %1 -o %output%%filename%.bin --type %type% -i %INCLUDE% --platform %PLATFORM% --varyingdef %VARYINGDEF% --profile %profile%
    goto:eof

:End
pause