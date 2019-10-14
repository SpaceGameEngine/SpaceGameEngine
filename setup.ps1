$input = "dependencies.txt"

foreach ($line in [System.IO.File]::ReadLines("$input")) {
    $args = $line.Split(" ")
    $clone_path = "ThirdParty/$($args[0])/source"

    if(Test-Path -Path "$clone_path" -PathType Container) {
        Echo "Directory $clone_path exists. Omit it."
    } else {
        Echo "Init $($args[0])."
        &"git" "clone" "-q" "--config" "advice.detachedHead=false" "-b" "$($args[1])" "--depth=1" "$($args[2])" "$clone_path"
    }

}