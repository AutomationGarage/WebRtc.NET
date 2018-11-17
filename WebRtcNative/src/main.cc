
#pragma comment(lib,"secur32.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"dmoguids.lib")
#pragma comment(lib,"wmcodecdspuuid.lib")
#pragma comment(lib,"msdmo.lib")
#pragma comment(lib,"Strmiids.lib")

#pragma comment(lib,"webrtc.lib")

#include "internals.h"

#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d11.lib")

#include "rtc_base/ssladapter.h"

namespace Native
{
	bool CFG_quality_scaler_enabled_ = false;

	void InitializeSSL()
	{
		rtc::InitializeSSL();
	}

	void CleanupSSL()
	{
		rtc::CleanupSSL();
	}
}

FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}