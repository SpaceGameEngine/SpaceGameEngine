cd Test

$arch = $args[0]
$config = $args[1]

(Get-ChildItem -Directory) | foreach {
    $testName = $_.Name
    cd $testName
    &".\Binary\$testName\$arch\$config\$testName.exe" "--gtest_output=xml:$testName-REPORT.xml"
    cd ..
}

cd ..