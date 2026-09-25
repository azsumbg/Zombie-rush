#pragma once

#ifdef ZOMBIES_EXPORTS
#define ZOMBIES_API __declspec(dllexport)
#else 
#define ZOMBIES_API __declspec(dllimport)
#endif

#include <d2d1.h>
#include "ccontainer.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "ccontainer.lib")


constexpr float scr_width{ 800.0f };
constexpr float scr_height{ 800.0f };

constexpr float sky{ 50.0f };
constexpr float ground{ 750.0f };

constexpr D2D1_RECT_F FULL_SCREEN{ 0, 0, 800.0f, 800.0f };
constexpr D2D1_RECT_F GAME_SCREEN{ 0, 50.0f, 800.0f, 750.0f };
constexpr D2D1_RECT_F MENU_BAR{ 0, 0, 800.0f, 50.0f };
constexpr D2D1_RECT_F STATUS_BAR{ 0, 750.0f, 800.0f, 800.0f };

enum class background { ocean = 0, intro = 1, pause = 2 };
enum class creature { zombie1 = 0, zombie2 = 1, zombie3 = 2, warrior = 3, mage = 4 };
enum class portals { warrior_portal = 0, mage_portal = 1};


namespace zombie
{
	class ZOMBIES_API PROTON
	{
	protected:
		float _width{ 1.0f };
		float _height{ 1.0f };

	public:
		D2D1_POINT_2F start{};
		D2D1_POINT_2F end{};
		D2D1_POINT_2F center{};

		float x_rad{};
		float y_rad{};

		D2D1_RECT_F rect{};

		PROTON();
		PROTON(float s_x, float s_y);
		PROTON(float s_x, float s_y, float s_width, float s_height);
		PROTON(D2D1_POINT_2F up_left, D2D1_POINT_2F down_right);

		virtual ~PROTON() {};

		float get_width() const;
		float get_height() const;

		void set_edges();

		void set_width(float new_width);
		void set_height(float new_height);
		void new_dims(float new_width, float new_height);
	};

	class ZOMBIES_API BACKGROUND :public PROTON
	{
	private:
		background type{ background::intro };

		int frame = 0;
		int frame_delay = 0;
		int max_frames = 0;
		int max_frame_delay = 0;

	public:

		BACKGROUND(background what_type);

		int get_frame();

		background get_type()const;
	};

	class ZOMBIES_API FIELD :public PROTON
	{
	private:
		float speed = 1.0f;

		FIELD(float _sy);

	public:

		bool move(float gear);

		static FIELD* create(float sy);

		void Release();
	};

	class ZOMBIES_API SHOT :public PROTON
	{
	private: 
		float speed = 5.0f;

		int frame = 0;
		int max_frames = 3;
		int frame_delay = 20;

		float move_sx{ 0 };
		float move_sy{ 0 };
		float move_ex{ 0 };
		float move_ey{ 0 };

		float vector_next_x{ 0 };
		float vector_next_y{ 0 };

		SHOT(float _sx, float _sy, float _ex, float _ey);

	public:

		bool move(float gear);

		int get_frame();

		void Release();

		static SHOT* create(float sx, float sy, float ex, float ey);
	};

	class ZOMBIES_API CREATURE :public PROTON
	{
	private:

		creature type{ creature::warrior };

		float speed = 1.0f;

		int frame = 0;
		int max_frames = 0;
		int frame_delay = 0;
		int max_frame_delay = 0;

		int attack_delay = 0;
		int max_attack_delay = 0;

		float x_delta = 0;
		float y_delta = 0;

		int damage = 0;

		CREATURE(creature _what, float _sx, float _sy);

	public:
		
		int lifes = 0;

		void path_info(float ex, float ey);

		int get_frame();

		void move(float gear);

		int attack();

		void change_warrior_type();

		creature get_type() const;

		void Release();

		static CREATURE* create(creature what, float sx, float sy);
	};

	class ZOMBIES_API PORTAL :public PROTON
	{
	private:
		portals type{ portals::warrior_portal };
		float speed{ 1.0f };

		PORTAL(portals _what, float _sx, float _sy);

	public:

		portals get_type()const;

		bool move(float gear);

		void Release();

		static PORTAL* create(portals what, float sx, float sy);
	};

	// FUNCTIONS ***************************************

	D2D1_POINT_2F ZOMBIES_API find_nearest(contlib::BAG<D2D1_POINT_2F>& enemies, D2D1_RECT_F ref);

	bool ZOMBIES_API Intersect(D2D1_RECT_F first, D2D1_RECT_F second);
}