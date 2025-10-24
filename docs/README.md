# 📘 Project Documentation

This project uses **[Doxygen](https://www.doxygen.nl/)** to generate complete API documentation directly from the annotated C++ source code.

Both **HTML** and **PDF** formats are supported.

---

## 🧩 Requirements

| Component | Purpose |
|------------|----------|
| **Doxygen** | Main documentation generator |
| **Graphviz** | Generates class and call graphs (recommended) |
| **TeX Live + Make** | Required only to generate PDF output (Linux only) |

---

## 🪟 Windows (PowerShell)

1. **Install Doxygen**

   👉 [Download Doxygen for Windows](https://www.doxygen.nl/download.html)  
   During setup, enable:  
   > ☑ “Add Doxygen to system PATH”

2. **Install Graphviz**

   👉 [Download Graphviz](https://graphviz.org/download/)  
   Add its `bin/` folder to your `PATH`.

3. **Generate Documentation**

   ```powershell
   Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
   .\scripts\generate_docs.ps1
   ```

4. **Open in your browser**

   ```powershell
    start docs\html\index.html
   ```

🧾 Note: On Windows, only HTML and LaTeX sources are generated.
PDF output is disabled by default.

---

## 🐧 Debian / Ubuntu (Linux / WSL)
1️. **Install dependencies**

```bash
sudo apt update
sudo apt install -y doxygen graphviz make \
    texlive-latex-base texlive-fonts-recommended \
    texlive-latex-extra texlive-fonts-extra
```

2️. **Generate documentation**

```bash
chmod +x scripts/generate_docs.sh
./scripts/generate_docs.sh
```

3️. **Output locations**

| Type | Path |
|------------|----------|
|HTML | docs/html/index.html|
|PDF | docs/latex/refman.pdf|

---

## 🚀 Continuous Integration (GitHub Actions)

The GitHub Actions workflow in .github/workflows/docs.yml automatically:

    1. Installs dependencies and runs Doxygen.

    2. Builds both HTML and PDF outputs.

    3. Uploads the documentation as an artifact.

    4. Publishes the HTML version to GitHub Pages.

After each push to main, you can access the documentation at:

👉 [HTML documentation](https://sergioguerreroblanco-oss.github.io/worker_cola_multithread/)

--- 

## 🧭 Tips

- To rebuild everything from scratch, delete the docs/html and docs/latex folders.

- If you modify Doxygen configuration, update docs/Doxyfile accordingly.

- Check the build logs in build/Testing/Temporary/LastTest.log if tests or documentation fail.

---

## 📚 Generated outputs summary

| Format | Contents | Typical use |
|------------|----------|----------|
| HTML | Full navigable API reference with graphs | Online documentation |
| LaTeX + PDF | Printable manual including diagrams | Technical reports or offline reference |

---

© 2025 Sergio Guerrero Blanco — All rights reserved.