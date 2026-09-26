#include "framework.h"
#include "Zombie rush.h"

#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include "ccontainer.h"
#include "D2BMPLOADER.h"
#include "ErrH.h"
#include "FCheck.h"
#include "gifresizer.h"
#include "zombies.h"
#include <chrono>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "ccontainer.lib")
#pragma comment(lib, "d2bmploader.lib")
#pragma comment(lib, "errh.lib")
#pragma comment(lib, "fcheck.lib")
#pragma comment(lib, "gifresizer.lib")
#pragma comment(lib, "zombies.lib")

constexpr wchar_t bWinClassName[]{ L"ZombRush" };

constexpr char tmp_file[](".\\res\\data\\temp.dat");
constexpr wchar_t Ltmp_file[](L".\\res\\data\\temp.dat");
constexpr wchar_t help_file[](L".\\res\\data\\help.dat");
constexpr wchar_t record_file[](L".\\res\\data\\record.dat");
constexpr wchar_t save_file[](L".\\res\\data\\save.dat");
constexpr wchar_t sound_file[](L".\\res\\snd\\main.wav");

constexpr int mNew{ 1001 };
constexpr int mLvl{ 1002 };
constexpr int mExit{ 1003 };
constexpr int mSave{ 1004 };
constexpr int mLoad{ 1005 };
constexpr int mHoF{ 1006 };

constexpr int first_record{ 2001 };
constexpr int no_record{ 2002 };
constexpr int record{ 2003 };

WNDCLASS bWinClass{};
HINSTANCE bIns{ nullptr };
HWND bHwnd{ nullptr };
HMENU bBar{ nullptr };
HMENU bMain{ nullptr };
HMENU bStore{ nullptr };
HICON mainIcon{ nullptr };
HCURSOR mainCursor{ nullptr };
HCURSOR outCursor{ nullptr };
MSG bMsg{};
BOOL bRet{ 0 };

POINT cur_pos{};

UINT bTimer{};

D2D1_RECT_F b1Rect{ 20.0f, 10.0f, scr_width / 3.0f - 20.0f, 40.0f };
D2D1_RECT_F b2Rect{ scr_width / 3.0f + 20.0f, 10.0f, scr_width * 2.0f / 3.0f - 20.0f , 40.0f };
D2D1_RECT_F b3Rect{ scr_width * 2.0f / 3.0f + 20.0f, 10.0f, scr_width - 20.0f , 40.0f };

D2D1_RECT_F b1TxtRect{ 35.0f, 15.0f, scr_width / 3.0f - 50.0f, 40.0f };
D2D1_RECT_F b2TxtRect{ scr_width / 3.0f + 60.0f, 15.0f, scr_width * 2.0f / 3.0f - 20.0f , 40.0f };
D2D1_RECT_F b3TxtRect{ scr_width * 2.0f / 3.0f + 50.0f, 15.0f, scr_width - 20.0f , 40.0f };

bool pause = false;
bool sound = true;
bool in_client = true;
bool show_help = false;
bool b1Hglt = false;
bool b2Hglt = false;
bool b3Hglt = false;

bool name_set = false;

bool castle_active = false;
bool castle_demolished = false;
D2D1_RECT_F CastleRect{};
D2D1_RECT_F ExplosionRect{};

bool level_skipped = false;

wchar_t current_player[16]{ L"TARLYO" };

float scale_x{ 0 };
float scale_y{ 0 };

int castle_lifes{ 250 };

float level{ 1.0f };
int score{ 0 };

float distance{};

ID2D1Factory* iFactory{ nullptr };
ID2D1HwndRenderTarget* Draw{ nullptr };

ID2D1SolidColorBrush* statBrush{ nullptr };
ID2D1SolidColorBrush* inactBrush{ nullptr };
ID2D1SolidColorBrush* txtBrush{ nullptr };
ID2D1SolidColorBrush* hgltBrush{ nullptr };

ID2D1RadialGradientBrush* b1BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b2BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b3BckgBrush{ nullptr };

IDWriteFactory* iWriteFactory{ nullptr };
IDWriteTextFormat* nrmText{ nullptr };
IDWriteTextFormat* midText{ nullptr };
IDWriteTextFormat* bigText{ nullptr };

ID2D1Bitmap* bmpCastle{ nullptr };
ID2D1Bitmap* bmpEnergyField1{ nullptr };
ID2D1Bitmap* bmpEnergyField2{ nullptr };
ID2D1Bitmap* bmpSand{ nullptr };

ID2D1Bitmap* logoFirstScreen{ nullptr };
ID2D1Bitmap* logoLevel{ nullptr };
ID2D1Bitmap* logoLoose{ nullptr };
ID2D1Bitmap* logoWin{ nullptr };
ID2D1Bitmap* logoRecord{ nullptr };

ID2D1Bitmap* bmpZombie1[16]{ nullptr };
ID2D1Bitmap* bmpZombie2[14]{ nullptr };
ID2D1Bitmap* bmpZombie3[21]{ nullptr };

ID2D1Bitmap* bmpExplosion[24]{ nullptr };

ID2D1Bitmap* bmpVitWarrior[8]{ nullptr };
ID2D1Bitmap* bmpFallWarrior[8]{ nullptr };

ID2D1Bitmap* bmpVitMage[8]{ nullptr };
ID2D1Bitmap* bmpFallMage[8]{ nullptr };

ID2D1Bitmap* bmpIntro[17]{ nullptr };
ID2D1Bitmap* bmpOcean[20]{ nullptr };
ID2D1Bitmap* bmpPause[4]{ nullptr };
ID2D1Bitmap* bmpShot[4]{ nullptr };

/////////////////////////////////////////////////////////

contlib::RAND RandIt{};

contlib::BAG<zombie::FIELD*>vSands;
zombie::BACKGROUND Ocean(background::ocean);
zombie::BACKGROUND Intro(background::intro);
zombie::BACKGROUND Pause(background::pause);

contlib::BAG<zombie::CREATURE*>vGoods;
contlib::BAG<zombie::CREATURE*>vEvils;

contlib::BAG<zombie::SHOT*>vShots;

contlib::BAG<zombie::PORTAL*>vPortals;

/////////////////////////////////////////////////////////

int ExplosionFrame()
{
	static int frame_delay = 3;
	static int frame = 0;

	--frame_delay;
	if (frame_delay <= 0)
	{
		frame_delay = 3;
		++frame;
		if (frame > 23)return -1;
	}

	return frame;
}

template<typename T>concept HasRelease = requires(T var)
{
	var.Release();
};
template<HasRelease T>bool FreeMem(T** var)
{
	if ((*var))
	{
		(*var)->Release();
		(*var) = nullptr;
		return true;
	}

	return false;
}
void LogErr(const wchar_t* what)
{
	std::wofstream err(L".\\res\\data\\error.log", std::ios::app);
	err << what << L" time stamp: " << std::chrono::system_clock::now() << std::endl;
	err.close();
}
void FreeResources()
{
	if (!FreeMem(&iFactory))LogErr(L"Error releasing main D2D1Factory !");
	if (!FreeMem(&Draw))LogErr(L"Error releasing main D2D1HwndRenderTarget !");

	if (!FreeMem(&statBrush))LogErr(L"Error releasing main D2D1 statBrush !");
	if (!FreeMem(&txtBrush))LogErr(L"Error releasing main D2D1 txtBrush !");
	if (!FreeMem(&hgltBrush))LogErr(L"Error releasing main D2D1 hgltBrush !");
	if (!FreeMem(&inactBrush))LogErr(L"Error releasing main D2D1 inactBrush !");

	if (!FreeMem(&b1BckgBrush))LogErr(L"Error releasing main D2D1 b1BckgBrush !");
	if (!FreeMem(&b2BckgBrush))LogErr(L"Error releasing main D2D1 b2BckgBrush !");
	if (!FreeMem(&b3BckgBrush))LogErr(L"Error releasing main D2D1 b3BckgBrush !");

	if (!FreeMem(&iWriteFactory))LogErr(L"Error releasing main D2D1WriteFactory !");
	if (!FreeMem(&nrmText))LogErr(L"Error releasing main nrmText !");
	if (!FreeMem(&midText))LogErr(L"Error releasing main midText !");
	if (!FreeMem(&bigText))LogErr(L"Error releasing main bigText !");

	if (!FreeMem(&bmpCastle))LogErr(L"Error releasing bmpCastle !");
	if (!FreeMem(&bmpEnergyField1))LogErr(L"Error releasing bmpEnergyField1 !");
	if (!FreeMem(&bmpEnergyField2))LogErr(L"Error releasing bmpEnergyField2 !");
	if (!FreeMem(&bmpSand))LogErr(L"Error releasing bmpSand !");

	if (!FreeMem(&logoFirstScreen))LogErr(L"Error releasing logoFirstScreen !");
	if (!FreeMem(&logoLevel))LogErr(L"Error releasing logoLevel !");
	if (!FreeMem(&logoLoose))LogErr(L"Error releasing logoLoose !");
	if (!FreeMem(&logoWin))LogErr(L"Error releasing logoWin !");
	if (!FreeMem(&logoRecord))LogErr(L"Error releasing logoRecord !");

	for (int i = 0; i < 16; ++i)if (!FreeMem(&bmpZombie1[i]))LogErr(L"Error releasing bmpZombie1 !");
	for (int i = 0; i < 14; ++i)if (!FreeMem(&bmpZombie2[i]))LogErr(L"Error releasing bmpZombie2 !");
	for (int i = 0; i < 21; ++i)if (!FreeMem(&bmpZombie3[i]))LogErr(L"Error releasing bmpZombie3 !");

	for (int i = 0; i < 24; ++i)if (!FreeMem(&bmpExplosion[i]))LogErr(L"Error releasing bmpExplosion !");

	for (int i = 0; i < 8; ++i)if (!FreeMem(&bmpVitWarrior[i]))LogErr(L"Error releasing bmpVitWarrior !");
	for (int i = 0; i < 8; ++i)if (!FreeMem(&bmpFallWarrior[i]))LogErr(L"Error releasing bmpFallWarrior !");

	for (int i = 0; i < 8; ++i)if (!FreeMem(&bmpVitMage[i]))LogErr(L"Error releasing bmpVitMage !");
	for (int i = 0; i < 8; ++i)if (!FreeMem(&bmpFallMage[i]))LogErr(L"Error releasing bmpFallMage !");

	for (int i = 0; i < 17; ++i)if (!FreeMem(&bmpIntro[i]))LogErr(L"Error releasing bmpIntro !");
	for (int i = 0; i < 20; ++i)if (!FreeMem(&bmpOcean[i]))LogErr(L"Error releasing bmpOcean !");
	for (int i = 0; i < 4; ++i)if (!FreeMem(&bmpPause[i]))LogErr(L"Error releasing bmpPause !");
	for (int i = 0; i < 4; ++i)if (!FreeMem(&bmpShot[i]))LogErr(L"Error releasing bmpShot !");
}
void ErrExit(int what)
{
	MessageBeep(MB_ICONERROR);
	MessageBox(NULL, ErrHandle(what), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);

	FreeResources();
	std::remove(tmp_file);
	exit(1);
}

void GameOver()
{
	KillTimer(bHwnd, bTimer);

	PlaySound(NULL, NULL, NULL);


	bMsg.message = WM_QUIT;
	bMsg.wParam = 0;
}
void InitGame()
{
	level = 1.0f;
	score = 0;
	wcscpy_s(current_player, L"TARLYO");
	name_set = false;

	distance = 300.0f;
	castle_lifes = 250;
	castle_active = false;
	castle_demolished = false;
	level_skipped = false;

	if (!vSands.empty())for (int i = 0; i < vSands.size(); ++i)FreeMem(&vSands[i]);
	vSands.clear();
	for (float ty = -700.0f; ty <= 700.0f; ty += 700.0f)vSands.push_back(zombie::FIELD::create(ty));

	if (!vGoods.empty())for (int i = 0; i < vGoods.size(); ++i)FreeMem(&vGoods[i]);
	vGoods.clear();

	float good_x{ 150.0f + RandIt(0.0f, 20.0f) };
	float good_y{ ground - 60.0f };

	for (float row = 0; row < 3.0f; ++row)
	{
		for (float col = 0; col < 10.0f; ++col)
		{
			good_x += 20.0f + RandIt(0.0f, 20.0f);
			vGoods.push_back(zombie::CREATURE::create(creature::warrior, good_x, good_y));

			vGoods.back()->path_info(vGoods.back()->center.x, sky);
		}

		good_x = 150.0f + RandIt(0.0f, 20.0f);
		good_y -= 40.0f;
	}

	if (!vEvils.empty())for (int i = 0; i < vEvils.size(); ++i)FreeMem(&vEvils[i]);
	vEvils.clear();
	
	float evil_x{ 150.0f + RandIt(0.0f, 20.0f) };
	float evil_y{ sky + 5.0f };

	for (float row = 0; row < 3.0f; ++row)
	{
		for (float col = 0; col < 8.0f; ++col)
		{
			int ttype = RandIt(0, 2);

			evil_x += 20.0f + RandIt(0.0f, 20.0f);
			vEvils.push_back(zombie::CREATURE::create(static_cast<creature>(ttype), evil_x, evil_y));

			vEvils.back()->path_info(vEvils.back()->center.x, ground);
		}

		evil_x = 150.0f + RandIt(0.0f, 20.0f);
		evil_y += 30.0f;
	}
	
	if (!vShots.empty())for (int i = 0; i < vShots.size(); ++i)FreeMem(&vShots[i]);
	vShots.clear();

	if (!vPortals.empty())for (int i = 0; i < vPortals.size(); ++i)FreeMem(&vPortals[i]);
	vPortals.clear();
}
void LevelUp()
{
	Draw->BeginDraw();
	Draw->DrawBitmap(logoLevel, FULL_SCREEN);
	if (!level_skipped)
	{
		score += (int)(vGoods.size());
		
		wchar_t txt[40]{ L"БОНУС: " };
		wchar_t add[5]{ L"\0" };
		int size = 0;

		wsprintf(add, L"%d", (int)(vGoods.size()));
		wcscat_s(txt, add);
		
		for (int i = 0; i < 40; ++i)
		{
			if (txt[i] != '\0')++size;
			else break;
		}

		if (bigText && hgltBrush)Draw->DrawTextW(txt, size, bigText, D2D1::RectF(150.0f, scr_height / 2.0f, scr_width,
			scr_height), hgltBrush);
	}
	else if (bigText && hgltBrush)Draw->DrawTextW(L"ПРЕСКОЧЕНО НИВО !", 18, bigText, D2D1::RectF(150.0f, scr_height / 2.0f,
		scr_width, scr_height), hgltBrush);
	Draw->EndDraw();

	if (sound)mciSendString(L"play .\\res\\snd\\levelup.wav", NULL, NULL, NULL);

	Sleep(3000);

	++level;
	distance = 300.0f + 10.0f * level;
	castle_lifes = 250;
	castle_active = false;
	castle_demolished = false;
	level_skipped = false;

	if (!vSands.empty())for (int i = 0; i < vSands.size(); ++i)FreeMem(&vSands[i]);
	vSands.clear();

	if (!vGoods.empty())for (int i = 0; i < vGoods.size(); ++i)FreeMem(&vGoods[i]);
	vGoods.clear();

	float good_x{ 150.0f + RandIt(0.0f, 20.0f) };
	float good_y{ ground - 60.0f };

	for (float row = 0; row < 3.0f + level; ++row)
	{
		for (float col = 0; col < 10.0f + level; ++col)
		{
			good_x += 20.0f + RandIt(0.0f, 20.0f);
			vGoods.push_back(zombie::CREATURE::create(creature::warrior, good_x, good_y));

			vGoods.back()->path_info(vGoods.back()->center.x, sky);
		}

		good_x = 150.0f + RandIt(0.0f, 20.0f);
		good_y -= 40.0f;
	}

	if (!vEvils.empty())for (int i = 0; i < vEvils.size(); ++i)FreeMem(&vEvils[i]);
	vEvils.clear();

	float evil_x{ 150.0f + RandIt(0.0f, 20.0f) };
	float evil_y{ sky + 5.0f };

	for (float row = 0; row < 3.0f + level ; ++row)
	{
		for (float col = 0; col < 8.0f + level; ++col)
		{
			int ttype = RandIt(0, 2);

			evil_x += 20.0f + RandIt(0.0f, 20.0f);
			vEvils.push_back(zombie::CREATURE::create(static_cast<creature>(ttype), evil_x, evil_y));

			vEvils.back()->path_info(vEvils.back()->center.x, ground);
		}

		evil_x = 150.0f + RandIt(0.0f, 20.0f);
		evil_y += 30.0f;
	}

	for (float ty = -700.0f; ty <= 700.0f; ty += 700.0f)vSands.push_back(zombie::FIELD::create(ty));

	if (!vShots.empty())for (int i = 0; i < vShots.size(); ++i)FreeMem(&vShots[i]);
	vShots.clear();

	if (!vPortals.empty())for (int i = 0; i < vPortals.size(); ++i)FreeMem(&vPortals[i]);
	vPortals.clear();
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
	switch (ReceivedMsg)
	{
	case WM_INITDIALOG:
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)(mainIcon));
		return true;

	case WM_CLOSE:
		EndDialog(hwnd, IDCANCEL);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;

		case IDOK:
			if (GetDlgItemTextW(hwnd, IDC_NAME, current_player, 16) < 1)
			{
				wcscpy_s(current_player, L"TARLYO");

				if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
				MessageBox(bHwnd, L"Ха, ха, ха ! Забрави си името !", L"Забраватор !", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);

				EndDialog(hwnd, IDCANCEL);
				break;
			}
			EndDialog(hwnd, IDOK);
			break;
		}
		break;
	}

	return (INT_PTR)(FALSE);
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
	switch (ReceivedMsg)
	{
	case WM_CREATE:
		if (bIns)
		{
			SetTimer(hwnd, bTimer, 1000, NULL);

			bBar = CreateMenu();
			bMain = CreateMenu();
			bStore = CreateMenu();

			AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
			AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bStore), L"Меню за данни");
		
			AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
			AppendMenu(bMain, MF_STRING, mLvl, L"Следващо ниво");
			AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
			AppendMenu(bMain, MF_STRING, mExit, L"Изход");
		
			AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
			AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
			AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
			AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");

			SetMenu(hwnd, bBar);
		
			InitGame();
		}
		break;

	case WM_CLOSE:
		pause = true;
		if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
		if (MessageBox(hwnd, L"Ако излезеш, губиш прогреса по тази игра !\n\nНаистина ли излизаш ?",
			L"Изход !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
		{
			pause = false;
			break;
		}
		GameOver();
		break;

	case WM_TIMER:
		if (pause || castle_active)break;
		distance -= (1.0f + RandIt(0.2f, 0.4f));
		if (distance <= 0)
		{
			castle_active = true;
			CastleRect.left = 150.0f + RandIt( 0.0f,300.0f );
			CastleRect.top = 60.0f;
			CastleRect.right = CastleRect.left + 150.0f;
			CastleRect.bottom = CastleRect.top + 120.0f;
		}
		break;

	case WM_SETCURSOR:
		GetCursorPos(&cur_pos);
		ScreenToClient(hwnd, &cur_pos);
		if (LOWORD(lParam) == HTCLIENT)
		{
			if (!in_client)
			{
				in_client = true;
				pause = false;
			}

			if (cur_pos.y * scale_y <= 50)
			{
				if (cur_pos.x * scale_x >= b1Rect.left && cur_pos.x * scale_x <= b1Rect.right)
				{
					if (!b1Hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1Hglt = true;
						b2Hglt = false;
						b3Hglt = false;
					}
				}
				else if (cur_pos.x * scale_x >= b2Rect.left && cur_pos.x * scale_x <= b2Rect.right)
				{
					if (!b2Hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1Hglt = false;
						b2Hglt = true;
						b3Hglt = false;
					}
				}
				else if (cur_pos.x * scale_x >= b3Rect.left && cur_pos.x * scale_x <= b3Rect.right)
				{
					if (!b3Hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1Hglt = false;
						b2Hglt = false;
						b3Hglt = true;
					}
				}
				else if (b1Hglt || b2Hglt || b3Hglt)
				{
					if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
					b1Hglt = false;
					b2Hglt = false;
					b3Hglt = false;
				}

				SetCursor(outCursor);
				return true;
			}
			else if (b1Hglt || b2Hglt || b3Hglt)
			{
				if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
				b1Hglt = false;
				b2Hglt = false;
				b3Hglt = false;
			}

			SetCursor(mainCursor);

			return true;
		}
		else
		{
			if (in_client)
			{
				in_client = false;
				pause = true;
			}
			if (b1Hglt || b2Hglt || b3Hglt)
			{
				if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
				b1Hglt = false;
				b2Hglt = false;
				b3Hglt = false;
			}
		
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			return true;
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case mNew:
			pause = true;
			if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
			if (MessageBox(hwnd, L"Ако рестартираш, губиш прогреса по тази игра !\n\nНаистина ли рестартираш ?",
				L"Рестарт !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
			{
				pause = false;
				break;
			}
			InitGame();
			break;

		case mLvl:
			pause = true;
			if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
			if (MessageBox(hwnd, L"Ако прескочиш нивото, губиш прогреса по него !\n\nНаистина ли прескачаш ?",
				L"Прескочи ниво !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
			{
				pause = false;
				break;
			}
			level_skipped = true;
			LevelUp();
			break;

		case mExit:
			SendMessage(hwnd, WM_CLOSE, NULL, NULL);
			break;


		}
		break;

	case WM_KEYDOWN:
		if (!vGoods.empty())
		{
			switch (wParam)
			{
			case VK_LEFT:
				for (int i = 0; i < vGoods.size(); ++i)vGoods[i]->path_info(150.0f, vGoods[i]->start.y);
				break;

			case VK_RIGHT:
				for (int i = 0; i < vGoods.size(); ++i)vGoods[i]->path_info(650.0f, vGoods[i]->start.y);
				break;

			case VK_UP:
				for (int i = 0; i < vGoods.size(); ++i)vGoods[i]->path_info(vGoods[i]->center.x, sky);
				break;

			case VK_DOWN:
				for (int i = 0; i < vGoods.size(); ++i)vGoods[i]->path_info(vGoods[i]->center.x, ground);
				break;
			}
		}
		break;

	default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
	}

	return (LRESULT)(FALSE);
}

void CreateResources()
{
	int result = 0;
	CheckFile(Ltmp_file, &result);
	if (result == FILE_EXIST)ErrExit(eStarted);
	else
	{
		std::wofstream start{ Ltmp_file };
		start << L"Game started at: " << std::chrono::system_clock::now();
		start.close();
	}

	int win_x = (int)(GetSystemMetrics(SM_CXSCREEN) / 2 - (int)(scr_width / 2.0f));
	int win_y = 10;

	if (GetSystemMetrics(SM_CXSCREEN) < win_x + (int)(scr_width)
		|| GetSystemMetrics(SM_CYSCREEN) < win_y + (int)(scr_height))ErrExit(eScreen);

	mainIcon = (HICON)(LoadImage(NULL, L".\\res\\main.ico", IMAGE_ICON, 80, 97, LR_LOADFROMFILE));
	if (!mainIcon)ErrExit(eIcon);

	mainCursor = LoadCursorFromFileW(L".\\res\\bcursor.ani");
	outCursor = LoadCursorFromFileW(L".\\res\\out.ani");
	if (!mainCursor || !outCursor)ErrExit(eCursor);

	bWinClass.lpszClassName = bWinClassName;
	bWinClass.hInstance = bIns;
	bWinClass.lpfnWndProc = &WinProc;
	bWinClass.hbrBackground = CreateSolidBrush(RGB(10, 10, 10));
	bWinClass.hIcon = mainIcon;
	bWinClass.hCursor = mainCursor;
	bWinClass.style = CS_DROPSHADOW;

	if (!RegisterClass(&bWinClass))ErrExit(eClass);

	bHwnd = CreateWindow(bWinClassName, L"ЗОМБИТАТА АТАКУВАТ !", WS_CAPTION | WS_SYSMENU, win_x, win_y,
		(int)(scr_width), (int)(scr_height), NULL, NULL, bIns, NULL);
	if (!bHwnd)ErrExit(eWindow);
	else
	{
		ShowWindow(bHwnd, SW_SHOWDEFAULT);

		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
		if (hr != S_OK)
		{
			LogErr(L"Error creating main D2D1 Factory !");
			ErrExit(eD2D);
		}

		if (iFactory)
		{
			hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(bHwnd,
				D2D1::SizeU((UINT32)(scr_width), (UINT32)(scr_height))), &Draw);
			if (hr != S_OK)
			{
				LogErr(L"Error creating main D2D1 HwndRenderTarget !");
				ErrExit(eD2D);
			}
		}

		if (Draw)
		{
			RECT DPIRect{};
			GetClientRect(bHwnd, &DPIRect);

			D2D1_SIZE_F DIPDims{ Draw->GetSize() };

			scale_x = DIPDims.width / (DPIRect.right - DPIRect.left);
			scale_y = DIPDims.height / (DPIRect.bottom - DPIRect.top);

			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Maroon), &statBrush);
			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkBlue), &txtBrush);
			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange), &hgltBrush);
			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::IndianRed), &inactBrush);

			if (hr != S_OK)
			{
				LogErr(L"Error creating main D2D1 Solid Color Brushes !");
				ErrExit(eD2D);
			}

			D2D1_GRADIENT_STOP gStops[2]{};
			ID2D1GradientStopCollection* gColl{ nullptr };

			gStops[0].position = 0;
			gStops[0].color = D2D1::ColorF(D2D1::ColorF::NavajoWhite);
			gStops[1].position = 1.0f;
			gStops[1].color = D2D1::ColorF(D2D1::ColorF::DarkCyan);

			hr = Draw->CreateGradientStopCollection(gStops, 2, &gColl);
			if (hr != S_OK)
			{
				LogErr(L"Error creating main D2D1 GradiantStopCollection !");
				ErrExit(eD2D);
			}

			if (gColl)
			{
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b1Rect.left +
					(b1Rect.right - b1Rect.left) / 2.0f, 25.0f), D2D1::Point2F(0, 0), (b1Rect.right - b1Rect.left) / 2.0f, 25.0f),
					gColl, &b1BckgBrush);
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b2Rect.left +
					(b2Rect.right - b2Rect.left) / 2.0f, 25.0f), D2D1::Point2F(0, 0), (b2Rect.right - b2Rect.left) / 2.0f, 25.0f),
					gColl, &b2BckgBrush);
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b3Rect.left +
					(b3Rect.right - b3Rect.left) / 2.0f, 25.0f), D2D1::Point2F(0, 0), (b3Rect.right - b3Rect.left) / 2.0f, 25.0f),
					gColl, &b3BckgBrush);

				if (hr != S_OK)
				{
					LogErr(L"Error creating main D2D1 Radial Gradient Brushes!");
					ErrExit(eD2D);
				}

				if (!FreeMem(&gColl))
				{
					LogErr(L"Error releasing main D2D1 GradiantStopCollection !");
					ErrExit(eD2D);
				}
			}

			bmpCastle = Load(L".\\res\\img\\castle.png", Draw);
			if (!bmpCastle)
			{
				LogErr(L"Error loading bmpCastle !");
				ErrExit(eD2D);
			}
			bmpEnergyField1 = Load(L".\\res\\img\\EnergyField1.png", Draw);
			if (!bmpEnergyField1)
			{
				LogErr(L"Error loading bmpEnergyField1 !");
				ErrExit(eD2D);
			}
			bmpEnergyField2 = Load(L".\\res\\img\\EnergyField2.png", Draw);
			if (!bmpEnergyField2)
			{
				LogErr(L"Error loading bmpEnergyField2 !");
				ErrExit(eD2D);
			}
			bmpSand = Load(L".\\res\\img\\Sand.png", Draw);
			if (!bmpSand)
			{
				LogErr(L"Error loading bmpSand !");
				ErrExit(eD2D);
			}

			logoFirstScreen = Load(L".\\res\\img\\logos\\first.png", Draw);
			if (!logoFirstScreen)
			{
				LogErr(L"Error loading logoFirstScreen !");
				ErrExit(eD2D);
			}
			logoLevel = Load(L".\\res\\img\\logos\\level.png", Draw);
			if (!logoLevel)
			{
				LogErr(L"Error loading logoLevel !");
				ErrExit(eD2D);
			}
			logoLoose = Load(L".\\res\\img\\logos\\loose.png", Draw);
			if (!logoLoose)
			{
				LogErr(L"Error loading logoLoose !");
				ErrExit(eD2D);
			}
			logoWin = Load(L".\\res\\img\\logos\\win.png", Draw);
			if (!logoWin)
			{
				LogErr(L"Error loading logoWin !");
				ErrExit(eD2D);
			}
			logoRecord = Load(L".\\res\\img\\logos\\record.png", Draw);
			if (!logoRecord)
			{
				LogErr(L"Error loading logoRecord !");
				ErrExit(eD2D);
			}

			for (int i = 0; i < 16; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\evils\\zombie1\\" };
				wchar_t add[3]{ L"\0" };
				
				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpZombie1[i] = Load(name, Draw);

				if (!bmpZombie1[i])
				{
					LogErr(L"Error loading bmpZombie1 !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 14; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\evils\\zombie2\\" };
				wchar_t add[3]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpZombie2[i] = Load(name, Draw);

				if (!bmpZombie2[i])
				{
					LogErr(L"Error loading bmpZombie2 !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 21; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\evils\\zombie3\\" };
				wchar_t add[3]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpZombie3[i] = Load(name, Draw);

				if (!bmpZombie3[i])
				{
					LogErr(L"Error loading bmpZombie3 !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 24; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\explosion\\" };
				wchar_t add[3]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpExplosion[i] = Load(name, Draw);

				if (!bmpExplosion[i])
				{
					LogErr(L"Error loading bmpExplosion !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 8; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\good\\warrior\\vital\\" };
				wchar_t add[3]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpVitWarrior[i] = Load(name, Draw);

				if (!bmpVitWarrior[i])
				{
					LogErr(L"Error loading bmpVitWarrior !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 8; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\good\\warrior\\fallen\\" };
				wchar_t add[3]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpFallWarrior[i] = Load(name, Draw);

				if (!bmpFallWarrior[i])
				{
					LogErr(L"Error loading bmpFallWarrior !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 8; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\good\\mage\\vital\\" };
				wchar_t add[3]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpVitMage[i] = Load(name, Draw);

				if (!bmpVitMage[i])
				{
					LogErr(L"Error loading bmpVitMage !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 8; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\good\\mage\\fallen\\" };
				wchar_t add[3]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpFallMage[i] = Load(name, Draw);

				if (!bmpFallMage[i])
				{
					LogErr(L"Error loading bmpFallMage !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 17; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\intro\\" };
				wchar_t add[3]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpIntro[i] = Load(name, Draw);

				if (!bmpIntro[i])
				{
					LogErr(L"Error loading bmpIntro !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 20; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\ocean\\" };
				wchar_t add[3]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpOcean[i] = Load(name, Draw);

				if (!bmpOcean[i])
				{
					LogErr(L"Error loading bmpOcean !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 4; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\pause\\" };
				wchar_t add[3]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpPause[i] = Load(name, Draw);

				if (!bmpPause[i])
				{
					LogErr(L"Error loading bmpPause !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 4; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\shot\\" };
				wchar_t add[3]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpShot[i] = Load(name, Draw);

				if (!bmpShot[i])
				{
					LogErr(L"Error loading bmpShot !");
					ErrExit(eD2D);
				}
			}
		}

		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&iWriteFactory));
		if (hr != S_OK)
		{
			LogErr(L"Error creating main D2D1 Write Factory !");
			ErrExit(eD2D);
		}

		if (iWriteFactory)
		{
			hr = iWriteFactory->CreateTextFormat(L"Segoe script", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL, 16.0f, L"", &nrmText);
			hr = iWriteFactory->CreateTextFormat(L"Segoe script", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL, 24.0f, L"", &midText);
			hr = iWriteFactory->CreateTextFormat(L"Segoe script", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL, 72.0f, L"", &bigText);
			if (hr != S_OK)
			{
				LogErr(L"Error creating main D2D1 Write Factory Text Formats!");
				ErrExit(eD2D);
			}
		}
	}

	PlaySound(L".\\res\\snd\\intro.wav", NULL, SND_ASYNC);

	for (int i = 0; i < 240; ++i)
	{
		Draw->BeginDraw();
		Draw->DrawBitmap(bmpIntro[Intro.get_frame()], FULL_SCREEN);
		Draw->DrawBitmap(logoFirstScreen, FULL_SCREEN);
		Draw->EndDraw();
	}

	PlaySound(L".\\res\\snd\\boom.wav", NULL, SND_SYNC);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	bIns = hInstance;
	if (!bIns)
	{
		LogErr(L"Error in Windows hInstance !");
		ErrExit(eClass);
	}

	CreateResources();



	while (bMsg.message != WM_QUIT)
	{
		if ((bRet = PeekMessage(&bMsg, NULL, NULL, NULL, PM_REMOVE)) != 0)
		{
			if (bRet == -1)ErrExit(eMsg);

			TranslateMessage(&bMsg);
			DispatchMessage(&bMsg);
		}

		if (pause)
		{
			if (show_help)continue;

			if (bigText && txtBrush)
			{
				Draw->BeginDraw();
				Draw->DrawBitmap(bmpPause[Pause.get_frame()], FULL_SCREEN);
				Draw->DrawTextW(L"ПАУЗА", 6, bigText, D2D1::RectF(scr_width / 2.0f - 100.0f, scr_height / 2.0f - 50.0f,
					scr_width, scr_height), txtBrush);
				Draw->EndDraw();
			}
	
			continue;
		}
		
	/////////////////////////////////////////////////////////////
	
	// GAME ACTION *********************************************
	
		// FIELD MOVING
	
		if (!vSands.empty() && !castle_active)
		{
			for (contlib::BAG<zombie::FIELD*>::iterator field = vSands.begin(); field < vSands.end(); ++field)
			{
				if (!(*field)->move(level))
				{
					(*field)->Release();
					vSands.erase(field);

					vSands.push_back(zombie::FIELD::create(vSands.front()->start.y - 700.0f));
				}
			}
		}

		///////////////
	
		// GOODS MOVING ***************************************
		
		if (!vGoods.empty())
		{
			for (int i = 0; i < vGoods.size(); ++i)vGoods[i]->move(level);
		}
		
		////////////////////////////////////////////////////// 

		// EVILS MOVING ***************************************

		if (!vEvils.empty())
		{
			for (int i = 0; i < vEvils.size(); ++i)vEvils[i]->move(level);

		}

		if (vEvils.size() < 20 + (int)(level) && RandIt(0, 20) == 10)
		{
			float evil_x{ 150.0f + RandIt(0.0f, 300.0f) };
			float evil_y{ sky + 5.0f };
			int ttype = RandIt(0, 2);

			vEvils.push_back(zombie::CREATURE::create(static_cast<creature>(ttype), evil_x, evil_y));
			
			vEvils.back()->path_info(vEvils.back()->center.x, sky);
		}
	
		////////////////////////////////////////////////////////

		// COMBAT *********************************************
	
		if (!vEvils.empty() && !vGoods.empty())
		{
			bool killed = false;

			for (contlib::BAG<zombie::CREATURE*>::iterator evil = vEvils.begin(); evil < vEvils.end(); ++evil)
			{
				for (contlib::BAG<zombie::CREATURE*>::iterator good = vGoods.begin(); good < vGoods.end(); ++good)
				{
					if (!zombie::Intersect((*good)->rect, (*evil)->rect))
					{
						if (contlib::Distance((*good)->center, (*evil)->center) <= 150.0f)
						{
							(*good)->path_info((*evil)->center.x, (*evil)->center.y);
							(*evil)->path_info((*good)->center.x, (*good)->center.y);
						}
					}
					else
					{
						int zombie_attack = (*evil)->attack();
						int good_attack = (*good)->attack();

						if ((*good)->lifes <= 0)continue;

						if (good_attack > 0)
						{
							if ((*good)->get_type() == creature::mage)
							{
								vShots.push_back(zombie::SHOT::create((*good)->center.x, (*good)->center.y,
									(*evil)->center.x, (*evil)->center.y));
							}
							else
							{
								(*evil)->lifes--;
								if ((*evil)->lifes <= 0)
								{
									if (sound)mciSendString(L"play .\\res\\snd\\evilkilled.wav", NULL, NULL, NULL);
									score += 10;
									
									D2D1_RECT_F temp{ (*evil)->rect };

									for (int i = 0; i < vGoods.size(); ++i)
									{
										if (vGoods[i]->lifes <= 0)continue;
										
										if (zombie::Intersect(vGoods[i]->rect, temp))
											vGoods[i]->path_info(vGoods[i]->center.x, sky);
									}
									
									(*evil)->Release();
									vEvils.erase(evil);
									killed = true;

									break;
								}
							}
						}
						else if (zombie_attack > 0)
						{
							if (sound)mciSendString(L"play .\\res\\snd\\herokilled.wav", NULL, NULL, NULL);
							(*good)->lifes -= zombie_attack;
						}
					}
				}

				if (killed)break;
			}
		}
	
		if (!vShots.empty())
		{
			for (contlib::BAG<zombie::SHOT*>::iterator shot = vShots.begin(); shot < vShots.end(); ++shot)
			{
				if (!(*shot)->move(level))
				{
					(*shot)->Release();
					vShots.erase(shot);
					break;
				}
			}
		}

		if (!vShots.empty() && !vEvils.empty())
		{
			bool killed = false;

			for (contlib::BAG<zombie::CREATURE*>::iterator evil = vEvils.begin(); evil < vEvils.end(); ++evil)
			{
				for (contlib::BAG<zombie::SHOT*>::iterator shot = vShots.begin(); shot < vShots.end(); ++shot)
				{
					if (zombie::Intersect((*evil)->rect, (*shot)->rect))
					{
						(*evil)->lifes -= 20;

						(*shot)->Release();
						vShots.erase(shot);

						if ((*evil)->lifes <= 0)
						{
							if (sound)mciSendString(L"play .\\res\\snd\\evilkilled.wav", NULL, NULL, NULL);
							score += 10;
							(*evil)->Release();
							vEvils.erase(evil);
							killed = true;
							break;
						}

						break;
					}
				}

				if (killed)break;
			}
		}
	
		if (vPortals.size() < 2 && RandIt(0, 150) == 33)
		{
			zombie::PORTAL* temp{ zombie::PORTAL::create(static_cast<portals>(RandIt(0, 1)), RandIt(150.0f, 400.0f), -50.0f) };
			bool ok = true;

			if (!vPortals.empty())
			{
				for (int i = 0; i < vPortals.size(); ++i)
				{
					if (zombie::Intersect(vPortals[i]->rect, temp->rect))
					{
						ok = false;
						break;
					}
				}
			}
			
			if (ok)vPortals.push_back(temp);
		}
		
		if (!vPortals.empty())
		{
			for (contlib::BAG<zombie::PORTAL*>::iterator portal = vPortals.begin(); portal < vPortals.end(); ++portal)
			{
				if (!(*portal)->move(level))
				{
					(*portal)->Release();
					vPortals.erase(portal);
					break;
				}
			}
		}

		if (!vPortals.empty() && !vGoods.empty())
		{
			bool killed = false;

			for (contlib::BAG<zombie::PORTAL*>::iterator portal = vPortals.begin(); portal < vPortals.end(); ++portal)
			{
				for (int i = 0; i < vGoods.size(); ++i)
				{
					if (zombie::Intersect(vGoods[i]->rect, (*portal)->rect))
					{
						float tx = (*portal)->start.x;
						float ty = (*portal)->start.y;

						if (ty <= scr_height / 2.0f)ty = (*portal)->end.y;

						if ((*portal)->get_type() == portals::warrior_portal)
						{
							for (int i = 0; i < 10; ++i)
							{
								vGoods.push_back(zombie::CREATURE::create(creature::warrior, tx, ty));

								if (tx >= 350.0f)tx -= (30.0f + RandIt(10.0f, 20.0f));
								else tx += (30.0f + RandIt(10.0f, 20.0f));

								if (ty <= scr_height / 2.0f)vGoods.back()->path_info(vGoods.back()->center.x, sky);
								else vGoods.back()->path_info(vGoods.back()->center.x, ground);
							}
						}
						else
						{
							for (int i = 0; i < 10; ++i)
							{
								vGoods.push_back(zombie::CREATURE::create(creature::mage, tx, ty));

								if (tx >= 350.0f)tx -= (30.0f + RandIt(10.0f, 20.0f));
								else tx += (30.0f + RandIt(10.0f, 20.0f));

								if (ty <= scr_height / 2.0f)vGoods.back()->path_info(vGoods.back()->center.x, sky);
								else vGoods.back()->path_info(vGoods.back()->center.x, ground);
							}
						}

						(*portal)->Release();
						vPortals.erase(portal);
						killed = true;
						break;
					}
				}

				if (killed)break;
			}
		}

		if (castle_active && !vGoods.empty())
		{
			for (int i = 0; i < vGoods.size(); ++i)
			{
				if (zombie::Intersect(CastleRect, vGoods[i]->rect))
				{
					int damage = vGoods[i]->attack();

					if (damage > 0)
					{
						castle_lifes--;
						if (castle_lifes <= 0)
						{
							if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
							
							castle_demolished = true;
							
							ExplosionRect.left = CastleRect.left + 25.0f;
							ExplosionRect.right = ExplosionRect.left + 100.0f;
							ExplosionRect.top = CastleRect.top + 10.0f;
							ExplosionRect.bottom = ExplosionRect.top + 114.0f;
							
							break;
						}
					}
				}
			}
		}

		if (castle_active && !vShots.empty())
		{
			for (contlib::BAG<zombie::SHOT*>::iterator shot = vShots.begin(); shot < vShots.end(); ++shot)
			{
				if (zombie::Intersect(CastleRect, (*shot)->rect))
				{
					castle_lifes -= 20;

					(*shot)->Release();
					vShots.erase(shot);
					
					if (castle_lifes <= 0)
					{
						castle_demolished = true;

						if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);

						ExplosionRect.left = CastleRect.left + 25.0f;
						ExplosionRect.right = ExplosionRect.left + 100.0f;
						ExplosionRect.top = CastleRect.top + 10.0f;
						ExplosionRect.bottom = ExplosionRect.top + 114.0f;
						
						break;
					}

					break;
				}
			}
		}
		
		///////////////////////////////////////////////////////
	
	
	// DRAW THINGS ***************************************************

		Draw->BeginDraw();

		Draw->DrawBitmap(bmpOcean[Ocean.get_frame()], GAME_SCREEN);

		if (!vSands.empty())
			for (int i = 0; i < vSands.size(); ++i)Draw->DrawBitmap(bmpSand, vSands[i]->rect);

		if (!vPortals.empty())
		{
			for (int i = 0; i < vPortals.size(); ++i)
			{
				switch (vPortals[i]->get_type())
				{
				case portals::warrior_portal:
					Draw->DrawBitmap(bmpEnergyField1, vPortals[i]->rect);
					break;

				case portals::mage_portal:
					Draw->DrawBitmap(bmpEnergyField2, vPortals[i]->rect);
					break;
				}
			}
		}

		if (nrmText && inactBrush && statBrush && txtBrush && hgltBrush && b1BckgBrush && b2BckgBrush && b3BckgBrush)
		{
			Draw->FillRectangle(MENU_BAR, statBrush);
			Draw->FillRectangle(STATUS_BAR, statBrush);

			Draw->FillRoundedRectangle(D2D1::RoundedRect(b1Rect, 25.0f, 15.0f), b1BckgBrush);
			Draw->FillRoundedRectangle(D2D1::RoundedRect(b2Rect, 25.0f, 15.0f), b2BckgBrush);
			Draw->FillRoundedRectangle(D2D1::RoundedRect(b3Rect, 25.0f, 15.0f), b3BckgBrush);

			if (name_set)Draw->DrawTextW(L"ИМЕ НА ВЛАДЕТЕЛ", 16, nrmText, b1TxtRect, inactBrush);
			else
			{
				if (!b1Hglt)Draw->DrawTextW(L"ИМЕ НА ВЛАДЕТЕЛ", 16, nrmText, b1TxtRect, txtBrush);
				else Draw->DrawTextW(L"ИМЕ НА ВЛАДЕТЕЛ", 16, nrmText, b1TxtRect, hgltBrush);
			}
			if (!b2Hglt)Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmText, b2TxtRect, txtBrush);
			else Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmText, b2TxtRect, hgltBrush);
			if (!b3Hglt)Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmText, b3TxtRect, txtBrush);
			else Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmText, b3TxtRect, hgltBrush);
		}

	//////////////////////////////////////////////////////////////////
	
		if (castle_active)
		{
			if (!castle_demolished)Draw->DrawBitmap(bmpCastle, CastleRect);
			else
			{
				int frame = ExplosionFrame();
				
				if (frame >= 0)Draw->DrawBitmap(bmpExplosion[frame], Resizer(bmpExplosion[frame],
					ExplosionRect.left, ExplosionRect.top));
				else
				{
					Draw->EndDraw();
					LevelUp();
				}
			}
		}
		
		if (!vGoods.empty())
		{
			for (int i = 0; i < vGoods.size(); ++i)
			{
				if (vGoods[i]->lifes > 0)
				{
					int aframe = vGoods[i]->get_frame();

					switch (vGoods[i]->get_type())
					{
					case creature::warrior:
						Draw->DrawBitmap(bmpVitWarrior[aframe], Resizer(bmpVitWarrior[aframe],
							vGoods[i]->start.x, vGoods[i]->start.y));
						break;

					case creature::mage:
						Draw->DrawBitmap(bmpVitMage[aframe], Resizer(bmpVitMage[aframe],
							vGoods[i]->start.x, vGoods[i]->start.y));
					}
				}
				else
				{
					int aframe = vGoods[i]->get_frame();

					if (aframe < 0)
					{
						vGoods.erase(i);
						break;
					}
					else
					{
						switch (vGoods[i]->get_type())
						{
						case creature::warrior:
							Draw->DrawBitmap(bmpFallWarrior[aframe], Resizer(bmpFallWarrior[aframe],
								vGoods[i]->start.x, vGoods[i]->start.y));
							break;

						case creature::mage:
							Draw->DrawBitmap(bmpFallMage[aframe], Resizer(bmpFallMage[aframe],
								vGoods[i]->start.x, vGoods[i]->start.y));
						}
					}
				}
			}
		}

		if (!vEvils.empty())
		{
			for (int i = 0; i < vEvils.size(); ++i)
			{
				int aframe = vEvils[i]->get_frame();

				switch (vEvils[i]->get_type())
				{
				case creature::zombie1:
					Draw->DrawBitmap(bmpZombie1[aframe], Resizer(bmpZombie1[aframe], vEvils[i]->start.x, vEvils[i]->start.y));
					break;

				case creature::zombie2:
					Draw->DrawBitmap(bmpZombie2[aframe], Resizer(bmpZombie2[aframe], vEvils[i]->start.x, vEvils[i]->start.y));
					break;

				case creature::zombie3:
					Draw->DrawBitmap(bmpZombie3[aframe], Resizer(bmpZombie3[aframe], vEvils[i]->start.x, vEvils[i]->start.y));
					break;
				}
			}
		}

		if (!vShots.empty())
		{
			for (int i = 0; i < vShots.size(); ++i)
			{
				int frame{ vShots[i]->get_frame() };

				Draw->DrawBitmap(bmpShot[frame], Resizer(bmpShot[frame], vShots[i]->start.x, vShots[i]->start.y));
			}
		}

		if (nrmText && midText && hgltBrush && statBrush)
		{
			wchar_t stat_txt[200]{ L"владетел: " };
			wchar_t add[5]{ L"\0" };
			int size = 0;

			wcscat_s(stat_txt, current_player);

			wcscat_s(stat_txt, L", войници: ");
			wsprintf(add, L"%d", (int)(vGoods.size()));
			wcscat_s(stat_txt, add);

			wcscat_s(stat_txt, L", резултат: ");
			wsprintf(add, L"%d", score);
			wcscat_s(stat_txt, add);

			wcscat_s(stat_txt, L", ниво: ");
			wsprintf(add, L"%d", (int)(level));
			wcscat_s(stat_txt, add);

			for (int i = 0; i < 200; ++i)
			{
				if (stat_txt[i] != '\0')++size;
				else break;
			}

			Draw->DrawTextW(stat_txt, size, midText, D2D1::RectF(10.0f, ground + 2.0f, scr_width, scr_height), hgltBrush);
	
			swprintf_s(stat_txt, 200, L"остават: %.2f метра", distance * 10.0f);

			size = 0;
			for (int i = 0; i < 200; ++i)
			{
				if (stat_txt[i] != '\0')++size;
				else break;
			}

			Draw->DrawTextW(stat_txt, size, nrmText, D2D1::RectF(400.0f, sky + 10.0f, scr_width, scr_height), statBrush);
		}

	// END DRAW ************************************

		Draw->EndDraw();
	
	}

	FreeResources();
	std::remove(tmp_file);

    return (int) bMsg.wParam;
}