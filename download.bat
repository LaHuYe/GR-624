@echo off
REM Keil Command URL https://developer.arm.com/documentation/101407/0539/Command-Line?lang=en
@REM keil5 downlo
D:\software\Keil_V528\UV4\UV4.exe -f D:\Lebaicong\Project\GR-624\GR-624-Bat\MDK-ARM\GR-624-Bat.uvprojx -o Prg_Output.txt
type D:\Lebaicong\Project\GR-624\GR-624-Bat\MDK-ARM\Prg_Output.txt

powershell -Command "$content = Get-Content 'D:\Lebaicong\Project\GR-624\GR-624-Bat\MDK-ARM\Prg_Output.txt' -Encoding UTF8; foreach ($line in $content) { if ($line -match 'warning') { Write-Host $line -ForegroundColor Cyan } elseif ($line -match 'error') { Write-Host $line -ForegroundColor Red } else { Write-Host $line } }"
