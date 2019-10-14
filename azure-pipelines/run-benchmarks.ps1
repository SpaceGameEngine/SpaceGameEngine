cd Benchmark

$arch = $args[0]
$config = $args[1]

(Get-ChildItem -Directory) | foreach{
    $benchmarkName = $_.Name
    cd $benchmarkName
    &"..\..\Binary\$benchmarkName\$arch\$config\$benchmarkName.exe" "--benchmark_format=json" "--benchmark_out=$benchmarkName-REPORT.json"
    cd ..
}

cd ..