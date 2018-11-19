cd ..\..\..\depot_tools\webrtc-checkout\src

set DEPOT_TOOLS_WIN_TOOLCHAIN=0

call gn clean out/Debug

call gn gen out/Debug --args="target_cpu=\"x86\" rtc_use_h264=true use_lld=false"

call ninja -C out/Debug

pause