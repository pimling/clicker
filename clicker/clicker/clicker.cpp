#include <windows.h>
#include <thread>

#include "clicker.hpp"
#include <iostream>

c_clicker::c_clicker( )
{
	this->m_delay = 0;
	auto engine = std::random_device{ };
	this->m_random_gen = std::mt19937{ engine( ) };
}

void c_clicker::start( HWND mc_window )
{
	while ( true )
	{
		if ( GetAsyncKeyState( VK_INSERT ) & 0x1 )
			g_config.m_enabled = !g_config.m_enabled;

		if ( !g_config.m_enabled || !GetAsyncKeyState( VK_LBUTTON ) || GetForegroundWindow( ) != mc_window )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
			continue;
		}

		g_clicker.calculate_delay( );
		g_clicker.send_click( mc_window );
	}
}

void c_clicker::calculate_delay( )
{
	if ( ( this->m_dist_cache.mean( ) + 1.f != g_config.m_cps ) || ( this->m_dist_cache.stddev( ) != g_config.m_std ) )
		this->m_dist_cache = std::normal_distribution<float>( g_config.m_cps + 1.f, g_config.m_std );

	this->m_delay = static_cast< int >( 1000000.f / this->m_dist_cache( this->m_random_gen ) );
}

void c_clicker::send_click( HWND mc_window )
{
	std::this_thread::sleep_for( std::chrono::microseconds( this->m_delay / 2 ) );
	PostMessageA( mc_window, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM( 0, 0 ) );
	std::this_thread::sleep_for( std::chrono::microseconds( this->m_delay / 2 ) );
	PostMessageA( mc_window, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM( 0, 0 ) );
}
