
#ifndef WEBRTC_NET_DEFAULTS_H_
#define WEBRTC_NET_DEFAULTS_H_
#pragma once

#include "internals.h"
#include "rtc_base/logging.h"
#include "media/base/videocapturer.h"
#include "api/video/i420_buffer.h"
#include "api/mediastreaminterface.h"
#include "modules/desktop_capture/desktop_capturer.h"

namespace Native
{
	class Conductor;

	class YuvFramesCapturer : public cricket::VideoCapturer, webrtc::DesktopCapturer::Callback
	{
	public:
		YuvFramesCapturer(Conductor & c);
		virtual ~YuvFramesCapturer();

		// Override virtual methods of parent class VideoCapturer.
		virtual cricket::CaptureState Start(const cricket::VideoFormat& capture_format);
		virtual void Stop();
		virtual bool IsRunning();
		virtual bool IsScreencast() const { return false; }

		void PushFrame();

		void CaptureFrame();
		virtual void OnCaptureResult(webrtc::DesktopCapturer::Result result, std::unique_ptr<webrtc::DesktopFrame> frame);
		std::unique_ptr<webrtc::DesktopFrame> desktop_frame;
		webrtc::DesktopCapturer::SourceList desktop_screens;

		rtc::scoped_refptr<webrtc::I420Buffer> video_buffer;

	protected:
		virtual bool GetPreferredFourccs(std::vector<uint32_t>* fourccs);

	private:
		Conductor * con;
		webrtc::VideoFrame * video_frame;
		bool run;
		std::unique_ptr<webrtc::DesktopCapturer> desktop_capturer;
	};

	class VideoRenderer : public rtc::VideoSinkInterface<webrtc::VideoFrame>
	{
	public:
		VideoRenderer(Conductor & c, bool remote, webrtc::VideoTrackInterface * track_to_render) :
			rendered_track_(track_to_render), con(&c), remote(remote)
		{
			rendered_track_->AddOrUpdateSink(this, rtc::VideoSinkWants());
			bgr24 = nullptr;
		}
		virtual ~VideoRenderer();

		// VideoSinkInterface implementation
		void OnFrame(const webrtc::VideoFrame & frame) override;

	protected:
		int DecodeYUV(rtc::scoped_refptr<webrtc::I420BufferInterface> b420, int & width, int & height);

		uint8_t * bgr24;
		bool remote;
		Conductor * con;
		rtc::scoped_refptr<webrtc::VideoTrackInterface> rendered_track_;
	};

	class AudioRenderer : public webrtc::AudioTrackSinkInterface
	{
	public:
		AudioRenderer(Conductor & c, bool remote, webrtc::AudioTrackInterface * track_to_render) :
			audio_track_(track_to_render), con(&c), remote(remote)
		{
			audio_track_->AddSink(this);
		}
		virtual ~AudioRenderer()
		{
			audio_track_->RemoveSink(this);
		}

		virtual void OnData(const void* audio_data,
			int bits_per_sample,
			int sample_rate,
			size_t number_of_channels,
			size_t number_of_frames) override;

	protected:

		bool remote;
		Conductor * con;
		rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track_;
	};
}
#endif  // WEBRTC_NET_DEFAULTS_H_
