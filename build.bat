@echo off
REM Keil Command URL https://developer.arm.com/documentation/101407/0539/Command-Line?lang=en
REM Define path variables
set "keil_path=D:\software\Keil_V528\UV4" 
set "project_root=D:\Lebaicong\Project\GR-624\GR-624-Motor"
set "project_path=%project_root%\MDK-ARM"
set "build_output=%project_path%\Build_Output.txt"
set "uvprojx=%project_path%\GR-624-Motor.uvprojx"

REM Call the update script
call "%project_root%\commit_hash_update.bat"

REM Set default build parameter to -b
set "build_param=-b"

REM Check if -all parameter exists
if "%1"=="-all" (
    set "build_param=-r"
)

REM Keil build
"%keil_path%\UV4.exe" -j0 %build_param% "%uvprojx%" -o "%build_output%"

REM Use PowerShell to output the build result
powershell -Command "$content = Get-Content '%build_output%' -Encoding UTF8; foreach ($line in $content) { if ($line -match 'warning') { Write-Host $line -ForegroundColor Cyan } elseif ($line -match 'error') { Write-Host $line -ForegroundColor Red } else { Write-Host $line } }"

