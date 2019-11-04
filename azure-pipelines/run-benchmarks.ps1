cd Benchmark

$arch = $args[0]
$config = $args[1]

(Get-ChildItem -Directory) | foreach{
    $benchmarkName = $_.Name
    cd $benchmarkName
    &"..\..\Binary\$benchmarkName\$arch\$config\$benchmarkName.exe"
    cd ..
}

cd ..
