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

D2D1_RECT_F b1Rect{ 50.0f, 10.0f, scr_width / 3.0f - 50.0f, 40.0f };
D2D1_RECT_F b2Rect{ scr_width / 3.0f + 20.0f, 10.0f, scr_width * 2.0f / 3.0f - 50.0f , 40.0f };
D2D1_RECT_F b3Rect{ scr_width * 2.0f / 3.0f + 20.0f, 10.0f, scr_width - 50.0f , 40.0f };

D2D1_RECT_F b1TxtRect{ 70.0f, 15.0f, scr_width / 3.0f - 50.0f, 40.0f };
D2D1_RECT_F b2TxtRect{ scr_width / 3.0f + 35.0f, 15.0f, scr_width * 2.0f / 3.0f - 50.0f , 40.0f };
D2D1_RECT_F b3TxtRect{ scr_width * 2.0f / 3.0f + 30.0f, 15.0f, scr_width - 50.0f , 40.0f };

bool pause = false;
bool sound = true;
bool in_client = true;
bool show_help = false;
bool b1Hglt = false;
bool b2Hglt = false;
bool b3Hglt = false;

bool name_set = false;
bool castle_demolished = false;
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

contlib::BAG<zombie::BACKGROUND*>vSands;
zombie::BACKGROUND Ocean(background::ocean);
zombie::BACKGROUND Intro(background::intro);








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

	distance = 300;
	castle_lifes = 250;

	castle_demolished = false;
	level_skipped = false;
}
void LevelUp()
{
	if (!level_skipped)score += 10 * level;

	++level;

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
		if (pause)break;
		distance--;
		if (distance <= 0)LevelUp();
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










	default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
	}

	return (LRESULT)(FALSE);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	bIns = hInstance;
	if (!bIns)
	{
		LogErr(L"Error in Windows hInstance !");
		ErrExit(eClass);
	}









	FreeResources();
	std::remove(tmp_file);

    return (int) bMsg.wParam;
}