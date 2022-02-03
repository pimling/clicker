#include <tchar.h>

#include "gui.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "../clicker/clicker.hpp"

bool c_gui::create_device( HWND hwnd )
{
    if ( ( this->m_d3d = Direct3DCreate9( D3D_SDK_VERSION ) ) == nullptr )
        return false;

    ZeroMemory( &this->m_params, sizeof( this->m_params ) );
    this->m_params.Windowed = TRUE;
    this->m_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    this->m_params.BackBufferFormat = D3DFMT_UNKNOWN;
    this->m_params.EnableAutoDepthStencil = TRUE;
    this->m_params.AutoDepthStencilFormat = D3DFMT_D16;
    this->m_params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    if ( this->m_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, this->m_window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &this->m_params, &this->m_device ) < 0 )
        return false;

    return true;
}

void c_gui::cleanup_device( )
{
    if ( this->m_device )
    {
        this->m_device->Release( );
        this->m_device = nullptr;
    }

    if ( this->m_d3d ) 
    { 
        this->m_d3d->Release( );
        this->m_d3d = nullptr; 
    }
}

void c_gui::reset_device( )
{
    ImGui_ImplDX9_InvalidateDeviceObjects( );
    HRESULT hr = this->m_device->Reset( &this->m_params );
    if ( hr == D3DERR_INVALIDCALL )
        IM_ASSERT( 0 );
    ImGui_ImplDX9_CreateDeviceObjects( );
}

LRESULT __stdcall c_gui::wnd_proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    return g_gui.handle_wndproc( hwnd, msg, wparam, lparam );
}

void c_gui::pre_frame( )
{
    MSG msg;
    while ( ::PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) )
    {
        ::TranslateMessage( &msg );
        ::DispatchMessage( &msg );
        if ( msg.message == WM_QUIT )
            this->m_should_shutdown = true;
    }

    if ( this->m_should_shutdown )
        return;

    ImGui_ImplDX9_NewFrame( );
    ImGui_ImplWin32_NewFrame( );
    ImGui::NewFrame( );
}

void c_gui::frame( )
{
    ImGui::SetNextWindowPos( ImVec2{ 0, 0 } );
    ImGui::SetNextWindowSize( ImVec2{ 400, 300 } );
    ImGui::Begin( "clicker", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove );
    ImGui::SliderFloat( "cps", &g_config.m_cps, 0, 20, "%.1f" );
    ImGui::SliderFloat( "standard dev", &g_config.m_std, 0, 5, "%.2f" );
    ImGui::End( );
}

void c_gui::post_frame( )
{
    ImVec4 clear_color = ImVec4( 0.45f, 0.55f, 0.60f, 1.00f );
    ImGui::EndFrame( );
    this->m_device->SetRenderState( D3DRS_ZENABLE, FALSE );
    this->m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    this->m_device->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
    D3DCOLOR clear_col_dx = D3DCOLOR_RGBA( ( int ) ( clear_color.x * clear_color.w * 255.0f ), ( int ) ( clear_color.y * clear_color.w * 255.0f ), ( int ) ( clear_color.z * clear_color.w * 255.0f ), ( int ) ( clear_color.w * 255.0f ) );
    this->m_device->Clear( 0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0 );
    if ( this->m_device->BeginScene( ) >= 0 )
    {
        ImGui::Render( );
        ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
        this->m_device->EndScene( );
    }
    HRESULT result = this->m_device->Present( nullptr, nullptr, nullptr, nullptr );

    // Handle loss of D3D9 device
    if ( result == D3DERR_DEVICELOST && this->m_device->TestCooperativeLevel( ) == D3DERR_DEVICENOTRESET )
        this->reset_device( );
}

void c_gui::start( )
{
    this->m_window_class = { sizeof( WNDCLASSEX ), CS_CLASSDC, this->wnd_proc, 0L, 0L, GetModuleHandle( nullptr ), nullptr, nullptr, nullptr, nullptr, _T( "ImGui Example" ), nullptr };
    ::RegisterClassEx( &this->m_window_class );
    this->m_window = ::CreateWindow( this->m_window_class.lpszClassName, _T( "clicker" ), WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, 100, 100, 400, 300, nullptr, nullptr, this->m_window_class.hInstance, nullptr );
    SetWindowLong( this->m_window, GWL_STYLE, GetWindowLong( this->m_window, GWL_STYLE ) & ~WS_MAXIMIZEBOX );

    if ( !this->create_device( this->m_window ) )
    {
        this->cleanup_device( );
        ::UnregisterClass( this->m_window_class.lpszClassName, this->m_window_class.hInstance );
        return;
    }

    ::ShowWindow( this->m_window, SW_SHOWDEFAULT );
    ::UpdateWindow( this->m_window );

    IMGUI_CHECKVERSION( );
    ImGui::CreateContext( );
    ImGuiIO& io = ImGui::GetIO( ); ( void ) io;

    ImGui::StyleColorsDark( );

    ImGui_ImplWin32_Init( this->m_window );
    ImGui_ImplDX9_Init( this->m_device );
}

bool c_gui::should_shutdown( )
{
    return this->m_should_shutdown;
}

void c_gui::render( )
{
    this->pre_frame( );
    this->frame( );
    this->post_frame( );
}

void c_gui::shutdown( )
{
    ImGui_ImplDX9_Shutdown( );
    ImGui_ImplWin32_Shutdown( );
    ImGui::DestroyContext( );

    this->cleanup_device( );
    ::DestroyWindow( this->m_window );
    ::UnregisterClass( this->m_window_class.lpszClassName, this->m_window_class.hInstance );
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

LRESULT __stdcall c_gui::handle_wndproc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    if ( ImGui_ImplWin32_WndProcHandler( hwnd, msg, wparam, lparam ) )
        return true;

    switch ( msg )
    {
    case WM_SIZE:
        if ( this->m_device != nullptr && wparam != SIZE_MINIMIZED )
        {
            this->m_params.BackBufferWidth = LOWORD( lparam );
            this->m_params.BackBufferHeight = HIWORD( lparam );
            this->reset_device( );
        }
        return 0;
    case WM_SYSCOMMAND:
        if ( ( wparam & 0xfff0 ) == SC_KEYMENU )
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage( 0 );
        return 0;
    }
    return ::DefWindowProc( hwnd, msg, wparam, lparam );
}
