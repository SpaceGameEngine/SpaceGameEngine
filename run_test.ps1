$configuration=$env:CONFIGURATION
$platform=$env:PLATFORM
$solution_directory=$env:APPVEYOR_BUILD_FOLDER

Get-ChildItem $solution_directory\Test\ | ForEach-Object
{
Write-Host "Testing"$_
	if($_.Attributes -eq "Directory")
	{
		Write-Host "Testing"$_
		copy $solution_directory\Binary\$_\$platform\$configuration\$_.exe $solution_directory\Test\$_\$_.exe
		&$solution_directory\Test\$_\$_.exe
	}
}