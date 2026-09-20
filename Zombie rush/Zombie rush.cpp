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

int castle_lifes{ 100 };

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
ID2D1Bitmap* bmpRock1{ nullptr };
ID2D1Bitmap* bmpRock2{ nullptr };
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












int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    






    return (int) msg.wParam;
}