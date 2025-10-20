# scripts/generate_docs.ps1
$ProjectRoot = Split-Path -Parent $PSScriptRoot
Set-Location "$ProjectRoot/docs"

if (-not (Get-Command doxygen -ErrorAction SilentlyContinue)) {
    Write-Error "Error: doxygen not found. Please install it first."
    exit 1
}

Write-Output "Generating documentation..."
doxygen Doxyfile

# Compile PDF (if LaTeX Makefile exists and make is available)
if (Test-Path "docs\latex\Makefile") {
    Write-Output "Compiling LaTeX to PDF..."
    Push-Location docs\latex
    make
    Pop-Location
}

Write-Output "Documentation generated in docs/html (and docs/latex/refman.pdf if LaTeX is available)."
