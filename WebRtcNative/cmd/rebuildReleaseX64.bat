cd ..\..\..\depot_tools\webrtc-checkout\src

set DEPOT_TOOLS_WIN_TOOLCHAIN=0

call gn clean out/ReleaseX64

call gn gen out/ReleaseX64 --args="target_cpu=\"x64\" rtc_use_h264=true use_lld=false is_debug=false symbol_level=0"

call ninja -C out/ReleaseX64

pause