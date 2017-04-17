# Selfie
* Self-Modifying Code Unpacking Tool using DynamoRIO
* More Info: http://breakingmalware.com/

# Build
* Download latest package of DynamoRIO from https://github.com/DynamoRIO/dynamorio/wiki/Downloads
* Unzip DynamoRIO package into Selfie's folder
* Libs and include folders are already configured in Selfie's project.
* build

# Usage
* drrun.exe -client "selfie.dll" 1 "" -- "malware.exe"

# Tested Enviroment
* DynamoRIO latest version 5.1.0-RC1.
* Windows 7 32 and 64 bit.

# Malware samples used in blog post
* ed3d622c54b474c6caef540a3147731a1b2c7d4a7563b97731880bb15305d47d (Xswkit)
* 4fda5e7e8e682870e993f97ad26ba6b2 (Win32/Caphaw (Shylock))

# Warning
* Use virtualized environment to run malware.exe

# Authors
* enSilo research
