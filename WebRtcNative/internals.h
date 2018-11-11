
#pragma once

#ifndef WEBRTC_NET_INTERNALS_H_
#define WEBRTC_NET_INTERNALS_H_

namespace Native
{
	extern bool CFG_quality_scaler_enabled_;

	void InitializeSSL();
	void CleanupSSL();
}

#endif // WEBRTC_NET_INTERNALS_H_