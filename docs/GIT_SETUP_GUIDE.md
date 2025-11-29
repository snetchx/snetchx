# Git Setup and GitHub Upload Guide

## Step 1: Install Git

1. **Download Git for Windows**
   - Go to: https://git-scm.com/download/win
   - Click "Click here to download" (64-bit version)
   - Run the downloaded installer (Git-2.x.x-64-bit.exe)

2. **Installation Settings**
   - Click "Next" through all the screens
   - **IMPORTANT**: Make sure "Git from the command line and also from 3rd-party software" is selected
   - Keep all default settings
   - Click "Install"
   - When done, click "Finish"

3. **Verify Installation**
   - Close ALL terminal/PowerShell windows
   - Open a NEW PowerShell window
   - Type: `git --version`
   - You should see: `git version 2.x.x`

---

## Step 2: Configure Git (First Time Only)

Open PowerShell and run these commands (replace with your info):

```powershell
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"
```

---

## Step 3: Upload to GitHub

### Method A: Using Command Line (After Git is Installed)

1. Open PowerShell in your project folder
2. Run these commands ONE BY ONE:

```powershell
# Navigate to project folder
cd "c:\Users\frsnq\OneDrive\Desktop\UTEM\Sem3\Workshop 1\restaurant testing"

# Initialize Git repository
git init

# Add all files
git add .

# Commit files
git commit -m "Initial commit: Restaurant Management System"

# Add your GitHub repository
git remote add origin https://github.com/snetchx/snetchx.git

# Push to GitHub
git branch -M main
git push -u origin main
```

3. When prompted, enter your GitHub username and password (or Personal Access Token)

---

### Method B: Using GitHub Desktop (EASIER!)

1. **Download GitHub Desktop**
   - Go to: https://desktop.github.com/
   - Download and install

2. **Sign In**
   - Open GitHub Desktop
   - Sign in with your GitHub account

3. **Add Your Repository**
   - Click "File" → "Add Local Repository"
   - Browse to: `c:\Users\frsnq\OneDrive\Desktop\UTEM\Sem3\Workshop 1\restaurant testing`
   - Click "Add Repository"
   
   OR
   
   - Click "File" → "Clone Repository"
   - Paste: `https://github.com/snetchx/snetchx.git`
   - Choose where to save it
   - Then COPY all your project files into that folder

4. **Commit and Push**
   - You'll see all your files listed
   - Add a commit message: "Initial commit: Restaurant Management System"
   - Click "Commit to main"
   - Click "Push origin"

---

## Your GitHub Repository
https://github.com/snetchx/snetchx.git

---

## Troubleshooting

### Git command not recognized?
- Make sure you closed ALL terminal windows after installing Git
- Restart your computer if needed

### Authentication Failed?
- You may need a Personal Access Token instead of password
- Go to: GitHub.com → Settings → Developer settings → Personal access tokens → Tokens (classic)
- Generate new token with "repo" permissions
- Use this token as your password

### Files Not Showing Up?
- Make sure you're in the correct folder
- Check if .gitignore is blocking files


