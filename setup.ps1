$input = "dependencies.txt"

foreach ($line in [System.IO.File]::ReadLines("$input")) {
    $args = $line.Split(" ")
    &"git" "clone" "-b" "$($args[1])" "--depth=1" "$($args[2])" "ThirdParty/$($args[0])/source"
}