#include "defaults.h"
#include "conductor.h"

#include "third_party/libjpeg_turbo/turbojpeg.h"
#include "modules/desktop_capture/desktop_capture_options.h"
#include "modules/desktop_capture/desktop_capturer.h"

namespace Native
{
	int I420DataSize(int height, int stride_y, int stride_u, int stride_v)
	{
		return stride_y * height + (stride_u + stride_v) * ((height + 1) / 2);
	}

	YuvFramesCapturer::YuvFramesCapturer(Conductor & c) : run(false), con(&c), desktop_capturer(nullptr)
	{
		video_buffer = webrtc::I420Buffer::Create(con->width_, con->height_);
		frame_data_size_ = I420DataSize(con->height_, video_buffer->StrideY(), video_buffer->StrideU(), video_buffer->StrideV());

		video_frame = new webrtc::VideoFrame(video_buffer, 0, 0, webrtc::VideoRotation::kVideoRotation_0);

		// Enumerate the supported formats. We have only one supported format.
		cricket::VideoFormat format(con->width_, con->height_, cricket::VideoFormat::FpsToInterval(con->caputureFps), cricket::FOURCC_IYUV);
		
		std::vector<cricket::VideoFormat> supported;
		supported.push_back(format);
		SetSupportedFormats(supported);

		set_enable_video_adapter(false);
	}

	YuvFramesCapturer::~YuvFramesCapturer()
	{
		if (video_frame)
		{
			delete video_frame;
			video_frame = nullptr;
		}

		Stop();
	}

	cricket::CaptureState YuvFramesCapturer::Start(const cricket::VideoFormat& capture_format)
	{
		if (IsRunning()) { return cricket::CS_FAILED; }
		SetCaptureFormat(&capture_format);
		run = true;

		webrtc::DesktopCaptureOptions co;
		co.set_allow_directx_capturer(true);

		desktop_capturer = webrtc::DesktopCapturer::CreateScreenCapturer(co);

		desktop_capturer->GetSourceList(&desktop_screens);

		for (size_t i = 0; i < desktop_screens.size(); i++)
		{
			RTC_LOG(INFO) << "screen: " << desktop_screens[i].id << " -> " << desktop_screens[i].title;
		}

		desktop_capturer->SelectSource(desktop_screens[0].id);
		desktop_capturer->Start(this);

		return cricket::CS_RUNNING;
	}

	bool YuvFramesCapturer::IsRunning()
	{
		return run;
	}

	void YuvFramesCapturer::Stop()
	{
		run = false;
		SetCaptureFormat(nullptr);
	}

	bool YuvFramesCapturer::GetPreferredFourccs(std::vector<uint32_t>* fourccs)
	{
		if (!fourccs) { return false; }
		fourccs->push_back(GetSupportedFormats()->at(0).fourcc);
		return true;
	}

	void YuvFramesCapturer::PushFrame()
	{
		int64_t camera_time_us = rtc::TimeMicros();
		int64_t system_time_us = camera_time_us;
		int out_width;
		int out_height;
		int crop_width;
		int crop_height;
		int crop_x;
		int crop_y;
		int64_t translated_camera_time_us;

		if (AdaptFrame(con->width_,
			con->height_,
			camera_time_us,
			system_time_us,
			&out_width,
			&out_height,
			&crop_width,
			&crop_height,
			&crop_x,
			&crop_y,
			&translated_camera_time_us))
		{
			video_frame->set_timestamp_us(translated_camera_time_us);

			OnFrame(*video_frame, con->width_, con->height_);
		}
	}

	void YuvFramesCapturer::CaptureFrame()
	{
		if (desktop_capturer)
		{
			desktop_capturer->CaptureFrame();
		}
	}

	// webrtc::DesktopCapturer::Callback implementation
	void YuvFramesCapturer::OnCaptureResult(webrtc::DesktopCapturer::Result result, std::unique_ptr<webrtc::DesktopFrame> frame)
	{
		if (desktop_capturer)
		{
			desktop_frame.reset(frame.release());
		}
	}

	// VideoSinkInterface implementation
	void VideoRenderer::OnFrame(const webrtc::VideoFrame & frame)
	{
		if (remote && con->onRenderRemote)
		{
			auto b = frame.video_frame_buffer();
			int width = b->width();
			int height = b->height();

			auto b420 = b->ToI420();
			if (0 == DecodeYUV(b420->DataY(), width, height))
			{
				con->onRenderRemote(bgr24, width, height);
			}
		}
		else if (con->onRenderLocal)
		{
			auto b = frame.video_frame_buffer();
			int width = b->width();
			int height = b->height();

			auto b420 = b->ToI420();
			if (0 == DecodeYUV(b420->DataY(), width, height))
			{
				con->onRenderLocal(bgr24, width, height);
			}
		}
	}

	inline int VideoRenderer::DecodeYUV(const uint8_t * yuv, int & width, int & height)
	{
		if (jpegDecompressor == nullptr) 
		{
			jpegDecompressor = tjInitDecompress();
		}

		const int pad = 4;
		int pitch = TJPAD(tjPixelSize[TJPF_BGR] * width);

		if (bgr24 == nullptr)
			bgr24 = new uint8_t[pitch * height];

		return tjDecodeYUV(jpegDecompressor, yuv, pad, TJSAMP_420, bgr24, width, pitch, height, TJPF_BGR, true ? TJFLAG_FASTDCT : TJFLAG_ACCURATEDCT);
	}

	VideoRenderer::~VideoRenderer()
	{
		if (rendered_track_.get()) 
		{
			rendered_track_->RemoveSink(this);
		}

		if (jpegDecompressor != nullptr) 
		{
			tjDestroy(jpegDecompressor);
		}

		delete[] bgr24;
	}

	// AudioTrackSinkInterface implementation
	void AudioRenderer::OnData(const void* audio_data,
		int bits_per_sample,
		int sample_rate,
		size_t number_of_channels,
		size_t number_of_frames)
	{
		std::stringstream s;
		s << "AudioRenderer::OnData, bps: " << bits_per_sample
			<< ", rate: " << sample_rate
			<< ", channels: " << number_of_channels
			<< ", frames: " << number_of_frames;

		::OutputDebugStringA(s.str().c_str());
	}
}