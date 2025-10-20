param(
    [string]$preset = "release"
)

Write-Output "Building project with preset: $preset"

# Try to locate cmake
$cmakeCmd = Get-Command cmake -ErrorAction SilentlyContinue

if (-not $cmakeCmd) {
    # Try Visual Studio bundled cmake
    $vsCmake = "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    if (Test-Path $vsCmake) {
        $cmakeCmd = $vsCmake
    } else {
        Write-Error "CMake not found in PATH or Visual Studio installation."
        exit 1
    }
} else {
    $cmakeCmd = $cmakeCmd.Source
}

# Run build
& $cmakeCmd --preset $preset
& $cmakeCmd --build --preset $preset

Write-Output "Build completed."
Write-Output "Binary available at: build/$preset/worker_cola_multithread[.exe]"
