cd ..\..\..\depot_tools\webrtc-checkout\src

set DEPOT_TOOLS_WIN_TOOLCHAIN=0

call gn clean out/Release

call gn gen out/Release --args="target_cpu=\"x86\" use_lld=false is_debug=false symbol_level=0"

call ninja -C out/Release

pause