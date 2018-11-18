set SolutionDir=%~1
set TargetDir=%~2
set Configuration=%3

echo Copying WebRtcNative libs to %TargetDir%

robocopy "%SolutionDir%\WebRtcNative\%Configuration%"		"%TargetDir%\x86"	"WebRtcNative.dll"
robocopy "%SolutionDir%\WebRtcNative\%Configuration%_x64"	"%TargetDir%\x64"	"WebRtcNative.dll"

exit /B 0