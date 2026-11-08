[CmdletBinding()]
param([ValidateSet('Debug','Release')][string]$Configuration = 'Release')
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
Push-Location $projectRoot
try {
    & cmake -S tests -B build/tests -G 'Visual Studio 18 2026' -A x64
    if ($LASTEXITCODE) { throw 'Test-Konfiguration fehlgeschlagen.' }
    & cmake --build build/tests --config $Configuration
    if ($LASTEXITCODE) { throw 'Test-Build fehlgeschlagen.' }
    & python tests/regression.py --configuration $Configuration
    if ($LASTEXITCODE) { throw 'Regressionstest fehlgeschlagen.' }
    & "build/tests/$Configuration/engine_probe.exe" --threaded
    if ($LASTEXITCODE) { throw 'Thread-/Abbruchtest fehlgeschlagen.' }
    & "build/tests/$Configuration/fen_checks.exe"
    if ($LASTEXITCODE) { throw 'FEN-Pruefung fehlgeschlagen.' }
    & "build/tests/$Configuration/ep_checks.exe"
    if ($LASTEXITCODE) { throw 'EP-Pruefung fehlgeschlagen.' }
    & "build/tests/$Configuration/hash_checks.exe"
    if ($LASTEXITCODE) { throw 'Hash-Grenzpruefung fehlgeschlagen.' }
} finally { Pop-Location }
