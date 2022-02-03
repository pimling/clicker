#pragma once

#include <random>

class c_config
{
public:
	float m_cps;
	float m_std;
	bool m_enabled = false;
};

inline auto g_config = c_config{ };

class c_clicker
{
	int m_delay;
	std::normal_distribution<float> m_dist_cache{ 0.f, 1.f };
	std::mt19937 m_random_gen;

public:
	c_clicker( );

	static void start( HWND mc_window );
	void calculate_delay( );
	void send_click( HWND mc_window );
};

inline auto g_clicker = c_clicker{ };