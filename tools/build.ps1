[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release')][string[]]$Configuration = @('Debug', 'Release'),
    [ValidateRange(1,64)][int]$Jobs = 6,
    [switch]$SkipDependencies
)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
Push-Location $projectRoot
try {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    $vsPath = & $vswhere -latest -version '[18.0,19.0)' -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (!$vsPath) { throw 'Visual Studio 2026 mit Desktopentwicklung mit C++ und Windows SDK wird benötigt.' }
    $msbuild = Join-Path $vsPath 'MSBuild\Current\Bin\MSBuild.exe'
    if (!$SkipDependencies) {
        if (!(Get-Command cmake -ErrorAction SilentlyContinue)) { throw 'CMake >= 4.2 wird benötigt.' }
        $wxVersion = '3.2.12'
        $archive = "third_party\wxWidgets-$wxVersion.7z"
        $wxSource = "third_party\wxWidgets-$wxVersion"
        New-Item -ItemType Directory -Force third_party,build | Out-Null
        if (!(Test-Path "$wxSource\CMakeLists.txt")) {
            if (!(Test-Path -LiteralPath $archive)) {
                Invoke-WebRequest "https://github.com/wxWidgets/wxWidgets/releases/download/v$wxVersion/wxWidgets-$wxVersion.7z" -OutFile $archive
            }
            $expectedHash = '97EC67B9B87DBC0338C8305C0F83422A33C8D70AB3A3CD21F6D507950D338AF1'
            if ((Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash -ne $expectedHash) { throw 'wxWidgets-Archiv: SHA256 stimmt nicht überein.' }
            New-Item -ItemType Directory -Force $wxSource | Out-Null
            & tar -xf $archive -C $wxSource
            if ($LASTEXITCODE) { throw 'wxWidgets konnte nicht entpackt werden.' }
        }
        & cmake -S $wxSource -B build/wxWidgets -G 'Visual Studio 18 2026' -A x64 `
            -DwxBUILD_SHARED=OFF -DwxBUILD_USE_STATIC_RUNTIME=OFF -DwxBUILD_SAMPLES=OFF `
            -DwxBUILD_TESTS=OFF -DwxBUILD_DEMOS=OFF -DwxBUILD_PRECOMP=ON -DwxBUILD_INSTALL=OFF `
            -DwxBUILD_MSVC_MULTIPROC=ON -DwxUSE_WEBVIEW=OFF -DwxUSE_MEDIACTRL=OFF `
            -DwxUSE_LIBTIFF=OFF -DwxUSE_LIBJPEG=OFF -DwxUSE_LIBPNG=builtin -DwxUSE_REGEX=OFF -DwxUSE_EXPAT=OFF
        if ($LASTEXITCODE) { throw 'wxWidgets-Konfiguration fehlgeschlagen.' }
    }
    foreach ($config in $Configuration) {
        if (!$SkipDependencies) {
            & cmake --build build/wxWidgets --config $config --target wxcore --parallel $Jobs
            if ($LASTEXITCODE) { throw "wxWidgets $config fehlgeschlagen." }
        }
        & $msbuild JetChess.sln "/p:Configuration=$config" /p:Platform=x64 /m /v:minimal /nologo
        if ($LASTEXITCODE) { throw "JetChess $config fehlgeschlagen." }
    }
} finally { Pop-Location }
