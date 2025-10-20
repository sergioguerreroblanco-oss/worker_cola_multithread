# Documentation

This project uses **Doxygen** to generate API documentation from the annotated source code.

## Requirements
- Doxygen
- (Optional) Graphviz for diagrams
- (Linux only) LaTeX toolchain (TeX Live + make) to build PDF

## On Windows (PowerShell):

1. Install Doxygen for Windows (`https://www.doxygen.nl/download.html`)
	
	During setup, check “Add doxygen to system PATH

2. (Optional) Install Graphviz (`https://graphviz.org/download/`) and add its bin/ folder to PATH.

3.Open PowerShell in the project root and run:

```PowerShell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\scripts\generate_docs.ps1
```

4. Open documentation in your browser:
```PowerShell
start docs\html\index.html
```

## On Debian/Ubuntu (Linux / WSL):

```bash
sudo apt update
sudo apt install -y doxygen graphviz texlive-latex-base texlive-fonts-recommended texlive-latex-extra make
sudo apt install doxygen graphviz
```

Generate

```bash
chmod +x scripts/generate_docs.sh
./scripts/generate_docs.sh
```

## Output

### Windows
Only HTML + LaTeX sources are generated. The PDF is not produced on Windows.

### Debian/Ubuntu Linux
HTML: docs/html/index.html

PDF: docs/latex/refman.pdf
