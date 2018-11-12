cd ..\..\..\depot_tools\webrtc-checkout\src

set DEPOT_TOOLS_WIN_TOOLCHAIN=0

call gn clean out/DebugX64

call gn gen out/DebugX64 --ide="vs2017" --args="is_debug=true target_cpu=\"x64\" is_clang=false"

call ninja -C out/DebugX64

pause