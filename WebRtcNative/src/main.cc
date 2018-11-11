
#pragma comment(lib,"secur32.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"dmoguids.lib")
#pragma comment(lib,"wmcodecdspuuid.lib")
#pragma comment(lib,"msdmo.lib")
#pragma comment(lib,"Strmiids.lib")

#pragma comment(lib,"bbr.lib")
#pragma comment(lib,"bandwidth_sampler.lib")
#pragma comment(lib,"bbr_controller.lib")
#pragma comment(lib,"loss_rate_filter.lib")
#pragma comment(lib,"rtt_stats.lib")

#pragma comment(lib,"create_pc_factory.lib")
#pragma comment(lib,"peerconnection.lib")
#pragma comment(lib,"builtin_video_decoder_factory.lib")
#pragma comment(lib,"builtin_video_encoder_factory.lib")

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