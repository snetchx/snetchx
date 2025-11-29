# 🚀 How to Upload Your Restaurant System to GitHub

## ✅ Using GitHub Desktop (EASIEST - You Already Have This!)

### Step 1: Open GitHub Desktop
- Launch GitHub Desktop

### Step 2: Sign In (If Not Already)
- Click **File** → **Options** → **Accounts**
- Click **Sign in** to GitHub.com
- Your browser will open - sign in with your GitHub account
- Click **Authorize desktop**

### Step 3: Clone Your Repository
1. In GitHub Desktop, click **File** → **Clone Repository**
2. Click the **URL** tab
3. Paste this URL: `https://github.com/snetchx/snetchx.git`
4. Choose where to save it (e.g., `C:\Users\frsnq\Documents\GitHub\snetchx`)
5. Click **Clone**

### Step 4: Copy Your Project Files
1. Open File Explorer
2. Navigate to your current project:
   ```
   c:\Users\frsnq\OneDrive\Desktop\UTEM\Sem3\Workshop 1\restaurant testing
   ```

3. **Select ALL files** (Press Ctrl+A):
   - main.cpp
   - DatabaseConnection.h
   - DatabaseConnection.cpp
   - AdminModule.h
   - StaffModule.h
   - MenuModule.h
   - TableModule.h
   - OrderModule.h
   - BillingModule.h
   - restaurant_database.sql
   - All .dll files
   - All folders (restaura.7542d8b8, x64, plugin)
   - Visual Studio project files (.vcxproj, .sln)

4. **Copy** (Press Ctrl+C)

5. Navigate to the cloned repository folder (from Step 3)

6. **Paste** all files (Press Ctrl+V)

### Step 5: Commit Your Changes
1. Go back to **GitHub Desktop**
2. You'll see all your files listed on the left side
3. At the bottom left, fill in:
   - **Summary:** `Restaurant Management System - Initial Upload`
   - **Description:** 
     ```
     C++ Restaurant Management System with MySQL Database
     
     Features:
     - Admin & Staff Login
     - Menu Management
     - Table Management
     - Order Processing
     - Billing & Payment
     - Daily Sales Reports
     
     Technologies: C++, MySQL, MySQL Connector/C++
     ```

4. Click the blue **"Commit to main"** button

### Step 6: Push to GitHub
1. Click **"Push origin"** button at the top (or press Ctrl+P)
2. Wait for the upload to complete

### Step 7: Verify Upload ✅
1. Open your browser
2. Go to: **https://github.com/snetchx/snetchx**
3. Refresh the page
4. You should see all your files!

---

## 🔧 Alternative: Install Git + Use Command Line

If you want to use Git commands instead:

### 1. Install Git
- Download: https://git-scm.com/download/win
- Run installer with default settings
- **Restart your computer**

### 2. Open NEW PowerShell and Run:
```powershell
cd "c:\Users\frsnq\OneDrive\Desktop\UTEM\Sem3\Workshop 1\restaurant testing"

git config --global user.name "snetchx"
git config --global user.email "your-email@example.com"

git init
git add .
git commit -m "Restaurant Management System - Initial Upload"
git remote add origin https://github.com/snetchx/snetchx.git
git branch -M main
git push -u origin main
```

When asked for password, use your **Personal Access Token** (not GitHub password!)

---

## 📋 Your Repository
https://github.com/snetchx/snetchx

---

## ❓ Need Help?
If you get stuck at any step, let me know which step number!

