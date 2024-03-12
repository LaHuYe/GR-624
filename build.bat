@echo off
REM Keil Command URL https://developer.arm.com/documentation/101407/0539/Command-Line?lang=en
call D:\Lebaicong\Project\GR-624\GR-624-Bat\commit_hash_update.bat

REM Set default parameter to -b
set "all_param=-b"

REM Check if -all parameter exists
if "%1"=="-all" (
    set "all_param=-r"
)

@REM keil5 build
D:\software\Keil_V528\UV4\UV4.exe -j0 %all_param% D:\Lebaicong\Project\GR-624\GR-624-Bat\MDK-ARM\GR-624-Bat.uvprojx -o Build_Output.txt
type D:\Lebaicong\Project\GR-624\GR-624-Bat\MDK-ARM\Build_Output.txt

powershell -Command "$content = Get-Content 'D:\Lebaicong\Project\GR-624\GR-624-Bat\MDK-ARM\Build_Output.txt' -Encoding UTF8; foreach ($line in $content) { if ($line -match 'warning') { Write-Host $line -ForegroundColor Cyan } elseif ($line -match 'error') { Write-Host $line -ForegroundColor Red } else { Write-Host $line } }"

