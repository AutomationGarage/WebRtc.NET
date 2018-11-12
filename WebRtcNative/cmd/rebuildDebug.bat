cd ..\..\..\depot_tools\webrtc-checkout\src

set DEPOT_TOOLS_WIN_TOOLCHAIN=0

call gn clean out/Debug

call gn gen out/Debug --ide="vs2017" --args="is_debug=true target_cpu=\"x86\" is_clang=false"

call ninja -C out/Debug

pause