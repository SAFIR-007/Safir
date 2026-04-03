# Comprehensive Guide: Publishing to GitHub

This guide provides detailed steps to upload your **Wrath of God** project to a public GitHub repository.

---

## 1. Prerequisites
- **GitHub Account**: Log in to [github.com](https://github.com).
- **Git Installed**: If you don't have Git, download and install it from [git-scm.com](https://git-scm.com/download/win).
- **GitHub Desktop (Optional but Recommended)**: If you prefer a visual interface, download [desktop.github.com](https://desktop.github.com/).

---

## 2. Create Your GitHub Repository
1. On GitHub, click the **+** icon (top right) and select **New repository**.
2. **Repository name**: `Wrath-of-God` (or any name you like).
3. **Public**: Select this so others can see your code.
4. **Initialize this repository**: 
   - **DO NOT** check "Add a README file".
   - **DO NOT** check "Add .gitignore".
   - **DO NOT** check "Choose a license".
5. Click **Create repository**.

---

## 3. Connect Your Local Code to GitHub
Open **PowerShell** or **Git Bash** in your project folder (`c:\Users\tasee\Downloads\Compressed\Project2(4)\Project2`) and run these commands:

### A. Initialize Git
```bash
git init
```

### B. Add Your Files
This prepares all your code and assets for the first upload.
```bash
git add .
```

### C. Create Your First Commit
```bash
git commit -m "Initial commit: Complete game code and assets"
```

### D. Connect to GitHub
Copy the URL from your GitHub repository page (e.g., `https://github.com/YOUR_USERNAME/Wrath-of-God.git`) and use it here:
```bash
git remote add origin https://github.com/YOUR_USERNAME/Wrath-of-God.git
```

### E. Push Your Code
```bash
git branch -M main
git push -u origin main
```

---

## 4. Authentication (Crucial)
When you run `git push`, a window will pop up asking you to log in.
- **Option 1**: Log in via your browser (the easiest way).
- **Option 2 (Personal Access Token)**: If the browser login fails, you will need a token because GitHub no longer accepts account passwords for Git commands.
  1. Go to GitHub **Settings** → **Developer settings** → **Personal access tokens** → **Tokens (classic)**.
  2. Click **Generate new token (classic)**.
  3. Give it a name, set expiration, and check the `repo` box.
  4. **Copy the token** (it only shows once!) and use it as your password when Git asks.

---

## 5. Summary of Files Included
Your repository will contain:
- `iMain.cpp`: The core game logic.
- `iGraphics.h`: The engine.
- `Assets/`: Images (Stage, Player, Enemy, etc.).
- `Audios/`: Sound effects.
- `CODE_ARCHITECTURE.md`: The guide I created for you.
- `GAME_GUIDE.md`: The gameplay instructions.

*Note: Large build files like `Debug/` and `*.sdf` are ignored by the `.gitignore` I created for you to keeps your repo clean.*
