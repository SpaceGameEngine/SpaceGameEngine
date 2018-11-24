#you can just attend new test project name
[String[]]$test_projects=@("TestCatch2")

function run_test($configuration,$platform)
{
	for($i=0;$i -le $test_projects.count;$i++)
	{
		copy $env:APPVEYOR_BUILD_FOLDER\Binary\$test_projects[$i]\$platform\$configuration\$test_projects[$i].exe $env:APPVEYOR_BUILD_FOLDER\Test\$test_projects[$i]\$test_projects[$i].exe
		&$env:APPVEYOR_BUILD_FOLDER\Test\$test_projects[$i]\$test_projects[$i].exe
	}
}
run_test($env:CONFIGURATION,$env:PLATFORM)