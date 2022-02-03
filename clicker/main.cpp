#include "gui/gui.hpp"
#include "clicker/clicker.hpp"

int WinMain( HINSTANCE instance, HINSTANCE prev, LPSTR cmd, int show_cmd )
{
	auto mc_window = FindWindowA( "LWJGL", nullptr );
	
	if ( !mc_window )
		return 1;

	const auto clicker_thread = CreateThread( nullptr, 0, reinterpret_cast< LPTHREAD_START_ROUTINE >( c_clicker::start ), reinterpret_cast< LPVOID >( mc_window ), 0, nullptr );

	if ( !clicker_thread )
		return 1;

	g_gui.start( );

	while ( true )
	{
		if ( g_gui.should_shutdown( ) )
		{
			g_gui.shutdown( );
			break;
		}

		g_gui.render( );
	}

	CloseHandle( clicker_thread );

	return 1;
}