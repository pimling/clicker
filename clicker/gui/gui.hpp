#pragma once

#include <d3d9.h>

class c_gui
{
	LPDIRECT3D9 m_d3d = nullptr;
	LPDIRECT3DDEVICE9 m_device = nullptr;
	D3DPRESENT_PARAMETERS m_params = {};
	HWND m_window = nullptr;
	WNDCLASSEX m_window_class = { };

	bool create_device( HWND hwnd );
	void cleanup_device( );
	void reset_device( );
	static LRESULT WINAPI wnd_proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

	bool m_should_shutdown = false;

	void pre_frame( );
	void frame( );
	void post_frame( );

public:
	void start( );
	bool should_shutdown( );
	void render( );
	void shutdown( );
	LRESULT WINAPI handle_wndproc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
};

inline auto g_gui = c_gui{ };
