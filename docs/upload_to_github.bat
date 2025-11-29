@echo off
echo ========================================
echo RESTAURANT MANAGEMENT SYSTEM
echo Upload to GitHub Script
echo ========================================
echo.

REM Check if Git is installed
git --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Git is not installed!
    echo.
    echo Please install Git first:
    echo 1. Go to: https://git-scm.com/download/win
    echo 2. Download and install Git
    echo 3. Restart this script
    echo.
    pause
    exit /b 1
)

echo [SUCCESS] Git is installed!
echo.

REM Check if Git is already initialized
if exist ".git" (
    echo Git repository already initialized.
) else (
    echo Initializing Git repository...
    git init
    if errorlevel 1 (
        echo [ERROR] Failed to initialize Git repository!
        pause
        exit /b 1
    )
    echo [SUCCESS] Git repository initialized!
)

echo.
echo Checking Git configuration...
git config user.name >nul 2>&1
if errorlevel 1 (
    echo.
    echo Please configure your Git identity:
    set /p username="Enter your name: "
    set /p email="Enter your email: "
    git config --global user.name "%username%"
    git config --global user.email "%email%"
    echo [SUCCESS] Git configured!
)

echo.
echo Adding files to Git...
git add .
if errorlevel 1 (
    echo [ERROR] Failed to add files!
    pause
    exit /b 1
)
echo [SUCCESS] Files added!

echo.
echo Committing files...
git commit -m "Initial commit: Restaurant Management System with C++ and MySQL"
if errorlevel 1 (
    echo [WARNING] Nothing to commit or commit failed
)

echo.
echo Adding GitHub remote...
git remote add origin https://github.com/snetchx/snetchx.git 2>nul
if errorlevel 1 (
    echo Remote already exists, updating...
    git remote set-url origin https://github.com/snetchx/snetchx.git
)

echo.
echo Pushing to GitHub...
echo You may be prompted for your GitHub credentials.
echo.
git branch -M main
git push -u origin main

if errorlevel 1 (
    echo.
    echo [ERROR] Push failed!
    echo.
    echo Common issues:
    echo 1. Check your GitHub username and password
    echo 2. You may need a Personal Access Token instead of password
    echo 3. Go to: GitHub Settings - Developer settings - Personal access tokens
    echo.
) else (
    echo.
    echo ========================================
    echo [SUCCESS] Successfully uploaded to GitHub!
    echo ========================================
    echo.
    echo Your repository: https://github.com/snetchx/snetchx.git
    echo.
)

pause


