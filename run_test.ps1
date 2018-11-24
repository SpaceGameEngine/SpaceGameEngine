Write-Output "Start Testing"

$configuration=$env:CONFIGURATION
$platform=$env:PLATFORM
$solution_directory=$env:APPVEYOR_BUILD_FOLDER

#test
Write-Output $solution_directory
Get-ChildItem $solution_directory\Test\ | ForEach-Object
{
    Write-Output $_.BaseName
	if($_.Attributes -eq "Directory")
	{
		$test_name=$_.BaseName
		Write-Output ("Testing "+$test_name)
		copy $solution_directory\Binary\$test_name\$platform\$configuration\$test_name.exe $solution_directory\Test\$test_name\$test_name.exe
		&$solution_directory\Test\$test_name\$test_name.exe
	}
}