cd ..\..\..\depot_tools\webrtc-checkout\src

set DEPOT_TOOLS_WIN_TOOLCHAIN=0

call gn clean out/Release

call gn gen out/Release --ide="vs2017" --args="is_debug=false target_cpu=\"x86\" is_clang=false symbol_level=0"

call ninja -C out/Release

pause