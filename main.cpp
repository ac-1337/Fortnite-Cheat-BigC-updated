/*
	*Gloomy.cc
 	*https://github.com/Chase1803
	
	Copyright (c) 2022 Chase1803
	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:
	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
 */

#include <iostream>
#include "GL/glew.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <vector>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <intrin.h>
#include "driverloader/singed.hpp"
#include "driver.h"
#include "Controller.h"
#include <d3d9.h>
#include "d3dx9.h"
#include <thread>
#include <string>
#include <stdio.h>
#include <fstream>
#include "font_awesome.h"
#include "font_awesome.cpp"
#include "font.h"
#include "utils.hpp"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

extern void aimbot(float x, float y);
GLFWwindow* g_window;

static void glfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, XorStr("OverEW: %d: %s\n").c_str(), error, description);
}

void setupWindow() 
{
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit()) {
		std::cout << XorStr("Over Init Failed!\n").c_str();
		return;
	}

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	if (!monitor) {
		fprintf(stderr, XorStr("Failed to get monitor!\n").c_str());
		return;
	}

	g_width = glfwGetVideoMode(monitor)->width;
	g_height = glfwGetVideoMode(monitor)->height;

	glfwWindowHint(GLFW_FLOATING, true);
	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_MAXIMIZED, false);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);

	g_window = glfwCreateWindow(g_width, g_height, XorStr("Nvidia").c_str(), NULL, NULL);

	if (g_window == NULL) {
		std::cout << XorStr("Over: 0x2!\n").c_str();
		return;
	}

	glfwSetWindowAttrib(g_window, GLFW_DECORATED, false);
	glfwSetWindowAttrib(g_window, GLFW_MOUSE_PASSTHROUGH, true);

	glfwSetWindowMonitor(g_window, NULL, 0, 0, g_width, g_height + 1, 0);

	glfwMakeContextCurrent(g_window);
	glfwSwapInterval(1); 

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, XorStr("Over: 0x1!\n").c_str());
		return;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 }; //ICON_MIN_FA , ICON_MAX_FA
	ImFontConfig icons_config;

	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.OversampleH = 2.5;
	icons_config.OversampleV = 2.5;

	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	io.WantCaptureKeyboard;
	io.WantCaptureMouse;
	io.FontAllowUserScaling;

	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 13);
	io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 13.0f, &icons_config, icons_ranges);

	menucolors();

	ImGui_ImplGlfw_InitForOpenGL(g_window, true);
	ImGui_ImplOpenGL3_Init(XorStr("#version 130").c_str());
}

void cleanupWindow() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(g_window);
	glfwTerminate();
}
using namespace std;
int getClosest(int val1, int val2,
	int target)
{
	if (target - val1 >= val2 - target)
		return val2;
	else
		return val1;
}
int findClosest(int arr[], int n, int target)
{
	if (target <= arr[0])
		return arr[0];
	if (target >= arr[n - 1])
		return arr[n - 1];
	int i = 0, j = n, mid = 0;
	while (i < j) {
		mid = (i + j) / 2;
		if (arr[mid] == target)
			return arr[mid];
		if (target < arr[mid]) {
			if (mid > 0 && target > arr[mid - 1])
				return getClosest(arr[mid - 1],
					arr[mid], target);
			j = mid;
		}
		else {
			if (mid < n - 1 && target < arr[mid + 1])
				return getClosest(arr[mid],
					arr[mid + 1], target);
			i = mid + 1;
		}
	}
	return arr[mid];
}

BOOL CALLBACK retreiveFortniteWindow(HWND hwnd, LPARAM lparam) {
	DWORD process_id;
	GetWindowThreadProcessId(hwnd, &process_id);
	if (process_id == g_pid) {
		fortnite_wnd = hwnd;
	}
	return TRUE;
}

bool isFortniteFocused = false;

void activateFortniteWindow() {
	SetForegroundWindow(fortnite_wnd);
}

void handleKeyPresses() 
{
	if (GetAsyncKeyState(VK_F1) & 1) {
		g_overlay_visible = !g_overlay_visible;
		glfwSetWindowAttrib(g_window, GLFW_MOUSE_PASSTHROUGH, !g_overlay_visible);
		if (g_overlay_visible) {
			HWND overlay_window = glfwGetWin32Window(g_window);
			SetForegroundWindow(overlay_window);
		}
		else {
			activateFortniteWindow();
		}
	}
}

void drawlootloop()
{
	if (Globals::LocalPawn)
	{
		std::vector<LootEntity> tmpList;
		uintptr_t ItemLevels = read<uintptr_t>(g_pid, GWorld + 0x160);

		for (int i = 0; i < read<DWORD>(g_pid, GWorld + (0x160 + sizeof(PVOID))); ++i) {

			uintptr_t ItemLevel = read<uintptr_t>(g_pid, ItemLevels + (i * sizeof(uintptr_t)));

			for (int i = 0; i < read<DWORD>(g_pid, ItemLevel + (0x98 + sizeof(PVOID))); ++i) {
				uintptr_t ItemsPawns = read<uintptr_t>(g_pid, ItemLevel + 0x98);
				uintptr_t CurrentItemPawn = read<uintptr_t>(g_pid, ItemsPawns + (i * sizeof(uintptr_t)));
				uintptr_t ItemRootComponent = read<uintptr_t>(g_pid, CurrentItemPawn + 0x188);
				Vector3 ItemPosition = read<Vector3>(g_pid, ItemRootComponent + 0x128);
				float ItemDist = Globals::LocalPlayerRelativeLocation.Distance(ItemPosition) / 100.f;

				if (ItemDist < bLootRendering) {

					int currentitemid = read<int>(g_pid, CurrentItemPawn + 0x18);
					auto CurrentItemPawnName = GetNameFromFName(currentitemid);

					if ((g_loot && strstr(CurrentItemPawnName.c_str(), (XorStr("FortPickupAthena")).c_str())) || strstr(CurrentItemPawnName.c_str(), (XorStr("Tiered_Chest").c_str())) || 
						(g_vehicles && strstr(CurrentItemPawnName.c_str(), XorStr("Vehicl").c_str()) || strstr(CurrentItemPawnName.c_str(), XorStr("Valet_Taxi").c_str()) || 
						strstr(CurrentItemPawnName.c_str(), XorStr("Valet_BigRig").c_str()) || strstr(CurrentItemPawnName.c_str(), XorStr("Valet_BasicTr").c_str()) || 
						strstr(CurrentItemPawnName.c_str(), XorStr("Valet_SportsC").c_str()) || strstr(CurrentItemPawnName.c_str(), XorStr("Valet_BasicC").c_str()))) 
					{
						LootEntity fnlEntity{ };
						fnlEntity.CurrentActor = CurrentItemPawn;
						fnlEntity.name = CurrentItemPawnName;
						tmpList.push_back(fnlEntity);

					}
				}

			}
		}
		LootentityList.clear();
		LootentityList = tmpList;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}



void CacheNew()
{
	while (true)
	{
		std::vector<Playertest> tmpList;

		GWorld = read<DWORD_PTR>(g_pid, g_base_address + 0xC640158);
		uintptr_t PersistentLevel = read<uintptr_t>(g_pid, GWorld + 0x30);
		uintptr_t GameInstance = read<uintptr_t>(g_pid, GWorld + 0x1A8);
		uintptr_t LocalPlayers = read<uintptr_t>(g_pid, GameInstance + 0x38);
		Globals::LocalPlayer = read<uintptr_t>(g_pid, LocalPlayers);
		
		LocalPlayerController = read<uintptr_t>(g_pid, Globals::LocalPlayer + 0x30);
		PlayerCameraManager = read<uint64_t>(g_pid, LocalPlayerController + 0x328);
		Globals::LocalPawn = read<uintptr_t>(g_pid, LocalPlayerController + 0x310);
		Globals::LocalPawnRootComponent = read<uintptr_t>(g_pid, Globals::LocalPawn + 0x188);
		uint64_t localplayerstate = read<uint64_t>(g_pid, Globals::LocalPawn + 0x290);
		LocalTeam = read<int>(g_pid, localplayerstate + 0x1010);

		InLobby = false;
		if (!Globals::LocalPawn) InLobby = true;

		auto ActorCount = read<DWORD>(g_pid, PersistentLevel + 0xA0);
		auto AActors2 = read<uintptr_t>(g_pid, PersistentLevel + 0x98);
		
		for (int i = 0; i < ActorCount; ++i) {
			uintptr_t CurrentItemPawn = read<uintptr_t>(g_pid, AActors2 + (i * sizeof(uintptr_t)));
			
			int CurrentItemId = read<int>(g_pid, CurrentItemPawn + 0x18);
			auto CurrentItemPawnName = GetNameFromFName(CurrentItemId);

			if (strstr(CurrentItemPawnName.c_str(), "PlayerPawn_Athena_C") || strstr(CurrentItemPawnName.c_str(), "PlayerPawn"))
			{
				Playertest Actor{ };

				Actor.Acotr = CurrentItemPawn;
				Actor.Acotrmesh = read<uint64_t>(g_pid, CurrentItemPawn + 0x2F0);
				Actor.name = CurrentItemPawnName;
				Actor.rootcomp = Globals::LocalPawnRootComponent;

				tmpList.push_back(Actor);
			}
		}

		drawlootloop();
		PLIST.clear();
		PLIST = tmpList;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

bool isVisible(DWORD_PTR mesh)
{
	if (!mesh)
		return false;
	float fLastSubmitTime = read<float>(g_pid, mesh + 0x330);
	float fLastRenderTimeOnScreen = read<float>(g_pid, mesh + 0x338);

	const float fVisionTick = 0.06f;
	bool bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;
	return bVisible;
}
bool bulletTP = false;

Vector3 HeadPosition;



bool IsVec3Valid(Vector3 vec3)
{
	return !(vec3.x == 0 && vec3.y == 0 && vec3.z == 0);
}


void WriteAngles(Vector3 Location)
{
	//Depreciated
}

bool actorLoop() 
{
	bool bValidEnemyInArea = false;
	float ClosestActorDistance = FLT_MAX;
	Vector3 ClosestActorMouseAimbotPosition = Vector3(0.0f, 0.0f, 0.0f);
	float distance, distaim;

	try
	{
		for (LootEntity entity : LootentityList) {

			if (Globals::LocalPawn)
			{
				uintptr_t ItemRootComponent = read<uintptr_t>(g_pid, entity.CurrentActor + 0x188);
				Vector3 ItemPosition = read<Vector3>(g_pid, ItemRootComponent + 0x128);
				float ItemDist = Globals::LocalPlayerRelativeLocation.Distance(ItemPosition) / 100.f;
				std::string null = ("");

				auto IsSearched = read<BYTE>(g_pid, (uintptr_t)entity.CurrentActor + 0xf51);
				if (IsSearched >> 7 & 1) continue;

				if (strstr(entity.name.c_str(), ("Tiered_Chest")) && g_chests)
				{
					if (ItemDist < bLootRendering) {
						Vector3 ChestPosition;
						ChestPosition = g_functions::ConvertWorld2Screen(ItemPosition);
						std::string Text = null + ("Chest [") + std::to_string((int)ItemDist) + ("m]");
						DrawString(14, ChestPosition.x, ChestPosition.y, &Col.yellow, true, true, Text.c_str());
					}
				}
				else if ((g_vehicles && (strstr(entity.name.c_str(), XorStr("Vehicl").c_str()) || strstr(entity.name.c_str(), XorStr("Valet_Taxi").c_str()) || strstr(entity.name.c_str(), XorStr("Valet_BigRig").c_str()) || strstr(entity.name.c_str(), XorStr("Valet_BasicTr").c_str()) || strstr(entity.name.c_str(), XorStr("Valet_SportsC").c_str()) || strstr(entity.name.c_str(), XorStr("Valet_BasicC").c_str()))))
				{
					if (ItemDist < bLootRendering) {
						Vector3 VehiclePosition = g_functions::ConvertWorld2Screen(ItemPosition);
						std::string Text = null + ("Vehicle [") + std::to_string((int)ItemDist) + ("m]");
						DrawString(14, VehiclePosition.x, VehiclePosition.y, &Col.red, true, true, Text.c_str());
					}
				}
				else if (strstr(entity.name.c_str(), ("AthenaSupplyDrop_C")) && g_loot)
				{
					if (ItemDist < bLootRendering) {
						Vector3 ChestPosition;
						ChestPosition = g_functions::ConvertWorld2Screen(ItemPosition);

						std::string Text = null + ("Supply Drop [") + std::to_string((int)ItemDist) + ("m]");
						DrawString(14, ChestPosition.x, ChestPosition.y, &Col.blue, true, true, Text.c_str());

					}
				}
				else if (strstr(entity.name.c_str(), ("Tiered_Ammo")) && g_ammo)
				{
					if (ItemDist < bLootRendering) {
						Vector3 ChestPosition;
						ChestPosition = g_functions::ConvertWorld2Screen(ItemPosition);
						std::string Text = null + ("Ammo Box [") + std::to_string((int)ItemDist) + ("m]");
						DrawString(14, ChestPosition.x, ChestPosition.y, &Col.white, true, true, Text.c_str());

					}
				}

				else if (strstr(entity.name.c_str(), ("B_Prj_Bullet_Sniper")) && bulletTP)
				{
					auto RelativeLocation = read<uint64_t>(g_pid, entity.CurrentActor + 0x2F0);
					write<Vector3>(g_pid, RelativeLocation + 0x128, Vector3(HeadPosition.x, HeadPosition.y, HeadPosition.z + 10)); //sets the relative location of the projectile to the enemy's head
				}

				else if (g_loot && strstr(entity.name.c_str(), ("FortPickupAthena")))
				{
					if (ItemDist < bLootRendering) {

						auto definition = read<uint64_t>(g_pid, entity.CurrentActor + 0x2f8 + 0x18);
						BYTE tier = read<BYTE>(g_pid, definition + 0x74);

						RGBA Color, RGBAColor;
						Vector3 ChestPosition = g_functions::ConvertWorld2Screen(ItemPosition);

						if (g_loot)
						{
							auto DisplayName = read<uint64_t>(g_pid, definition + 0x90);
							auto WeaponLength = read<uint32_t>(g_pid, DisplayName + 0x38);
							wchar_t* WeaponName = new wchar_t[uint64_t(WeaponLength) + 1];

							Drive.ReadPtr(g_pid, (ULONG64)read<PVOID>(g_pid, DisplayName + 0x30), WeaponName, WeaponLength * sizeof(wchar_t));

							std::string Text = wchar_to_char(WeaponName);
							std::string wtf2 = Text + " [" + std::to_string((int)ItemDist) + ("m]");
							if (tier == 2)
							{
								Color = Col.green;
							}
							else if ((tier == 3))
							{
								Color = Col.blue;
							}
							else if ((tier == 4))
							{
								Color = Col.purple;
							}
							else if ((tier == 5))
							{
								Color = Col.yellow;
							}
							else if ((tier == 6))
							{
								Color = Col.yellow;
							}
							else if ((tier == 0) || (tier == 1))
							{
								Color = Col.white;
							}

							DrawString(14, ChestPosition.x, ChestPosition.y, &Color, true, true, wtf2.c_str());
						}
					}
				}
			}
		}

		int NewPlayerLocationX;
		int NewPlayerLocationY;
		Vector3 Headposw2s;
		Vector3 HeadAim;

		for (Playertest p : PLIST)
		{
			auto identify = g_functions::f_getbonewithIndex(0, 0);
			g_functions::ConvertWorld2Screen(identify);

			uintptr_t RootComponent = read<uintptr_t>(g_pid, p.Acotr + 0x188);
			Globals::LocalPlayerRelativeLocation = read<Vector3>(g_pid, p.rootcomp + 0x128);

			if (Globals::LocalPawn)
			{
				const auto SetupCameraRotationAndFov = [](uintptr_t LocalPlayer, uintptr_t RootComponent, Vector3& CameraRotation, float& FOVAngle)
				{
					auto CameraRotChain_tmp0 = read<uint64_t>(g_pid, LocalPlayer + 0xC8);
					auto CameraRotChain_tmp1 = read<uint64_t>(g_pid, CameraRotChain_tmp0 + 0x8);

					camera::m_CameraRotation.x = read<double>(g_pid, CameraRotChain_tmp1 + 0xAE0);

					double tester = asin(camera::m_CameraRotation.x);
					double degreees = tester * (180.0 / M_PI);

					camera::m_CameraRotation.y = read<double>(g_pid, RootComponent + 0x148);


					camera::m_CameraRotation.x = degreees;

					//set fov angle
					if (g_fovchanger)
					{
						FOVAngle = FOVChangerValue;
					}
					else
					{
						FOVAngle = 80.0f / (read<double>(g_pid, CameraRotChain_tmp1 + 0x610) / 1.19f);
					}

				};
				SetupCameraRotationAndFov(Globals::LocalPlayer, Globals::LocalPawnRootComponent, camera::m_CameraRotation, camera::m_FovAngle);
			}
			else {
				// PlayerCameraManager -> LastFrameCameraCachePrivate -> POV -> Rotation && FOV
				camera::m_CameraRotation = read<Vector3>(g_pid, PlayerCameraManager + 0x28d0 + 0x10 + 0x18);
				camera::m_CameraRotation.z = 0;

				if (g_fovchanger)
				{
					camera::m_FovAngle = FOVChangerValue;
				}
				else
				{
					camera::m_FovAngle = read<float>(g_pid, PlayerCameraManager + 0x28d0 + 0x10 + 0x30);
				}
			}

			// get camera location
			const auto SetupCameraLocation = [](uintptr_t LocalPlayer, Vector3& CameraLocation)
			{
				auto CameraLocChain_tmp0 = read<uint64_t>(g_pid, LocalPlayer + 0x70);
				auto CameraLocChain_tmp1 = read<uint64_t>(g_pid, CameraLocChain_tmp0 + 0x98);
				auto CameraLocChain_tmp2 = read<uint64_t>(g_pid, CameraLocChain_tmp1 + 0x180);

				CameraLocation = read<Vector3>(g_pid, CameraLocChain_tmp2 + 0x20);

			};
			SetupCameraLocation(Globals::LocalPlayer, camera::m_CameraLocation);

			uint64_t playerstate = read<uint64_t>(g_pid, p.Acotr + 0x290);

			int TeamIndex = read<int>(g_pid, playerstate + 0x1010);

			if (g_fovchanger)
			{
				write(g_pid, PlayerCameraManager + 0x288, FOVChangerValue);
			}

			Vector3 vHeadBone = g_functions::f_getbonewithIndex(p.Acotrmesh, 98);
			Vector3 vRootBone = g_functions::f_getbonewithIndex(p.Acotrmesh, 0);

			Vector3 vHeadBoneOut = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x, vHeadBone.y, vHeadBone.z + 15));
			Vector3 vRootBoneOut = g_functions::ConvertWorld2Screen(vRootBone);

			Vector3 w2shead = g_functions::ConvertWorld2Screen(vHeadBone);

			Vector3 RootPos = g_functions::f_getbonewithIndex(p.Acotrmesh, 68);
			Vector3 RootPosOut = g_functions::ConvertWorld2Screen(RootPos);

			float BoxHeight = abs(vHeadBoneOut.y - vRootBoneOut.y);
			float BoxWidth = BoxHeight * 0.50;

			Vector3 RelativeInternalLocation = read<Vector3>(g_pid, RootComponent + 0x128);
			if (!RelativeInternalLocation.x && !RelativeInternalLocation.y) {
				continue;
			}

			Vector3 RelativeScreenLocation = g_functions::ConvertWorld2Screen(RelativeInternalLocation);
			if (!RelativeScreenLocation.x && !RelativeScreenLocation.y) {
				continue;
			}

			distance = Globals::LocalPlayerRelativeLocation.Distance(RelativeInternalLocation) / 100.f;

			if (g_name_esp)
			{
				uint64_t APlayerState = read<uint64_t>(g_pid, 0x0);//APlayerState->PlayerNamePrivate*FString
				auto nameptr = read<uintptr_t>(g_pid, APlayerState + 0x370);

				uint64_t StringData = read<uint64_t>(g_pid, nameptr);
				uint32_t StringLength = read<uint32_t>(g_pid, nameptr + 0x8);
				wchar_t* OutString = new wchar_t[StringLength + 1];
				Drive.ReadPtr(g_pid, StringData, OutString, StringLength * sizeof(wchar_t));

				std::string Text = wchar_to_char(OutString);

				DrawString(13, vHeadBoneOut.x, vHeadBoneOut.y - 50, &Col.white, true, true, Text.c_str());
			}

			if (g_platform_esp)
			{
				uint64_t AFortPlayerState = read<uint64_t>(g_pid, 0x0);//AFortPlayerState->Platform*FString
				auto nameptr = read<uintptr_t>(g_pid, AFortPlayerState + 0x420);

				uint64_t StringData = read<uint64_t>(g_pid, nameptr);
				uint32_t StringLength = read<uint32_t>(g_pid, nameptr + 0x8);
				wchar_t* OutString = new wchar_t[StringLength + 1];
				Drive.ReadPtr(g_pid, StringData, OutString, StringLength * sizeof(wchar_t));

				std::string Text = wchar_to_char(OutString);

				if (strstr(Text.c_str(), ("WIN")))
				{
					DrawString(13, vHeadBoneOut.x, vHeadBoneOut.y - 20, &Col.white, true, true, "PC");

				}
				else if (strstr(Text.c_str(), ("XBL")) || strstr(Text.c_str(), ("XSX")))
				{
					DrawString(13, vHeadBoneOut.x, vHeadBoneOut.y - 20, &Col.green, true, true, "Xbox");

				}
				else if (strstr(Text.c_str(), ("PSN")) || strstr(Text.c_str(), ("PS5")))
				{
					DrawString(13, vHeadBoneOut.x, vHeadBoneOut.y - 20, &Col.lightblue, true, true, "PSN");

				}
				else if (strstr(Text.c_str(), ("SWT")))
				{
					DrawString(13, vHeadBoneOut.x, vHeadBoneOut.y - 20, &Col.red, true, true, "Nintendo");

				}
				else
				{
					DrawString(13, vHeadBoneOut.x, vHeadBoneOut.y - 20, &Col.white, true, true, Text.c_str());
				}

			}

			//Vector3 neck2 = g_functions::f_getbonewithIndex(p.Acotrmesh, 98);
			//Vector3 vneck2 = g_functions::ConvertWorld2Screen(neck2);
			Vector3 pelvis = g_functions::f_getbonewithIndex(p.Acotrmesh, 2);
			Vector3 vpelvis = g_functions::ConvertWorld2Screen(pelvis);
			//Vector3 bottom1 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 10, vHeadBone.y - 10, vHeadBone.z));
			//Vector3 bottom2 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 10, vHeadBone.y - 10, vHeadBone.z));
			//Vector3 bottom3 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 10, vHeadBone.y + 10, vHeadBone.z));
			//Vector3 bottom4 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 10, vHeadBone.y + 10, vHeadBone.z));

			//Vector3 top1 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 10, vHeadBone.y - 10, vHeadBone.z + 15));
			//Vector3 top2 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 10, vHeadBone.y - 10, vHeadBone.z + 15));
			//Vector3 top3 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 10, vHeadBone.y + 10, vHeadBone.z + 15));
			//Vector3 top4 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 10, vHeadBone.y + 10, vHeadBone.z + 15));

			//ImU32 Cay = ImGui::GetColorU32({ 255, 215, 0, 255 });

			//ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(top1.x, top1.y), Cay, 0.1f);
			//ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(top2.x, top2.y), Cay, 0.1f);
			//ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(top3.x, top3.y), Cay, 0.1f);
			//ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(top4.x, top4.y), Cay, 0.1f);

			//ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(bottom2.x, bottom2.y), Cay, 0.1f);
			//ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(bottom3.x, bottom3.y), Cay, 0.1f);
			//ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(bottom4.x, bottom4.y), Cay, 0.1f);
			//ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(bottom1.x, bottom1.y), Cay, 0.1f);

			//ImGui::GetForegroundDrawList()->AddLine(ImVec2(top1.x, top1.y), ImVec2(top2.x, top2.y), Cay, 0.1f);
			//ImGui::GetForegroundDrawList()->AddLine(ImVec2(top2.x, top2.y), ImVec2(top3.x, top3.y), Cay, 0.1f);
			//ImGui::GetForegroundDrawList()->AddLine(ImVec2(top3.x, top3.y), ImVec2(top4.x, top4.y), Cay, 0.1f);
			//ImGui::GetForegroundDrawList()->AddLine(ImVec2(top4.x, top4.y), ImVec2(top1.x, top1.y), Cay, 0.1f);
			RGBA Skellll;
			Skellll = { 255, 255, 255, 255 };
			//DrawLine(vneck2.x, vneck2.y, vpelvis.x, vpelvis.y, &Skellll, 0.5f);

			if (g_curweaponesp)
			{
				uintptr_t ItemRootComponent = read<uintptr_t>(g_pid, p.Acotr + 0x188);
				Vector3 ItemPosition = read<Vector3>(g_pid, ItemRootComponent + 0x128);
				float ItemDist = Globals::LocalPlayerRelativeLocation.Distance(ItemPosition) / 100.f;

				if (ItemDist < bLootRendering) {

					auto CurrentWeapon = read<uintptr_t>(g_pid, p.Acotr + 0x790);
					auto ItemData = read<DWORD_PTR>(g_pid, CurrentWeapon + 0x3d8);
					BYTE tier = read<BYTE>(g_pid, ItemData + 0x74);

					ImColor Color;
					if (tier == 2)
					{
						Color = IM_COL32(0, 255, 0, 255);
					}
					else if ((tier == 3))
					{
						Color = IM_COL32(0, 0, 255, 255);
					}
					else if ((tier == 4))
					{
						Color = IM_COL32(128, 0, 128, 255);
					}
					else if ((tier == 5))
					{
						Color = IM_COL32(255, 255, 0, 255);
					}
					else if ((tier == 6))
					{
						Color = IM_COL32(255, 255, 0, 255);
					}
					else if ((tier == 0) || (tier == 1))
					{
						Color = IM_COL32(255, 255, 255, 255);
					}

					auto AmmoCount = read<int>(g_pid, CurrentWeapon + 0xb14);
					auto bIsReloadingWeapon = read<bool>(g_pid, CurrentWeapon + 0x311);

					auto DisplayName = read<uint64_t>(g_pid, ItemData + 0x90);
					auto WeaponLength = read<uint32_t>(g_pid, DisplayName + 0x38);
					wchar_t* WeaponName = new wchar_t[uint64_t(WeaponLength) + 1];

					Drive.ReadPtr(g_pid, (ULONG64)read<PVOID>(g_pid, DisplayName + 0x30), WeaponName, WeaponLength * sizeof(wchar_t));
					std::string Text = wchar_to_char(WeaponName);

					if (strstr(p.name.c_str(), "PlayerPawn_Athena_C"))
					{
						std::string Player = XorStr("Player").c_str();
						ImVec2 TextSize = ImGui::CalcTextSize(Player.c_str());
						ImGui::GetOverlayDrawList()->AddText(ImVec2(vHeadBoneOut.x - 30 - TextSize.x / 2, vHeadBoneOut.y - 15 - TextSize.y / 2), ImGui::GetColorU32({ 255, 255, 255, 255 }), Player.c_str());
					}
					else
					{
						std::string Bot = XorStr("Bot/Npc").c_str();
						ImVec2 TextSize = ImGui::CalcTextSize(Bot.c_str());
						ImGui::GetOverlayDrawList()->AddText(ImVec2(vHeadBoneOut.x - 30 - TextSize.x / 2, vHeadBoneOut.y - 15 - TextSize.y / 2), ImGui::GetColorU32({ 255, 255, 255, 255 }), Bot.c_str());

					}

					if (instares) {
						write<float>(g_pid, Globals::LocalPawn + 0x3f60, 0.001);
					}
				
					if (guntest2)//silent
					{


						float AimbotLocationX;
						float AimbotLocationY;
						int AllPlayerLocationX = Headposw2s.x;
						int AllPlayerLocationY = Headposw2s.y;
						int array69_x[] = { AllPlayerLocationX };
						int n_x = sizeof(array69_x) / sizeof(array69_x[0]);
						int array69_y[] = { AllPlayerLocationY };
						int n_y = sizeof(array69_y) / sizeof(array69_y[0]);
						int targetX = Globals::ScreenCenterX;
						int targetY = Globals::ScreenCenterY;
						NewPlayerLocationX = findClosest(array69_x, n_x, targetX);
						NewPlayerLocationY = findClosest(array69_y, n_y, targetY);
						AimbotLocationX = NewPlayerLocationX;
						AimbotLocationY = NewPlayerLocationY;



						void*** Vtable = (void***)CurrentWeapon;
						if (!Vtable[0]);
						auto index = Vtable[0x4B];




						//(Vector3*)((uintptr_t)thingy + 0x2B9 + 0x350) = { NewPlayerLocationX, NewPlayerLocationY, 0 };
						write<Vector3>(g_pid, (uintptr_t)index + 0x2B9 + 0x350, Vector3(NewPlayerLocationX, NewPlayerLocationY, 0));


					}

					if (guntest2) {
						write<bool>(g_pid, Globals::LocalPawn + 0x4805, true);
					}


					if (carfly)
					{
						uintptr_t CurrentVehicle = read<DWORD_PTR>(g_pid, Globals::LocalPawn + 0x2158);

						if (CurrentVehicle) //checks if you are in a vehicle
						{
							write<bool>(g_pid, CurrentVehicle + 0x668, false); //if in vehicle then it disables vehicle gravity
						}
						else
						{
							write<bool>(g_pid, CurrentVehicle + 0x668, true); //if not in vehicle then it enables vehicle gravity
						}
					}
				
					
					if (nigger)
					{
						if (GetAsyncKeyState(VK_SHIFT)) { //Alt Keybind
							write<float>(g_pid, CurrentWeapon + 0x64, 99); //CustomTimeDilation Offset
						}
						else {
							write<float>(g_pid, CurrentWeapon + 0x64, 1); //CustomTimeDilation Offset
						}
					}
		
					
	


					
			

				        if (AmmoCount)
					{
						char buffer[128];
						sprintf_s(buffer, "Ammo: %i", AmmoCount);
						if (buffer != "?") 
						{
						        ImGui::GetOverlayDrawList()->AddText(ImVec2(vpelvis.x, vpelvis.y + 15), IM_COL32(255, 255, 255, 255), buffer);
						}
					 }

						if (bIsReloadingWeapon)
							ImGui::GetOverlayDrawList()->AddText(ImVec2(vpelvis.x - 30, vpelvis.y), IM_COL32(255, 255, 255, 255), "Reloading");
						else
							ImGui::GetOverlayDrawList()->AddText(ImVec2(vpelvis.x - 30, vpelvis.y), Color, Text.c_str());
				}
			}
			
			int Teamcheck;
			
			if (g_chams)
				Teamcheck = TeamIndex = LocalTeam;
			else
				Teamcheck = TeamIndex != LocalTeam; 

			if (Teamcheck || InLobby) {
				isVis = isVisible(p.Acotrmesh);
				if (distance <= bE5pD1st4nce || InLobby) 
				{
					if (g_boxesp)
					{
						if (isVis)
							DrawNormalBox(vRootBoneOut.x - (BoxWidth / 2), vHeadBoneOut.y, BoxWidth, BoxHeight, 1.0f, &ESPColor2, &ESPColor2);
						else
							DrawNormalBox(vRootBoneOut.x - (BoxWidth / 2), vHeadBoneOut.y, BoxWidth, BoxHeight, 1.0f, &ESPColor, &ESPColor);
					}
					else if (g_cornerboxesp)
					{
						if (isVis)
							DrawCorneredBox(vRootBoneOut.x - (BoxWidth / 2), vHeadBoneOut.y, BoxWidth, BoxHeight, IM_COL32(0, 25, 0, 255), 1.5);
						else
							DrawCorneredBox(vRootBoneOut.x - (BoxWidth / 2), vHeadBoneOut.y, BoxWidth, BoxHeight, IM_COL32(255, 0, 0, 255), 1.5);
					}
					if (g_esp_distance) {

						char dist[64];
						sprintf_s(dist, "%.fM", distance);
						ImVec2 TextSize = ImGui::CalcTextSize(dist);
						ImGui::GetOverlayDrawList()->AddText(ImVec2(vRootBoneOut.x - 15 - TextSize.x / 2, vRootBoneOut.y - 15 - TextSize.y / 2), ImGui::GetColorU32({ 255, 255, 255, 255 }), dist);
					}
					if (g_esp_skeleton) {
						Vector3 neck2 = g_functions::f_getbonewithIndex(p.Acotrmesh, 98);
						Vector3 vneck2 = g_functions::ConvertWorld2Screen(neck2);

						Vector3 neck = g_functions::f_getbonewithIndex(p.Acotrmesh, 66);
						Vector3 vneck = g_functions::ConvertWorld2Screen(neck);
						Vector3 rightChest = g_functions::f_getbonewithIndex(p.Acotrmesh, 8);
						Vector3 vrightChest = g_functions::ConvertWorld2Screen(rightChest);
						Vector3 leftChest = g_functions::f_getbonewithIndex(p.Acotrmesh, 37);
						Vector3 vleftChest = g_functions::ConvertWorld2Screen(leftChest);
						Vector3 leftShoulder = g_functions::f_getbonewithIndex(p.Acotrmesh, 38);
						Vector3 vleftShoulder = g_functions::ConvertWorld2Screen(leftShoulder);
						Vector3 rightShoulder = g_functions::f_getbonewithIndex(p.Acotrmesh, 9);
						Vector3 vrightShoulder = g_functions::ConvertWorld2Screen(rightShoulder);
						Vector3 leftElbow = g_functions::f_getbonewithIndex(p.Acotrmesh, 94);
						Vector3 vleftElbow = g_functions::ConvertWorld2Screen(leftElbow);
						Vector3 rightElbow = g_functions::f_getbonewithIndex(p.Acotrmesh, 10);
						Vector3 vrightElbow = g_functions::ConvertWorld2Screen(rightElbow);
						Vector3 leftWrist = g_functions::f_getbonewithIndex(p.Acotrmesh, 62);
						Vector3 vleftWrist = g_functions::ConvertWorld2Screen(leftWrist);
						Vector3 rightWrist = g_functions::f_getbonewithIndex(p.Acotrmesh, 33);
						Vector3 vrightWrist = g_functions::ConvertWorld2Screen(rightWrist);
						Vector3 pelvis = g_functions::f_getbonewithIndex(p.Acotrmesh, 2);
						Vector3 vpelvis = g_functions::ConvertWorld2Screen(pelvis);
						Vector3 leftAss = g_functions::f_getbonewithIndex(p.Acotrmesh, 76);
						Vector3 vleftAss = g_functions::ConvertWorld2Screen(leftAss);
						Vector3 rightAss = g_functions::f_getbonewithIndex(p.Acotrmesh, 69);
						Vector3 vrightAss = g_functions::ConvertWorld2Screen(rightAss);
						Vector3 leftKnee = g_functions::f_getbonewithIndex(p.Acotrmesh, 77);
						Vector3 vleftKnee = g_functions::ConvertWorld2Screen(leftKnee);
						Vector3 rightKnee = g_functions::f_getbonewithIndex(p.Acotrmesh, 70);
						Vector3 vrightKnee = g_functions::ConvertWorld2Screen(rightKnee);
						Vector3 leftAnkle = g_functions::f_getbonewithIndex(p.Acotrmesh, 78);
						Vector3 vleftAnkle = g_functions::ConvertWorld2Screen(leftAnkle);
						Vector3 rightAnkle = g_functions::f_getbonewithIndex(p.Acotrmesh, 71);
						Vector3 vrightAnkle = g_functions::ConvertWorld2Screen(rightAnkle);

						RGBA ESPSkeleton;

						if (isVis)
							ESPSkeleton = { 0, 255, 0, 255 };
						else
							ESPSkeleton = { 255, 0, 0, 255 };

						DrawLine(vleftChest.x, vleftChest.y, vrightChest.x, vrightChest.y, &ESPSkeleton, 0.5f);
						DrawLine(vleftChest.x, vleftChest.y, vleftShoulder.x, vleftShoulder.y, &ESPSkeleton, 0.5f);
						DrawLine(vrightChest.x, vrightChest.y, vrightShoulder.x, vrightShoulder.y, &ESPSkeleton, 0.5f);
						DrawLine(vleftShoulder.x, vleftShoulder.y, vleftElbow.x, vleftElbow.y, &ESPSkeleton, 0.5f);
						DrawLine(vrightShoulder.x, vrightShoulder.y, vrightElbow.x, vrightElbow.y, &ESPSkeleton, 0.5f);
						DrawLine(vleftElbow.x, vleftElbow.y, vleftWrist.x, vleftWrist.y, &ESPSkeleton, 0.5f);
						DrawLine(vrightElbow.x, vrightElbow.y, vrightWrist.x, vrightWrist.y, &ESPSkeleton, 0.5f);
						DrawLine(vneck2.x, vneck2.y, vpelvis.x, vpelvis.y, &ESPSkeleton, 0.5f);
						DrawLine(vpelvis.x, vpelvis.y, vleftAss.x, vleftAss.y, &ESPSkeleton, 0.5f);
						DrawLine(vpelvis.x, vpelvis.y, vrightAss.x, vrightAss.y, &ESPSkeleton, 0.5f);
						DrawLine(vleftAss.x, vleftAss.y, vleftKnee.x, vleftKnee.y, &ESPSkeleton, 0.5f);
						DrawLine(vrightAss.x, vrightAss.y, vrightKnee.x, vrightKnee.y, &ESPSkeleton, 0.5f);
						DrawLine(vleftKnee.x, vleftKnee.y, vleftAnkle.x, vleftAnkle.y, &ESPSkeleton, 0.5f);
						DrawLine(vrightKnee.x, vrightKnee.y, vrightAnkle.x, vrightAnkle.y, &ESPSkeleton, 0.5f);
					}
					if (klixx) {
						uintptr_t CurrentWeapon = read<uintptr_t>(g_pid, Globals::LocalPawn + 0x790);
						if (CurrentWeapon) {
							write<bool>(g_pid, CurrentWeapon + 0xf41, true); //AFortWeapon    bIgnoreTryToFireSlotCooldownRestriction    0xf41    bool
						}
					}


					if (g_3d_box)
					{
						if (vHeadBoneOut.x != 0 || vHeadBoneOut.y != 0 || vHeadBoneOut.z != 0)
						{
							ImU32 ESPSkeleton;
							if (isVis)
								ESPSkeleton = ImGui::GetColorU32({ 255, 215, 0, 255 });
							else
								ESPSkeleton = ImGui::GetColorU32({ 255, 255, 255, 255 });
							
							Vector3 bottom1 = g_functions::ConvertWorld2Screen(Vector3(vRootBone.x + 40, vRootBone.y - 40, vRootBone.z));
							Vector3 bottom2 = g_functions::ConvertWorld2Screen(Vector3(vRootBone.x - 40, vRootBone.y - 40, vRootBone.z));
							Vector3 bottom3 = g_functions::ConvertWorld2Screen(Vector3(vRootBone.x - 40, vRootBone.y + 40, vRootBone.z));
							Vector3 bottom4 = g_functions::ConvertWorld2Screen(Vector3(vRootBone.x + 40, vRootBone.y + 40, vRootBone.z));

							Vector3 top1 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 40, vHeadBone.y - 40, vHeadBone.z + 15));
							Vector3 top2 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 40, vHeadBone.y - 40, vHeadBone.z + 15));
							Vector3 top3 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 40, vHeadBone.y + 40, vHeadBone.z + 15));
							Vector3 top4 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 40, vHeadBone.y + 40, vHeadBone.z + 15));

							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(top1.x, top1.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(top2.x, top2.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(top3.x, top3.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(top4.x, top4.y), ESPSkeleton, 0.1f);

							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(bottom2.x, bottom2.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(bottom3.x, bottom3.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(bottom4.x, bottom4.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(bottom1.x, bottom1.y), ESPSkeleton, 0.1f);

							ImGui::GetForegroundDrawList()->AddLine(ImVec2(top1.x, top1.y), ImVec2(top2.x, top2.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(top2.x, top2.y), ImVec2(top3.x, top3.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(top3.x, top3.y), ImVec2(top4.x, top4.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(top4.x, top4.y), ImVec2(top1.x, top1.y), ESPSkeleton, 0.1f);
						}
					}

					if (g_lineesp)
					{
						ImU32 LineColor;
						if (isVis)
							LineColor = ImGui::ColorConvertFloat4ToU32(ImVec4(ESPColor2.R / 128.0, ESPColor2.G / 224.0, ESPColor2.B / 0.0, ESPColor2.A / 255.0));
						else
							LineColor = ImGui::ColorConvertFloat4ToU32(ImVec4(ESPColor.R / 128.0, ESPColor.G / 224.0, ESPColor.B / 0.0, ESPColor.A / 255.0));

						ImGui::GetOverlayDrawList()->AddLine(
							ImVec2(Globals::ScreenCenterX, Globals::Height),
							ImVec2(RelativeScreenLocation.x, RelativeScreenLocation.y + 30),
							LineColor,
							0.1f
						);
					}
				}
				

				auto dx = w2shead.x - (Globals::Width / 2);
				auto dy = w2shead.y - (Globals::Height / 2);
				//auto dist = sqrtf(dx * dx + dy * dy) / 100.0f;
                                auto dist = sqrtf(dx * dx + dy * dy);
				auto isDBNO = (read<char>(g_pid, p.Acotr + 0x6f2) >> 4) & 1;

				if (dist < bA1mb0tF0VV4lue && dist < closestDistance && TeamIndex != LocalTeam && !InLobby)
				{
					if (g_skipknocked)
					{
						if (!isDBNO)
						{
							closestDistance = dist;
							closestPawn = p.Acotr;
						}
					}
					else
					{
						closestDistance = dist;
						closestPawn = p.Acotr;
					}

				}
			}
		}
		if (closestPawn != 0)
		{
			if (closestPawn && Key.IsKeyPushing(hotkeys::aimkey) or Controller::IsPressingLeftTrigger() && isFortniteFocused)
			{
				if (g_aimbot)
				{
					uint64_t AimbotMesh = read<uint64_t>(g_pid, closestPawn + 0x2F0);
					if (!AimbotMesh)
						return false;

					Vector3 HeadPosition = g_functions::f_getbonewithIndex(AimbotMesh, select_hitbox());
					if (!IsVec3Valid(HeadPosition))
						return false;

					Vector3 Head = g_functions::ConvertWorld2Screen(HeadPosition);
					if (!IsVec3Valid(HeadPosition))
						return false;

					if (Head.x != 0 || Head.y != 0 || Head.z != 0)
					{
						if ((GetDistance(Head.x, Head.y, Head.z, Globals::Width / 2, Globals::Height / 2) <= bA1mb0tF0VV4lue))
						{
							if (g_mouse_aim)
							{
								aimbot(Head.x, Head.y);
							}
							/*
							else if (g_mem_aim)
							{
								Vector3 HeadPosition = g_functions::f_getbonewithIndex(AimbotMesh, 78);

								if (!IsVec3Valid(HeadPosition))
									return false;

								//WriteAngles(HeadPosition);

							}

							if (g_exploits_backtrack)
							{
								if (GetAsyncKeyState(VK_LBUTTON)) 
								{
									//write<float>(g_pid, closestPawn + 0x64, 0.001f); // time dilation 0x98
								}
								else
								{
									//write<float>(g_pid, closestPawn + 0x64, 1); // time dilation 0x98
								}
							}*/
						}

					}
				}
			}
			else
			{
				closestDistance = FLT_MAX;
				closestPawn = NULL;
			}
		}
	}
	catch (...) {}
}

bool Head = true, Neck, Chest;

void runRenderTick() {
	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	RECT rect = { 0 };

	HWND GameWnd = FindWindowA(XorStr("UnrealWindow").c_str(), XorStr("Fortnite  ").c_str());

	HWND hwnd_active = GetForegroundWindow();
	HWND overlay_window = glfwGetWin32Window(g_window);

	if (hwnd_active == GameWnd || hwnd_active == overlay_window) {
		isFortniteFocused = true;
	}
	else {
		isFortniteFocused = true;
	}

	if (isFortniteFocused)
	{
		if (GetWindowRect(GameWnd, &rect))
		{
			Globals::Width = rect.right - rect.left;
			Globals::Height = rect.bottom - rect.top;
		}

		Globals::ScreenCenterX = (Globals::Width / 2.0f);
		Globals::ScreenCenterY = (Globals::Height / 2.0f);

		actorLoop();
	}

	float radiusx = bA1mb0tF0VV4lue * (Globals::ScreenCenterX / 100);
	float radiusy = bA1mb0tF0VV4lue * (Globals::ScreenCenterY / 100);

	float calcradius = (radiusx + radiusy) / 2;

	if (Globals::LocalPawn)
	{
		if (g_fov)
		{
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, &Col.white, 2);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.white, 2);
			DrawLine((float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.white, 2);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.white, 2);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, &Col.white, 3);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.white, 3);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, &Col.black_, 1);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.black_, 1);
			DrawLine((float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.black_, 1);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.black_, 1);
		}

		if (g_crossh) {
			DrawLine((float)(Globals::Width / 2), (float)(Globals::Height / 2) - 8, (float)(Globals::Width / 2), (float)(Globals::Height / 2) + 8, &Col.white, 1);
			DrawLine((float)(Globals::Width / 2) - 8, (float)(Globals::Height / 2), (float)(Globals::Width / 2) + 8, (float)(Globals::Height / 2), &Col.white, 1);
		}

		if (g_circlefov) {
			ImGui::GetOverlayDrawList()->AddCircle(ImVec2(Globals::ScreenCenterX, Globals::ScreenCenterY), bA1mb0tF0VV4lue, ImGui::ColorConvertFloat4ToU32(ImVec4(ESPColor.R / 255.0, ESPColor.G / 255.0, ESPColor.B / 255.0, ESPColor.A / 255.0)), 100);
		}
	}
	DrawText1(15, 15, "BigC - F1", &Col.black);

	char dist[64];
	sprintf_s(dist, "FPS %.f\n", ImGui::GetIO().Framerate);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(15, 40), ImColor(cRainbow), dist);

	if (g_overlay_visible)
	{
		//cursor();
		background();

	}
	ImGuiIO& io = ImGui::GetIO();
	static int Tab;
	if (g_overlay_visible) {
		{

			ImGuiStyle* style = &ImGui::GetStyle();
			style->WindowPadding = ImVec2(15, 15);
			style->WindowRounding = 5.0f;
			style->FramePadding = ImVec2(5, 5);
			style->FrameRounding = 4.0f;
			style->ItemSpacing = ImVec2(12, 8);
			style->ItemInnerSpacing = ImVec2(8, 6);
			style->IndentSpacing = 25.0f;
			style->ScrollbarSize = 15.0f;
			style->ScrollbarRounding = 9.0f;
			style->GrabMinSize = 5.0f;
			style->GrabRounding = 3.0f;
			style->ButtonTextAlign = ImVec2(0.5f, 0.5f);
			style->WindowTitleAlign = ImVec2(0.5f, 0.5f);

			style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
			style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
			style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
			style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
			style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
			style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
			style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
			style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
			style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
			style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
			style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
			style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
			style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
			style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);

			ImGui::SetNextWindowSize({ 800.f,450.f });

			ImGui::Begin("BigC Legit", NULL, ImGuiWindowFlags_NoScrollbar);
			ImGui::Columns(2, "##maincolumn", true);
			ImGui::SetColumnOffset(1, 225);

			ImGui::SetCursorPosX(74);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 9);
			//ImGui::Image((void*)logo, ImVec2(76, 87));
			ImGui::NewLine();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
			ImGui::SetCursorPosX(0);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
			ImGui::SetCursorPosX(0);
			if (ImGui::Button(" Aimbot", ImVec2(225, 46)))
			{
				Tab = 1;
			}
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
			ImGui::SetCursorPosX(0);
			//Gui::Seperator("##main_seperator_2", 225, 1);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
			ImGui::SetCursorPosX(0);
			if (ImGui::Button(" Visuals", ImVec2(225, 46)))
			{
				Tab = 2;
			}
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
			ImGui::SetCursorPosX(0);
			//Gui::Seperator("##main_seperator_3", 225, 1);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3); //+ goes down , - goes up
			ImGui::SetCursorPosX(0);
			if (ImGui::Button(" Loot", ImVec2(225, 46)))
			{
				Tab = 3;
			}
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
			ImGui::SetCursorPosX(0);
			//Gui::Seperator("##main_seperator_4", 225, 1);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
			ImGui::SetCursorPosX(0);
			if (ImGui::Button(" Config", ImVec2(225, 46)))
			{
				Tab = 4;
			}
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);
			ImGui::SetCursorPosX(0);
			//Gui::Seperator("##main_seperator_5", 225, 1);

			ImGui::SetCursorPosY(380);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 25);
			ImGui::Text("BigC Legit");
			ImGui::SetCursorPosY(399);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
			ImGui::Text("Build Date: May 6 2022");

			ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 10); //Same here
			ImGui::Text(""); //Makes sure that the column goes the whole gui height, so don't remove :)

			ImGui::NextColumn();


			if (Tab == 1)
			{

				ImGui::Checkbox(XorStr("Mouse Aimbot").c_str(), &g_mouse_aim);
				ImGui::Text("Aimkey");
				ImGui::SameLine();
				HotkeyButton(hotkeys::aimkey, ChangeKey, keystatus);
				ImGui::Checkbox(XorStr("Controller LTrigger").c_str(), &controller);
				ImGui::Checkbox(XorStr("Skip Knocked Players").c_str(), &g_skipknocked);
				ImGui::Text("Smoothness");
				ImGui::SliderFloat(XorStr("    ").c_str(), &bA1mb0tSm00th1ngV4lue, .5, 30);
				//ImGui::Checkbox(XorStr("Silent Test").c_str(), &guntest2);


			}
			else if (Tab == 2)
			{

				ImGui::Checkbox(XorStr("3D Box").c_str(), &g_3d_box);
				ImGui::Checkbox(XorStr("Corner").c_str(), &g_cornerboxesp);
				ImGui::Checkbox(XorStr("2D Box").c_str(), &g_boxesp);
				ImGui::Checkbox(XorStr("Snaplines").c_str(), &g_lineesp);
				ImGui::Checkbox(XorStr("Skeleton").c_str(), &g_esp_skeleton);
				ImGui::Checkbox(XorStr("Distance ESP").c_str(), &g_esp_distance);


			}
			else if (Tab == 3)
			{
				ImGui::Checkbox(XorStr("Loot ESP").c_str(), &g_loot);
				ImGui::Checkbox(XorStr("Utils ESP").c_str(), &g_utils);
				ImGui::Checkbox(XorStr("Vehicle Esp").c_str(), &g_vehicles);
				ImGui::Checkbox(XorStr("Chest ESP").c_str(), &g_chests);
				ImGui::Checkbox(XorStr("Ammo Box ESP").c_str(), &g_ammo);

			}
			else if (Tab == 4)
			{

				{
					ImGui::Checkbox(XorStr("Draw Crosshair").c_str(), &g_crossh);
					//	ImGui::Checkbox(XorStr("Double Pump").c_str(), &klixx);
					ImGui::Checkbox(XorStr("Draw Circle FOV").c_str(), &g_circlefov);


					if (g_fov || g_circlefov)
					{
						ImGui::SetCursorPos(ImVec2(140, 75));
						ImGui::Text("FOV");
						ImGui::SetCursorPos(ImVec2(140, 90));
						ImGui::SliderFloat(XorStr("    ").c_str(), &bA1mb0tF0VV4lue, 10, 1000);
					}


					//ImGui::Checkbox("Fov Changer", &g_fovchanger);
					//if (g_fovchanger)
					//{
						//ImGui::SetCursorPos(ImVec2(140, 55));
						//ImGui::Text("FOV CHANGER");
						//ImGui::SetCursorPos(ImVec2(140, 75));

//					}
					//ImGui::Checkbox(XorStr("No Spread").c_str(), &nigger);
					//ImGui::Checkbox(XorStr("Ads Mid Air").c_str(), &adsmidair);
					//ImGui::Checkbox(XorStr("Insta Res").c_str(), &instares);
					//ImGui::Checkbox(XorStr("Car Fly").c_str(), &carfly);
					//ImGui::Checkbox(XorStr("Bullet TP").c_str(), &bulletTP);
				}



				ImGui::End();
			}
		}
	}

	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(g_window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(g_window);
}

enum InjectedInputMouseOptions
{
    Absolute = 32768,
    HWheel = 4096,
    LeftDown = 2,
    LeftUp = 4,
    MiddleDown = 32,
    MiddleUp = 64,
    Move = 1,
    MoveNoCoalesce = 8192,
    None = 0,
    RightDown = 8,
    RightUp = 16,
    VirtualDesk = 16384,
    Wheel = 2048,
    XDown = 128,
    XUp = 256
};

typedef struct _InjectedInputMouseInfo
{
	int DeltaX;
	int DeltaY;
	unsigned int MouseData;
	InjectedInputMouseOptions MouseOptions;
	unsigned int TimeOffsetInMilliseconds;
	void* ExtraInfo;
} InjectedInputMouseInfo;

typedef bool (WINAPI* InjectMouseInput_t)(InjectedInputMouseInfo* inputs, int count);
InjectMouseInput_t InjectMouseInput;

int main() {
	InjectMouseInput = reinterpret_cast<InjectMouseInput_t>(GetProcAddress(LoadLibraryA("user32.dll"), "InjectMouseInput"));

	if (GlobalFindAtomA("innit??") == 0)
	{
		system(XorStr("taskkill /F /IM EpicGamesLauncher.exe").c_str());
		system(XorStr("taskkill /F /IM EasyAntiCheatLauncher.exe").c_str());
		system(XorStr("taskkill /F /IM BEService.exe").c_str());
		system(XorStr("taskkill /F /IM BattleEyeLauncher.exe").c_str());
		system(XorStr("taskkill /F /IM FortniteClient-Win64-Shipping.exe").c_str());
		system(XorStr("taskkill /F /IM FortniteLauncher.exe").c_str());

		VulnerableDriver::Init();
		GlobalAddAtomA("innit??");
	}


	HWND Entryhwnd = NULL;

	while (Entryhwnd == NULL)
	{
		printf(XorStr("Run Fortnite\r").c_str());
		Sleep(1);
		Entryhwnd = FindWindowA(XorStr("UnrealWindow").c_str(), XorStr("Fortnite  ").c_str());
		Sleep(1);
	}

	system(XorStr("cls").c_str());

	g_pid = get_fn_processid();

	if (!g_pid) {
		std::cout << XorStr("Could not find Fortnite.\n").c_str();
		system(XorStr("pause").c_str());
		return 1;
	}

	Drive.Init();

	EnumWindows(retreiveFortniteWindow, NULL);
	if (!fortnite_wnd) {
		std::cout << XorStr("Could not find Fortnite.\n");
		system(XorStr("pause").c_str());
		return 1;
	}

	g_base_address = getBaseAddress(g_pid);

	if (!pattern_uworld)
		pattern_uworld = find_signature(XorStr("\x48\x89\x05\x00\x00\x00\x00\x48\x8B\x4B\x78").c_str(), XorStr("xxx????xxxx").c_str());



	if (!g_base_address) {
		std::cout << XorStr("Could not get base address.\n").c_str();
		system(XorStr("pause").c_str());
		return 1;
	}

	setupWindow();
	if (!g_window) {
		std::cout << XorStr("Could not setup window.\n").c_str();
		system(XorStr("pause").c_str());
		return 1;
	}

	HWND ass = FindWindowA(nullptr, XorStr("xxx????xxxx").c_str());
	DWORD assid = 0;
	GetWindowThreadProcessId(ass, &assid);
	//wndhide::hide_window(assid, ass, true); // hide overlay using SetWindowDisplayAffinity

	HANDLE handle = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(CacheNew), nullptr, NULL, nullptr);
	CloseHandle(handle);

	while (!glfwWindowShouldClose(g_window))
	{
		handleKeyPresses();
		runRenderTick();
	}

	cleanupWindow();
	return 0;
}

void aimbot(float x, float y)
{
	float ScreenCenterX = (Globals::Width / 2);
	float ScreenCenterY = (Globals::Height / 2);
	int AimSpeed = bA1mb0tSm00th1ngV4lue;
	float TargetX = 0;
	float TargetY = 0;

	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	InjectedInputMouseInfo info = { 0 };
	info.DeltaX = TargetX;
	info.DeltaY = TargetY;
	InjectMouseInput(&info, 1);

	return;
}
