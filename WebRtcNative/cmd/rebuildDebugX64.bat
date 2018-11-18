cd ..\..\..\depot_tools\webrtc-checkout\src

set DEPOT_TOOLS_WIN_TOOLCHAIN=0

call gn clean out/DebugX64

call gn gen out/DebugX64 --args="target_cpu=\"x64\" rtc_use_h264=true use_lld=false"

call ninja -C out/DebugX64

pause