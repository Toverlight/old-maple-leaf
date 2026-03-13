param(
    [Parameter(Mandatory = $true)]
    [string]$CommandLine
)

$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path $vswhere)) {
    throw "vswhere.exe not found: $vswhere"
}

$vcvars = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find 'VC\Auxiliary\Build\vcvars64.bat' | Select-Object -First 1
if (-not $vcvars) {
    throw 'vcvars64.bat not found.'
}

$resolvedCommand = '"' + $vcvars + '" && ' + $CommandLine
& cmd.exe /d /c $resolvedCommand

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}