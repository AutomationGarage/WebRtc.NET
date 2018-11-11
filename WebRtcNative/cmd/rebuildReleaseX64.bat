cd ..\..\..\depot_tools\webrtc-checkout\src

set DEPOT_TOOLS_WIN_TOOLCHAIN=0

call gn clean out/ReleaseX64

call gn gen out/ReleaseX64 --ide="vs2017" --args="is_debug=false target_cpu=\"x64\" is_clang=false symbol_level=0"

call ninja -C out/ReleaseX64

pause