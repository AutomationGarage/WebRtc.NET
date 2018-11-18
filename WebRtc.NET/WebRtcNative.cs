﻿using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Security;
using System.IO;

namespace WebRtc.NET
{
    public class WebRtcNative : IDisposable
    {
        const string dll = "WebRtcNative";

        static WebRtcNative()
        {
            var dir = Path.Combine(Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location), IntPtr.Size == 8 ? "x64" : "x86");
            var file =  Path.Combine(dir, dll) + ".dll";

            if (IntPtr.Zero == LoadLibrary(file)) { throw new Exception("Failed to load: " + file); }
        }

        #region -- Constructs --

        [DllImport(dll)]
        static extern IntPtr NewConductor();

        [DllImport(dll)]
        static extern void DeleteConductor(IntPtr p);

        #endregion

        #region -- Ssl --

        [DllImport(dll)]
        public static extern void InitializeSSL();

        [DllImport(dll)]
        public static extern void CleanupSSL();

        #endregion

        #region -- Events --

        [DllImport(dll)]
        extern static void onRenderLocal(IntPtr p, IntPtr callback);

        [DllImport(dll)]
        extern static void onRenderRemote(IntPtr p, IntPtr callback);

        [DllImport(dll)]
        extern static void onError(IntPtr p, IntPtr callback);

        [DllImport(dll)]
        extern static void onSuccess(IntPtr p, IntPtr callback);

        [DllImport(dll)]
        extern static void onFailure(IntPtr p, IntPtr callback);

        [DllImport(dll)]
        extern static void onDataMessage(IntPtr p, IntPtr callback);

        [DllImport(dll)]
        extern static void onDataBinaryMessage(IntPtr p, IntPtr callback);

        [DllImport(dll)]
        extern static void onIceCandidate(IntPtr p, IntPtr callback);

        #endregion

        #region -- PeerConnection --

        [DllImport(dll)]
        static extern bool InitializePeerConnection(IntPtr p);
        public bool InitializePeerConnection()
        {
            return InitializePeerConnection(p);
        }

        [DllImport(dll)]
        static extern void CreateOffer(IntPtr p);
        public void CreateOffer()
        {
            CreateOffer(p);
        }

        [DllImport(dll)]
        static extern void OnOfferReply(IntPtr p, string type, string sdp);
        public void OnOfferReply(string type, string sdp)
        {
            OnOfferReply(p, type, sdp);
        }

        [DllImport(dll)]
        static extern void OnOfferRequest(IntPtr p, string sdp);
        public void OnOfferRequest(string sdp)
        {
            OnOfferRequest(p, sdp);
        }

        [DllImport(dll)]
        static extern bool AddIceCandidate(IntPtr p, string sdp_mid, int sdp_mlineindex, string sdp);
        public bool AddIceCandidate(string sdp_mid, int sdp_mlineindex, string sdp)
        {
            return AddIceCandidate(p, sdp_mid, sdp_mlineindex, sdp);
        }

        #endregion

        #region -- DataChannel --

        [DllImport(dll)]
        static extern void CreateDataChannel(IntPtr p, string label);
        public void CreateDataChannel(string label)
        {
            CreateDataChannel(p, label);
        }

        [DllImport(dll)]
        static extern void DataChannelSendText(IntPtr p, string text);
        public void DataChannelSendText(string text)
        {
            DataChannelSendText(p, text);
        }

        [DllImport(dll)]
        static extern void DataChannelSendData(IntPtr p, byte[] data, int length);
        public void DataChannelSendData(byte[] data, int length)
        {
            DataChannelSendData(p, data, length);
        }

        #endregion

        #region -- VideoCapturer --

        [DllImport(dll)]
        static extern bool OpenVideoCaptureDevice(IntPtr p, string name);
        public bool OpenVideoCaptureDevice(string name)
        {
            return OpenVideoCaptureDevice(p, name);
        }

        [DllImport(dll)]
        [return: MarshalAs(UnmanagedType.SafeArray)]
        static extern string[] GetVideoDevices();
        public static string[] VideoDevices()
        {
            return GetVideoDevices();
        }

        [DllImport(dll)]
        static extern void SetVideoCapturer(IntPtr p, int width, int height, int caputureFps);
        public void SetVideoCapturer(int width, int height, int caputureFps)
        {
            SetVideoCapturer(p, width, height, caputureFps);
        }

        [SuppressUnmanagedCodeSecurity]
        [DllImport(dll)]
        static extern void PushFrame(IntPtr p, IntPtr img);
        public void PushFrame(IntPtr img)
        {
            PushFrame(p, img);
        }

        [SuppressUnmanagedCodeSecurity]
        [DllImport(dll)]
        static extern IntPtr CaptureFrameBGRX(IntPtr p, ref Int32 width, ref Int32 height);
        public IntPtr CaptureFrameBGRX(ref Int32 width, ref Int32 height)
        {
            return CaptureFrameBGRX(p, ref width, ref height);
        }

        [SuppressUnmanagedCodeSecurity]
        [DllImport(dll)]
        static extern void CaptureFrameAndPush(IntPtr p);
        public void CaptureFrameAndPush()
        {
            CaptureFrameAndPush(p);
        }

        #endregion

        #region -- Etc --

        [DllImport(dll)]
        static extern void SetAudio(IntPtr p, bool enable);
        public void SetAudio(bool enable)
        {
            SetAudio(p, enable);
        }

        [DllImport(dll)]
        static extern void AddServerConfig(IntPtr p, string uri, string username, string password);
        public void AddServerConfig(string uri, string username, string password)
        {
            AddServerConfig(p, uri, username, password);
        }

        [SuppressUnmanagedCodeSecurity]
        [DllImport(dll)]
        static extern bool ProcessMessages(IntPtr p, int delay);
        public bool ProcessMessages(int delay)
        {
            return ProcessMessages(p, delay);
        }

        [DllImport("kernel32", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern IntPtr LoadLibrary(string lpFileName);

        #endregion

        #region -- Servers --

        [DllImport(dll)]
        static extern bool RunStunServer(IntPtr p, string bindIp);
        bool RunStunServer(string bindIp)
        {
            return RunStunServer(p, bindIp);
        }

        [DllImport(dll)]
        static extern bool RunTurnServer(IntPtr p, string bindIp, string ip, string realm, string authFile);
        public bool RunTurnServer(string bindIp, string ip, string realm, string authFile)
        {
            return RunTurnServer(p, bindIp, ip, realm, authFile);
        }

        #endregion

        #region -- public events --

        public delegate void OnCallbackSdp(String sdp);
        public event OnCallbackSdp OnSuccessOffer;
        public event OnCallbackSdp OnSuccessAnswer;

        public delegate void OnCallbackIceCandidate(String sdp_mid, Int32 sdp_mline_index, String sdp);
        public event OnCallbackIceCandidate OnIceCandidate;

        public event Action<string> OnError;

        public delegate void OnCallbackError(String error);
        public event OnCallbackError OnFailure;

        public delegate void OnCallbackDataMessage(String msg);
        public event OnCallbackDataMessage OnDataMessage;

        public delegate void OnCallbackDataBinaryMessage(byte[] msg);
        public event OnCallbackDataBinaryMessage OnDataBinaryMessage;

        public unsafe delegate void OnCallbackRender(IntPtr BGR24, UInt32 w, UInt32 h);
        public event OnCallbackRender OnRenderLocal;
        public event OnCallbackRender OnRenderRemote;

        #endregion

        IntPtr p;
        public WebRtcNative()
        {
            p = NewConductor();

            #region -- events --

            _onRenderLocal = new _OnRenderCallback(_OnRenderLocal);
            onRenderLocal(p, Marshal.GetFunctionPointerForDelegate(_onRenderLocal));

            _onRenderRemote = new _OnRenderCallback(_OnRenderRemote);
            onRenderRemote(p, Marshal.GetFunctionPointerForDelegate(_onRenderRemote));

            _onError = new _OnErrorCallback(_OnError);
            onError(p, Marshal.GetFunctionPointerForDelegate(_onError));

            _onSuccess = new _OnSuccessCallback(_OnSuccess);
            onSuccess(p, Marshal.GetFunctionPointerForDelegate(_onSuccess));

            _onFailure = new _OnFailureCallback(_OnFailure);
            onFailure(p, Marshal.GetFunctionPointerForDelegate(_onFailure));

            _onDataMessage = new _OnDataMessageCallback(_OnDataMessage);
            onDataMessage(p, Marshal.GetFunctionPointerForDelegate(_onDataMessage));

            _onDataBinaryMessage = new _OnDataBinaryMessageCallback(_OnDataBinaryMessage);
            onDataBinaryMessage(p, Marshal.GetFunctionPointerForDelegate(_onDataBinaryMessage));

            _onIceCandidate = new _OnIceCandidateCallback(_OnIceCandidate);
            onIceCandidate(p, Marshal.GetFunctionPointerForDelegate(_onIceCandidate));

            #endregion
        }

        public void Dispose()
        {
            DeleteConductor(p);
        }

        #region -- events --

        delegate void _OnRenderCallback(IntPtr BGR24, UInt32 w, UInt32 h);
        _OnRenderCallback _onRenderLocal;
        _OnRenderCallback _onRenderRemote;

        delegate void _OnErrorCallback();
        _OnErrorCallback _onError;

        delegate void _OnSuccessCallback(String type, String sdp);
        _OnSuccessCallback _onSuccess;

        delegate void _OnFailureCallback(String error);
        _OnFailureCallback _onFailure;

        delegate void _OnDataMessageCallback(String msg);
        _OnDataMessageCallback _onDataMessage;

        delegate void _OnDataBinaryMessageCallback(IntPtr msg, UInt32 size);
        _OnDataBinaryMessageCallback _onDataBinaryMessage;

        delegate void _OnIceCandidateCallback(String sdp_mid, Int32 sdp_mline_index, String sdp);
        _OnIceCandidateCallback _onIceCandidate;

        void _OnRenderLocal(IntPtr BGR24, UInt32 w, UInt32 h)
        {
            OnRenderLocal?.Invoke(BGR24, w, h);
        }

        void _OnRenderRemote(IntPtr BGR24, UInt32 w, UInt32 h)
        {
            OnRenderRemote?.Invoke(BGR24, w, h);
        }

        void _OnError()
        {
            Debug.WriteLine("OnError");

            OnError?.Invoke("webrtc error");
        }

        void _OnSuccess(String type, String sdp)
        {
            Debug.WriteLine(String.Format("OnSuccess: {0} -> {1}", type, sdp));

            if (type == "offer")
            {
                OnSuccessOffer?.Invoke(sdp);
            }
            else if (type == "answer")
            {
                OnSuccessAnswer?.Invoke(sdp);
            }
        }

        void _OnIceCandidate(String sdp_mid, Int32 sdp_mline_index, String sdp)
        {
            Debug.WriteLine(String.Format("OnIceCandidate: {0}", sdp));

            OnIceCandidate?.Invoke(sdp_mid, sdp_mline_index, sdp);
        }

        void _OnFailure(String error)
        {
            Debug.WriteLine(String.Format("OnFailure: {0}", error));

            OnFailure?.Invoke(error);
        }

        void _OnDataMessage(String msg)
        {
            OnDataMessage?.Invoke(msg);
        }

        void _OnDataBinaryMessage(IntPtr msg, UInt32 size)
        {
            byte[] data_array = new byte[size];

            Marshal.Copy(msg, data_array, 0, (int)size);

            OnDataBinaryMessage?.Invoke(data_array);
        }

        #endregion
    }
}
