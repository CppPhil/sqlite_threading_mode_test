Push-Location .

$build_dir = 'build'

if (-Not (Test-Path -Path $build_dir)) {
  mkdir $build_dir
}

Push-Location $build_dir

Write-Output ""
Write-Output ""
$time_string = Get-Date -Format "HH:mm:ss" | Out-String
Write-Output "~~~~~~~~~~~~ Starting debug build ~~~~~~~~~~~~ $time_string"
Write-Output ""

cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug

if (-Not ($LASTEXITCODE -eq "0")) {
  Write-Output "cmake --build for Debug mode failed!"
  Pop-Location
  exit 1
}

Write-Output ""
$time_string = Get-Date -Format "HH:mm:ss" | Out-String
Write-Output "~~~~~~~~~~~~ Completed debug build ~~~~~~~~~~~~ $time_string"
Write-Output ""
Write-Output ""

Pop-Location
Push-Location $build_dir

Write-Output ""
Write-Output ""
$time_string = Get-Date -Format "HH:mm:ss" | Out-String
Write-Output "~~~~~~~~~~~~ Starting release build ~~~~~~~~~~~~ $time_string"
Write-Output ""

cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release

if (-Not ($LASTEXITCODE -eq "0")) {
  Write-Output "cmake --build for Release mode failed!"
  Pop-Location
  exit 1
}

Write-Output ""
$time_string = Get-Date -Format "HH:mm:ss" | Out-String
Write-Output "~~~~~~~~~~~~ Completed release build ~~~~~~~~~~~~ $time_string"
Write-Output ""
Write-Output ""

Pop-Location
exit 0
