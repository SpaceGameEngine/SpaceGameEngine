Write-Output "Start Testing"

$configuration=$env:CONFIGURATION
$platform=$env:PLATFORM
$solution_directory=$env:APPVEYOR_BUILD_FOLDER

Get-ChildItem $solution_directory\Test\ | ForEach-Object{
	if($_.Attributes -eq "Directory")
	{
		$test_name=$_.BaseName
		Write-Output ("Testing "+$test_name)
		cd $solution_directory\Test\$test_name
		$solution_directory\Binary\$test_name\$platform\$configuration\$test_name.exe
	}
}