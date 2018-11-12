
#include "defaults.h"
#include "conductor.h"

#include "rtc_base/checks.h"
#include "api/test/fakeconstraints.h"
#include "api/video_codecs/video_encoder.h"
#include "modules/audio_device/include/audio_device_default.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "modules/video_coding/codecs/vp8/include/vp8.h"
#include "modules/video_capture/video_capture_factory.h"
#include "media/engine/webrtcvideocapturerfactory.h"

// for servers
#include "p2p/base/relayserver.h"
#include "p2p/base/stunserver.h"
#include "p2p/base/basicpacketsocketfactory.h"
#include "p2p/base/turnserver.h"
#include "rtc_base/asyncudpsocket.h"
#include "rtc_base/optionsfile.h"
#include "rtc_base/stringencode.h"
#include "rtc_base/thread.h"

#include "third_party/libjpeg_turbo/turbojpeg.h"
#include "atlsafe.h"

extern "C"
{
	__declspec(dllexport) void * WINAPI NewConductor()
	{
		return new Native::Conductor();
	}

	__declspec(dllexport) void WINAPI DeleteConductor(Native::Conductor * cd)
	{
		delete cd;
	}

	__declspec(dllexport) void WINAPI InitializeSSL()
	{
		Native::InitializeSSL();
	}

	__declspec(dllexport) void WINAPI CleanupSSL()
	{
		Native::CleanupSSL();
	}

	//------------------------------------------------

	__declspec(dllexport) void WINAPI onRenderLocal(Native::Conductor * cd, Native::OnRenderCallbackNative callback)
	{
		cd->onRenderLocal = callback;
	}

	__declspec(dllexport) void WINAPI onRenderRemote(Native::Conductor * cd, Native::OnRenderCallbackNative callback)
	{
		cd->onRenderRemote = callback;
	}

	__declspec(dllexport) void WINAPI onError(Native::Conductor * cd, Native::OnErrorCallbackNative callback)
	{
		cd->onError = callback;
	}

	__declspec(dllexport) void WINAPI onSuccess(Native::Conductor * cd, Native::OnSuccessCallbackNative callback)
	{
		cd->onSuccess = callback;
	}

	__declspec(dllexport) void WINAPI onFailure(Native::Conductor * cd, Native::OnFailureCallbackNative callback)
	{
		cd->onFailure = callback;
	}

	__declspec(dllexport) void WINAPI onDataMessage(Native::Conductor * cd, Native::OnDataMessageCallbackNative callback)
	{
		cd->onDataMessage = callback;
	}

	__declspec(dllexport) void WINAPI onDataBinaryMessage(Native::Conductor * cd, Native::OnDataBinaryMessageCallbackNative callback)
	{
		cd->onDataBinaryMessage = callback;
	}

	__declspec(dllexport) void WINAPI onIceCandidate(Native::Conductor * cd, Native::OnIceCandidateCallbackNative callback)
	{
		cd->onIceCandidate = callback;
	}

	//------------------------------------------------

	__declspec(dllexport) bool WINAPI InitializePeerConnection(Native::Conductor * cd)
	{
		return cd->InitializePeerConnection();
	}

	__declspec(dllexport) void WINAPI CreateOffer(Native::Conductor * cd)
	{
		cd->CreateOffer();
	}

	__declspec(dllexport) bool WINAPI ProcessMessages(Native::Conductor * cd, int delay)
	{
		return cd->ProcessMessages(delay);
	}

	__declspec(dllexport) bool WINAPI OpenVideoCaptureDevice(Native::Conductor * cd, const char * name)
	{
		return cd->OpenVideoCaptureDevice(name);
	}

	__declspec(dllexport) LPSAFEARRAY WINAPI GetVideoDevices()
	{
		auto device_names = Native::Conductor::GetVideoDevices();

		CComSafeArray<BSTR> a(device_names.size()); // cool ATL helper that requires atlsafe.h

		int i = 0;
		for (auto it = device_names.begin(); it != device_names.end(); ++it, ++i)
		{
			// note: you could also use std::wstring instead and avoid A2W conversion
			a.SetAt(i, A2BSTR_EX((*it).c_str()), FALSE);
		}
		return a.Detach();
	}

	__declspec(dllexport) void WINAPI OnOfferReply(Native::Conductor * cd, const char * type, const char * sdp)
	{
		cd->OnOfferReply(type, sdp);
	}

	__declspec(dllexport) void WINAPI OnOfferRequest(Native::Conductor * cd, const char * sdp)
	{
		cd->OnOfferRequest(sdp);
	}

	__declspec(dllexport) bool WINAPI AddIceCandidate(Native::Conductor * cd, const char * sdp_mid, int sdp_mlineindex, const char * sdp)
	{
		return cd->AddIceCandidate(sdp_mid, sdp_mlineindex, sdp);
	}

	__declspec(dllexport) void WINAPI AddServerConfig(Native::Conductor * cd, const char * uri, const char * username, const char * password)
	{
		cd->AddServerConfig(uri, username, password);
	}

	__declspec(dllexport) void WINAPI CreateDataChannel(Native::Conductor * cd, const char * label)
	{
		cd->CreateDataChannel(label);
	}

	__declspec(dllexport) void WINAPI DataChannelSendText(Native::Conductor * cd, const char * text)
	{
		cd->DataChannelSendText(text);
	}

	__declspec(dllexport) void WINAPI DataChannelSendData(Native::Conductor * cd, uint8_t * array_data, int length)
	{
		rtc::CopyOnWriteBuffer writeBuffer(array_data, length);
		cd->DataChannelSendData(webrtc::DataBuffer(writeBuffer, true));
	}

	__declspec(dllexport) void WINAPI SetAudio(Native::Conductor * cd, bool enable)
	{
		cd->audioEnabled = enable;
	}

	__declspec(dllexport) void WINAPI SetVideoCapturer(Native::Conductor * cd, int width, int height, int caputureFps)
	{
		cd->width_ = width;
		cd->height_ = height;
		cd->caputureFps = caputureFps;
	}

	__declspec(dllexport) void WINAPI PushFrame(Native::Conductor * cd, uint8_t * img, int type)
	{
		cd->PushFrame(img, type); // default: TJPF_BGR
	}

	__declspec(dllexport) bool WINAPI RunStunServer(Native::Conductor * cd, const char * bindIp)
	{
		return cd->RunStunServer(bindIp);
	}

	__declspec(dllexport) uint8_t * WINAPI CaptureFrameBGRX(Native::Conductor * cd, int & w, int & h)
	{
		int wn = 0, hn = 0;
		auto b = cd->CaptureFrameBGRX(wn, hn);
		if (b != nullptr)
		{
			w = wn;
			h = hn;
		}
		return b;
	}

	__declspec(dllexport) void WINAPI CaptureFrameAndPush(Native::Conductor * cd)
	{
		int wn = 0, hn = 0;
		auto b = cd->CaptureFrameBGRX(wn, hn);
		if (b != nullptr && wn == cd->width_ && hn == cd->height_)
		{
			cd->PushFrame(b, TJPF_BGRX);
		}
	}

	// File is stored as lines of <username>=<HA1>.
	// Generate HA1 via "echo -n "<username>:<realm>:<password>" | md5sum"
	__declspec(dllexport) bool WINAPI RunTurnServer(Native::Conductor * cd, const char * bindIp, const char * ip, const char * realm, const char * authFile)
	{
		return cd->RunTurnServer(bindIp, ip, realm, authFile);
	}
}

namespace Native
{
	const char kAudioLabel[] = "audio_label";
	const char kVideoLabel[] = "video_label";
	const char kStreamLabel[] = "stream_label";
	const char kSoftware[] = "libjingle TurnServer";

	class TurnFileAuth : public cricket::TurnAuthInterface
	{
	public:
		explicit TurnFileAuth(const std::string& path) : file_(path)
		{
		}

		bool Load()
		{
			return file_.Load();
		}

		virtual bool GetKey(const std::string& username, const std::string& realm, std::string* key)
		{
			// File is stored as lines of <username>=<HA1>.
			// Generate HA1 via "echo -n "<username>:<realm>:<password>" | md5sum"
			std::string hex;
			bool ret = file_.GetStringValue(username, &hex);
			if (ret)
			{
				char buf[32];
				size_t len = rtc::hex_decode(buf, sizeof(buf), hex);
				*key = std::string(buf, len);
			}
			return ret;
		}
	private:
		rtc::OptionsFile file_;
	};

	Conductor::Conductor()
	{
		onError = nullptr;
		onSuccess = nullptr;
		onFailure = nullptr;
		onIceCandidate = nullptr;

		width_ = 640;
	    height_ = 360;
		caputureFps = 5;
		audioEnabled = false;

		turnServer = nullptr;
		data_channel = nullptr;
		onDataMessage = nullptr;
		internal_capturer = nullptr;
		external_capturer = nullptr;

		jpegc = nullptr;
	}

	Conductor::~Conductor()
	{
		if (jpegc != nullptr)
			tjDestroy(jpegc);

		DeletePeerConnection();
		RTC_DCHECK(peer_connection_ == nullptr);

		if (turnServer)
		{
			turnServer->disconnect_all();
		}

		if (stunServer)
		{
			stunServer->disconnect_all();
		}

		if (turnServer || stunServer)
		{
			auto c = rtc::Thread::Current();
			if (c)
			{
				c->Quit();
			}
		}
	}

	void Conductor::DeletePeerConnection()
	{
		if (peer_connection_.get())
		{
			local_video.reset();

			for (auto it = active_streams_.begin(); it != active_streams_.end(); ++it)
			{
				peer_connection_->RemoveStream(it->second);
			}
			active_streams_.clear();

			peer_connection_->Close();
			peer_connection_ = nullptr;
		}

		if (data_channel.get())
		{
			data_channel->Close();
			data_channel->UnregisterObserver();
			data_channel = nullptr;
		}
		serverConfigs.clear();

		internal_capturer = nullptr;
		external_capturer = nullptr;
		pc_factory_ = nullptr;
	}

	bool Conductor::InitializePeerConnection()
	{
		RTC_DCHECK(pc_factory_ == nullptr);
		RTC_DCHECK(peer_connection_ == nullptr);

		pc_factory_ = webrtc::CreatePeerConnectionFactory(
			nullptr /* network_thread */, nullptr /* worker_thread */,
			nullptr /* signaling_thread */, audioEnabled ? nullptr : webrtc::AudioDeviceModule::Create(webrtc::AudioDeviceModule::AudioLayer::kDummyAudio),
			webrtc::CreateBuiltinAudioEncoderFactory(),
			webrtc::CreateBuiltinAudioDecoderFactory(),
			webrtc::CreateBuiltinVideoEncoderFactory(),
			webrtc::CreateBuiltinVideoDecoderFactory(), nullptr /* audio_mixer */,
			nullptr /* audio_processing */);

		if (!pc_factory_)
		{
			DeletePeerConnection();
			return false;
		}

		webrtc::PeerConnectionFactoryInterface::Options opt;
		{
			//opt.disable_encryption = true;
			//opt.disable_network_monitor = true;
			//opt.disable_sctp_data_channels = true;
			pc_factory_->SetOptions(opt);
		}

		if (!CreatePeerConnection(true))
		{
			DeletePeerConnection();
			return false;
		}
		AddStreams();
		return peer_connection_ != nullptr;
	}

	bool Conductor::CreatePeerConnection(bool dtls)
	{
		RTC_DCHECK(pc_factory_ != nullptr);
		RTC_DCHECK(peer_connection_ == nullptr);

		webrtc::PeerConnectionInterface::RTCConfiguration config;
		config.tcp_candidate_policy = webrtc::PeerConnectionInterface::kTcpCandidatePolicyDisabled;
		config.disable_ipv6 = true;
		config.enable_dtls_srtp = absl::optional<bool>(dtls);
		config.rtcp_mux_policy = webrtc::PeerConnectionInterface::kRtcpMuxPolicyRequire;

		for each (auto server in serverConfigs)
		{
			config.servers.push_back(server);
		}

		peer_connection_ = pc_factory_->CreatePeerConnection(config, nullptr, nullptr, this);
		return peer_connection_ != nullptr;
	}

	void Conductor::AddServerConfig(const std::string & uri, const std::string & username, const std::string & password)
	{
		webrtc::PeerConnectionInterface::IceServer server;
		server.uri = uri;
		server.username = username;
		server.password = password;

		serverConfigs.push_back(server);
	}

	void Conductor::CreateOffer()
	{
		if (!peer_connection_)
			return;

		peer_connection_->CreateOffer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
	}

	void Conductor::OnOfferReply(const std::string & type, const std::string & sdp)
	{
		if (!peer_connection_)
			return;

		webrtc::SdpParseError error;
		webrtc::SessionDescriptionInterface* session_description(webrtc::CreateSessionDescription(type, sdp, &error));
		if (!session_description)
		{
			RTC_LOG(WARNING) << "Can't parse received session description message. " << "SdpParseError was: " << error.description;
			return;
		}
		peer_connection_->SetRemoteDescription(this, session_description);
	}

	void Conductor::OnOfferRequest(const std::string & sdp)
	{
		if (!peer_connection_)
			return;

		webrtc::SdpParseError error;
		webrtc::SessionDescriptionInterface* session_description(webrtc::CreateSessionDescription("offer", sdp, &error));
		if (!session_description)
		{
			RTC_LOG(WARNING) << "Can't parse received session description message. " << "SdpParseError was: " << error.description;
			return;
		}
		peer_connection_->SetRemoteDescription(this, session_description);

		webrtc::PeerConnectionInterface::RTCOfferAnswerOptions o;
		{
			o.voice_activity_detection = false;
			o.offer_to_receive_audio = webrtc::PeerConnectionInterface::RTCOfferAnswerOptions::kOfferToReceiveMediaTrue;
			o.offer_to_receive_video = webrtc::PeerConnectionInterface::RTCOfferAnswerOptions::kOfferToReceiveMediaTrue;
		}
		peer_connection_->CreateAnswer(this, o);
	}

	bool Conductor::AddIceCandidate(const std::string & sdp_mid, int sdp_mlineindex, const std::string & sdp)
	{
		webrtc::SdpParseError error;
		webrtc::IceCandidateInterface * candidate = webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp, &error);
		if (!candidate)
		{
			RTC_LOG(WARNING) << "Can't parse received candidate message. " << "SdpParseError was: " << error.description;
			return false;
		}

		if (!peer_connection_)
			return false;

		if (!peer_connection_->AddIceCandidate(candidate))
		{
			RTC_LOG(WARNING) << "Failed to apply the received candidate";
			return false;
		}
		return true;
	}

	// ...

	std::vector<std::string> Conductor::GetVideoDevices()
	{
		std::vector<std::string> device_names;
		{
			std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(webrtc::VideoCaptureFactory::CreateDeviceInfo());
			if (info)
			{
				int num_devices = info->NumberOfDevices();
				for (int i = 0; i < num_devices; ++i)
				{
					const uint32_t kSize = 256;
					char name[kSize] = { 0 };
					char id[kSize] = { 0 };
					if (info->GetDeviceName(i, name, kSize, id, kSize) != -1)
					{
						device_names.push_back(name);
					}
				}
			}
		}
		return device_names;
	}

	bool Conductor::OpenVideoCaptureDevice(const std::string & name)
	{
		if (!internal_capturer)
		{
			cricket::WebRtcVideoDeviceCapturerFactory factory;
			internal_capturer = factory.Create(cricket::Device(name, 0));
			if (internal_capturer)
			{
				RTC_LOG(LS_ERROR) << "Capturer != NULL!";
				return true;
			}
		}
		return false;
	}

	void Conductor::PushFrame(uint8_t * img, int pxFormat)
	{
		if (external_capturer)
		{
			auto yuv = (uint8_t*)external_capturer->video_buffer->DataY();
			if (yuv != nullptr)
			{
				if (img != nullptr)
				{
					const int pad = 4;
					int pitch = TJPAD(tjPixelSize[pxFormat] * width_);

					if (jpegc == nullptr)
						jpegc = tjInitCompress();

					int r = 0;

					if (jpegc)
						r = tjEncodeYUV3(jpegc, img, width_, pitch, height_, pxFormat, yuv, pad, TJSAMP_420, true ? TJFLAG_FASTDCT : TJFLAG_ACCURATEDCT);

					if (r == 0)
					{
						external_capturer->PushFrame();
					}
					else
					{
						RTC_LOG(LS_ERROR) << tjGetErrorStr();
					}
				}
			}
		}
	}

	void Conductor::AddStreams()
	{
		if (active_streams_.find(kStreamLabel) != active_streams_.end())
			return;  // Already added.

		auto v = pc_factory_->CreateVideoSource(internal_capturer ? std::move(internal_capturer) : std::unique_ptr<cricket::VideoCapturer>(external_capturer = new Native::YuvFramesCapturer(*this)));
		auto video_track = pc_factory_->CreateVideoTrack(kVideoLabel, v);
		if (onRenderLocal)
		{
			local_video.reset(new VideoRenderer(*this, false, video_track));
	    }

		auto stream = pc_factory_->CreateLocalMediaStream(kStreamLabel);
		{
			if (audioEnabled)
			{
				auto a = pc_factory_->CreateAudioSource(cricket::AudioOptions());
				auto audio_track = pc_factory_->CreateAudioTrack(kAudioLabel, a);
				stream->AddTrack(audio_track);
			}
			stream->AddTrack(video_track);

			if (!peer_connection_->AddStream(stream))
			{
				RTC_LOG(LS_ERROR) << "Adding stream to PeerConnection failed";
			}
			typedef std::pair<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface> >	MediaStreamPair;
			active_streams_.insert(MediaStreamPair(stream->id(), stream));
		}
	}

	// Called when a remote stream is added
	void Conductor::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
	{
		RTC_LOG(INFO) << __FUNCTION__ << " " << stream->id();

		if (onRenderRemote)
		{
			webrtc::VideoTrackVector tracks = stream->GetVideoTracks();
			if (!tracks.empty())
			{
				webrtc::VideoTrackInterface* track = tracks[0];
				remote_video.reset(new Native::VideoRenderer(*this, true, track));
			}
		}

		if (audioEnabled)
		{
			webrtc::AudioTrackVector atracks = stream->GetAudioTracks();
			if (!atracks.empty())
			{
				webrtc::AudioTrackInterface* track = atracks[0];
				remote_audio.reset(new Native::AudioRenderer(*this, true, track));
			}
		}
	}

	void Conductor::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
	{
		RTC_LOG(INFO) << __FUNCTION__ << " " << stream->id();
		remote_video.reset(nullptr);
		remote_audio.reset(nullptr);

		// lost ownership, do not delete
		external_capturer = nullptr;
		internal_capturer = nullptr;
	}

	void Conductor::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
	{
		RTC_LOG(INFO) << __FUNCTION__ << " " << candidate->sdp_mline_index();

		std::string sdp;
		if (!candidate->ToString(&sdp))
		{
			RTC_LOG(LS_ERROR) << "Failed to serialize candidate";
			return;
		}

		if (onIceCandidate != nullptr)
		{
			onIceCandidate(candidate->sdp_mid().c_str(), candidate->sdp_mline_index(), sdp.c_str());
		}
	}

	void Conductor::OnSuccess(webrtc::SessionDescriptionInterface* desc)
	{
		if (!peer_connection_)
			return;

		peer_connection_->SetLocalDescription(this, desc);

		std::string sdp;
		desc->ToString(&sdp);

		if (onSuccess != nullptr)
		{
			onSuccess(desc->type().c_str(), sdp.c_str());
		}
	}

	void Conductor::OnFailure(const std::string & error)
	{
		RTC_LOG(LERROR) << error;

		if (onFailure != nullptr)
		{
			onFailure(error.c_str());
		}
	}

	void Conductor::OnError()
	{
		if (onError != nullptr)
		{
			onError();
		}
	}

	void Conductor::CreateDataChannel(const std::string & label)
	{
		if (!peer_connection_)
			return;

		webrtc::DataChannelInit dc_options;
		//dc_options.id = 1;
		dc_options.maxRetransmits = 1;
		dc_options.negotiated = false;
		dc_options.ordered = false;

		data_channel = peer_connection_->CreateDataChannel(label, &dc_options);
		data_channel->RegisterObserver(this);
	}

	void Conductor::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel)
	{
		RTC_LOG(INFO) << __FUNCTION__ << " " << channel->label();

		data_channel = channel.get();
		data_channel->RegisterObserver(this);
	}

	void Conductor::DataChannelSendText(const std::string & text)
	{
		data_channel->Send(webrtc::DataBuffer(text));
	}

	void Conductor::DataChannelSendData(const webrtc::DataBuffer & data)
	{
		data_channel->Send(data);
	}

	//  A data buffer was successfully received.
	void Conductor::OnMessage(const webrtc::DataBuffer& buffer)
	{
		RTC_LOG(INFO) << __FUNCTION__;

		if (buffer.binary)
		{
			if (onDataBinaryMessage != nullptr)
			{
				auto * data = buffer.data.data();
				onDataBinaryMessage(data, buffer.size());
			}
		}
		else
		{
			if (onDataMessage != nullptr)
			{
				std::string msg(buffer.data.data<char>(), buffer.size());
				onDataMessage(msg.c_str());
			}
		}
	}

	bool Conductor::RunStunServer(const std::string & bindIp)
	{
		rtc::SocketAddress server_addr;
		if (!server_addr.FromString(bindIp))
		{
			RTC_LOG(LERROR) << "Unable to parse IP address: ";
			return false;
		}

		rtc::Thread * main = rtc::ThreadManager::Instance()->WrapCurrentThread();
		rtc::AsyncUDPSocket* server_socket = rtc::AsyncUDPSocket::Create(main->socketserver(), server_addr);
		if (!server_socket)
		{
			RTC_LOG(LERROR) << "Failed to create a UDP socket";
			return false;
		}

		stunServer.reset(new cricket::StunServer(server_socket));

		RTC_LOG(INFO) << "Listening at " << server_addr.ToString();

		return true;
	}

	bool Conductor::RunTurnServer(const std::string & bindIp, const std::string & ip,
								  const std::string & realm, const std::string & authFile)
	{
		rtc::SocketAddress int_addr;
		if (!int_addr.FromString(bindIp))
		{
			RTC_LOG(LERROR) << "Unable to parse IP address: " << bindIp;
			return false;
		}

		rtc::IPAddress ext_addr;
		if (!IPFromString(ip, &ext_addr))
		{
			RTC_LOG(LERROR) << "Unable to parse IP address: " << ip;
			return false;
		}

		rtc::Thread * main = rtc::ThreadManager::Instance()->WrapCurrentThread();
		rtc::AsyncUDPSocket * int_socket = rtc::AsyncUDPSocket::Create(main->socketserver(), int_addr);
		if (!int_socket)
		{
			RTC_LOG(LERROR) << "Failed to create a UDP socket bound at" << int_addr.ToString();
			return false;
		}

		TurnFileAuth * auth = new TurnFileAuth(authFile);
		if (!auth->Load())
		{
			RTC_LOG(LERROR) << "Failed to load auth file " << authFile;
			return false;
		}


		auto t = new cricket::TurnServer(main);
		turnServer.reset(t);

		t->set_realm(realm);
		t->set_software(kSoftware);
		t->set_auth_hook(auth);
		t->AddInternalSocket(int_socket, cricket::PROTO_UDP);
		t->SetExternalSocketFactory(new rtc::BasicPacketSocketFactory(),
											 rtc::SocketAddress(ext_addr, 0));

		RTC_LOG(INFO) << "Listening internally at " << int_addr.ToString();

		return true;
	}
}