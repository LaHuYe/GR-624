@echo off
REM Keil Command URL https://developer.arm.com/documentation/101407/0539/Command-Line?lang=en
REM Define path variables
set "keil_path=D:\software\Keil_V528\UV4"
set "project_path=D:\Lebaicong\Project\GR-624\GR-624-Motor\MDK-ARM"
set "uvprojx=%project_path%\GR-624-Motor.uvprojx"
set "output_file=%project_path%\Prg_Output.txt"

REM Download Keil5 project
"%keil_path%\UV4.exe" -f "%uvprojx%" -o "%output_file%"

REM Display the content of the output file
type "%output_file%"

REM Use PowerShell to output the compilation result
powershell -Command "$content = Get-Content '%output_file%' -Encoding UTF8; foreach ($line in $content) { if ($line -match 'warning') { Write-Host $line -ForegroundColor Cyan } elseif ($line -match 'error') { Write-Host $line -ForegroundColor Red } else { Write-Host $line } }"

