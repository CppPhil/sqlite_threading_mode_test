function Build($build_dir, $enable_mutex, $build_type) {
  if (-Not (Test-Path -Path $build_dir)) {
      mkdir $build_dir
  }

  Push-Location $build_dir
  $cpuCores = Get-CimInstance Win32_Processor | Measure-Object -Property NumberOfCores -Sum | Select-Object -ExpandProperty Sum
  cmake -DCMAKE_BUILD_TYPE=$build_type -DENABLE_MUTEX:BOOL=$enable_mutex ..
  cmake --build . --config $build_type --parallel $cpuCores

  if (-Not ($LASTEXITCODE -eq "0")) {
      Pop-Location
      exit 1
  }

  Pop-Location
}

$scriptDirectory = $PSScriptRoot
Push-Location $scriptDirectory
$build_dir_multi_threaded = 'build_multi_threaded'
$build_dir_mutex = 'build_mutex'
Build $build_dir_multi_threaded $false "Debug"
Build $build_dir_multi_threaded $false "Release"
Build $build_dir_mutex $true "Debug"
Build $build_dir_mutex $true "Release"
Pop-Location
exit 0

