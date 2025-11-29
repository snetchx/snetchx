@echo off
echo ========================================
echo PUSH TO GITHUB WITH TOKEN
echo ========================================
echo.

REM Check if Git is installed
git --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Git is not installed!
    echo.
    echo Please install Git first:
    echo Download from: https://git-scm.com/download/win
    echo Then RESTART your computer and run this script again.
    echo.
    pause
    exit /b 1
)

echo [SUCCESS] Git is installed!
echo.

REM Prompt for GitHub credentials
set /p username="Enter your GitHub username (snetchx): "
if "%username%"=="" set username=snetchx

set /p email="Enter your email: "
if "%email%"=="" (
    echo Email cannot be empty!
    pause
    exit /b 1
)

set /p token="Paste your Personal Access Token: "
if "%token%"=="" (
    echo Token cannot be empty!
    pause
    exit /b 1
)

echo.
echo Configuring Git...
git config --global user.name "%username%"
git config --global user.email "%email%"

echo.
echo Initializing repository...
git init

echo.
echo Adding files...
git add .

echo.
echo Committing files...
git commit -m "Initial commit: Restaurant Management System with C++ and MySQL"

echo.
echo Adding remote repository...
git remote add origin https://github.com/snetchx/snetchx.git 2>nul
if errorlevel 1 (
    git remote set-url origin https://github.com/snetchx/snetchx.git
)

echo.
echo Pushing to GitHub...
git branch -M main

REM Use token in URL for authentication
git push -u https://%username%:%token%@github.com/snetchx/snetchx.git main

if errorlevel 1 (
    echo.
    echo [ERROR] Push failed!
    echo Please check:
    echo 1. Your token is valid
    echo 2. Token has 'repo' permission
    echo 3. Your internet connection
    echo.
) else (
    echo.
    echo ========================================
    echo [SUCCESS] Pushed to GitHub!
    echo ========================================
    echo.
    echo View your repository at:
    echo https://github.com/snetchx/snetchx
    echo.
)

pause


