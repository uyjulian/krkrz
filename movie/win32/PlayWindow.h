

#ifndef __PLAY_WINDOW_H__
#define __PLAY_WINDOW_H__

#if 0
#include <atlbase.h>
#endif
#include <windows.h>
#include <streams.h>
#if 1
#include <mfidl.h>
#include <mfapi.h>
#include <evr.h>
#include <comdef.h>
_COM_SMARTPTR_TYPEDEF(IMFByteStream,__uuidof(IMFByteStream));
_COM_SMARTPTR_TYPEDEF(IMFVideoDisplayControl,__uuidof(IMFVideoDisplayControl));
_COM_SMARTPTR_TYPEDEF(IMFMediaSession,__uuidof(IMFMediaSession));
_COM_SMARTPTR_TYPEDEF(IMFTopology,__uuidof(IMFTopology));
_COM_SMARTPTR_TYPEDEF(IMFRateControl,__uuidof(IMFRateControl));
_COM_SMARTPTR_TYPEDEF(IMFRateSupport,__uuidof(IMFRateSupport));
_COM_SMARTPTR_TYPEDEF(IMFPresentationClock,__uuidof(IMFPresentationClock));
_COM_SMARTPTR_TYPEDEF(IMFAudioStreamVolume,__uuidof(IMFAudioStreamVolume));
_COM_SMARTPTR_TYPEDEF(IMFSimpleAudioVolume,__uuidof(IMFSimpleAudioVolume));
_COM_SMARTPTR_TYPEDEF(IMFMediaSource,__uuidof(IMFMediaSource));
#endif

/**
 * Media Foundation で再生するときに使用する子Window
 */
class PlayWindow {
	HWND			m_ChildWnd;
	static ATOM		m_ChildAtom;
	HWND			m_OwnerWindow;
	HWND			m_MessageDrainWindow;

	bool		m_Visible;
	RECT		m_Rect;			//!< 指定されたムービーの表示矩形領域を保持
	RECT		m_ChildRect;

protected:
	CCritSec	m_Lock;

public:
	// window procedure
	static LRESULT WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

	PlayWindow();
	virtual ~PlayWindow();

protected:
	LRESULT WINAPI Proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

	void SetOwner( HWND hOwner ) {
		m_OwnerWindow = hOwner;
	}
	void SetMessageDrainWindow( HWND hWnd ) {
		m_MessageDrainWindow = hWnd;
	}
	HWND GetChildWindow() { return m_ChildWnd; }

	void SetRect(RECT *rect);
	void SetVisible(bool b);

	HRESULT CreateChildWindow();
	void DestroyChildWindow();
	void CalcChildWindowSize( RECT& childRect );

	virtual void OnDestoryWindow() = 0;
};


#endif // __PLAY_WINDOW_H__
