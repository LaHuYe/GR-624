@echo off
cd /D D:\Lebaicong\Project\GR-624\GR-624-Bat

REM 从 Git 获取最新的提交哈希
for /f %%i in ('git rev-parse HEAD') do set COMMIT_HASH=%%i
echo COMMIT_HASH: %COMMIT_HASH%

REM 更新 application.h 文件
powershell -Command "$content = (Get-Content Inc\application.h -Encoding UTF8) -replace '#define COMMIT_HASH\s+\"[a-zA-Z0-9]+\"', ('#define COMMIT_HASH \"%COMMIT_HASH%\"'); Set-Content -Path Inc\application.h -Value $content -Encoding UTF8"

