#define IMGUI_DEFINE_MATH_OPERATORS
#include <thread>
#include <iostream>
#include <atomic> 
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

#include "ui.hpp"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <src/Overlay/Overlay.hpp>
#include <src/Globals.hpp>
#include <imgui_internal.h>
#include <auth/MVPAuth.h>
#include <algorithm>
#include "src/adb/adb.hpp"
extern void adbInit();

#include <Memory/SmartyMem2.h>
#include <map>

// Declare WndProcHandler extern
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// New UI styling and assets headers
#include "imgui_edited.hpp"
#include "font2.h"
#include "icons.h"
#include "texture.h"
#include "ccccccc.h"

// Define namespaces for styles and custom notifications
using namespace ImGui;

// Textures loaded in DirectX
static ID3D11ShaderResourceView* gun1 = nullptr;
static ID3D11ShaderResourceView* LOGIN1 = nullptr;
static ID3D11ShaderResourceView* LOG1 = nullptr;
static ID3D11ShaderResourceView* REGISTER1 = nullptr;
static ID3D11ShaderResourceView* VISUALS1 = nullptr;
static ID3D11ShaderResourceView* BR91 = nullptr;
static ID3D11ShaderResourceView* SETI1 = nullptr;
static ID3D11ShaderResourceView* INFO1 = nullptr;

namespace texture
{
    ID3D11ShaderResourceView* esp_preview = nullptr;
    ID3D11ShaderResourceView* avatumxd = nullptr;
}

static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
HWND hwnd = nullptr;
static bool bUIInit = false;

// Variables from original ui.cpp
SmartyMemory smartyInstance;
std::string text = "YxN CHEATS x86 INTERNAL!";

void MouseMovement() {
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
        static ImVec2 dragOffset;

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            dragOffset = ImGui::GetMousePos();
            ImVec2 windowPos = ImGui::GetWindowPos();
            dragOffset.x -= windowPos.x;
            dragOffset.y -= windowPos.y;
        }

        ImVec2 mousePos = ImGui::GetMousePos();
        ImGui::SetWindowPos(ImVec2(mousePos.x - dragOffset.x, mousePos.y - dragOffset.y));
    }
}

// Particles effect from ADI CHEATS
void ParticlesCircle()
{
    ImVec2 screen_size = { (float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN) };

    static ImVec2 partile_pos[100];
    static ImVec2 partile_target_pos[100];
    static float partile_speed[500];
    static float partile_radius[100];

    for (int i = 1; i < 50; i++)
    {
        if (partile_pos[i].x == 0 || partile_pos[i].y == 0)
        {
            partile_pos[i].x = rand() % (int)screen_size.x + 1;
            partile_pos[i].y = 15.f;
            partile_speed[i] = 1 + rand() % 25;
            partile_radius[i] = rand() % 4;

            partile_target_pos[i].x = rand() % (int)screen_size.x;
            partile_target_pos[i].y = screen_size.y * 2;
        }

        partile_pos[i] = ImLerp(partile_pos[i], partile_target_pos[i], ImGui::GetIO().DeltaTime * (partile_speed[i] / 60));

        if (partile_pos[i].y > screen_size.y)
        {
            partile_pos[i].x = 0;
            partile_pos[i].y = 0;
        }

        ImGui::GetWindowDrawList()->AddCircleFilled(partile_pos[i], partile_radius[i], ImGui::GetColorU32(c::accent));
        ImGui::GetWindowDrawList()->AddShadowCircle(partile_pos[i], 2.f, ImGui::GetColorU32(c::accent), 43.f, ImVec2(0, 0), 0, 9);
    }
}

namespace FWork {
    void Interface::Initialize(HWND Window, HWND TargetWindow, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext) {
        hWindow = Window;
        IDevice = Device;
        g_pd3dDevice = Device;
        g_pd3dDeviceContext = DeviceContext;
        hwnd = Window;

        D3DX11_IMAGE_LOAD_INFO info;
        ID3DX11ThreadPump* pump{ nullptr };

        // Create shader resource views for new textures
        if (texture::esp_preview == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, esp_preview1, sizeof(esp_preview1), &info, pump, &texture::esp_preview, 0);
        if (texture::avatumxd == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, avatumxd1, sizeof(avatumxd1), &info, pump, &texture::avatumxd, 0);

        if (SETI1 == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, SETI, sizeof(SETI), &info, pump, &SETI1, 0);
        if (gun1 == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, gun, sizeof(gun), &info, pump, &gun1, 0);
        if (BR91 == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, BR9, sizeof(BR9), &info, pump, &BR91, 0);
        if (VISUALS1 == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, VISUALS, sizeof(VISUALS), &info, pump, &VISUALS1, 0);
        if (INFO1 == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, INFO, sizeof(INFO), &info, pump, &INFO1, 0);

        if (LOGIN1 == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, LOGIN, sizeof(LOGIN), &info, pump, &LOGIN1, 0);
        if (REGISTER1 == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, REGISTER, sizeof(REGISTER), &info, pump, &REGISTER1, 0);
        if (LOG1 == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, LOG, sizeof(LOG), &info, pump, &LOG1, 0);

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hWindow);
        ImGui_ImplDX11_Init(Device, DeviceContext);

        InitializeMenu();
        UpdateStyle();
    }

    void Interface::InitializeMenu() {
        bIsMenuOpen = true;
        SetWindowLong(hWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
        SetForegroundWindow(hWindow);
        SetWindowPos(hWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
    }

    void Interface::UpdateStyle() {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle* Style = &ImGui::GetStyle();
        Style->AntiAliasedLines = true;
        Style->AntiAliasedLinesUseTex = true;
        Style->AntiAliasedFill = true;
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;
        Style->WindowRounding = 10;
        Style->FrameRounding = 5.f;
        Style->ItemSpacing = ImVec2(10, 10);
        Style->WindowBorderSize = 0;
        Style->WindowPadding = ImVec2(0, 0);
        Style->ScrollbarSize = 8.f;
        Style->FramePadding = ImVec2(8, 4);
        Style->ScrollbarRounding = 10;
        Style->WindowShadowSize = 0;
        Style->PopupRounding = 10;

        // Initialize custom ADI CHEATS fonts
        ImFontConfig cfg;
        font::lexend_general_bold = io.Fonts->AddFontFromMemoryTTF(lexend_bold, sizeof(lexend_bold), 18.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        font::lexend_bold = io.Fonts->AddFontFromMemoryTTF(lexend_regular, sizeof(lexend_regular), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        font::lexend_regular = io.Fonts->AddFontFromMemoryTTF(lexend_regular, sizeof(lexend_regular), 14.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        font::icomoon = io.Fonts->AddFontFromMemoryTTF(icomoon, sizeof(icomoon), 20.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        font::icomoon_widget = io.Fonts->AddFontFromMemoryTTF(icomoon_widget, sizeof(icomoon_widget), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        font::icomoon_widget2 = io.Fonts->AddFontFromMemoryTTF(icomoon, sizeof(icomoon), 16.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
        font::ContiB = io.Fonts->AddFontFromMemoryTTF(continuum_bold, sizeof(continuum_bold), 30.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

        // Map font11 pointers to ensure notification system doesn't crash on null font
        font11::lexend_bold = font::lexend_bold;
        font11::lexend_regular = font::lexend_regular;
        font11::lexend_general_bold = font::lexend_general_bold;
        font11::icomoon = font::icomoon;
        font11::icomoon_widget = font::icomoon_widget;
        font11::icomoon_widget2 = font::icomoon_widget2;
        font11::ContiB = font::ContiB;
        font11::ContiM = font::ContiB;
    }

    void Interface::RenderGui()
    {
        if (!bIsMenuOpen)
            return;

        static bool reiman = false;
        static bool positioned = false;
        static int active_tab = 0;
        
        static char username[64] = "";
        static char password[64] = "";
        static bool auth_error = false;
        static std::string error_msg = "";
        static bool loginInProgress = false;

        // Custom notification system rendering
        notificationSystem.DrawNotifications();

        // Accent color setting (Red accent)
        static float color[4] = { 255 / 255.f, 0 / 255.f, 0 / 255.f, 1.f };
        c::accent = { color[0], color[1], color[2], 1.f };

        if (!reiman) // Login Page
        {
            float formWidth = 260.0f;
            float windowWidth1 = 335.0f;
            float windowHeight1 = 425.0f;

            ImGui::SetNextWindowSize(ImVec2(windowWidth1, windowHeight1));
            
            // Center the login window once
            if (!positioned) {
                ImGui::SetNextWindowPos(ImVec2(
                    (GetSystemMetrics(SM_CXSCREEN) - windowWidth1) * 0.5f,
                    (GetSystemMetrics(SM_CYSCREEN) - windowHeight1) * 0.5f
                ));
                positioned = true;
            }

            ImGui::Begin("IMGUI!", nullptr,
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoBringToFrontOnFocus);
            {
                ParticlesCircle();
                ImVec2 pos = ImGui::GetWindowPos();
                ImVec2 size = ImVec2(windowWidth1, windowHeight1);

                ImGui::GetBackgroundDrawList()->AddRectFilled(
                    pos, pos + size,
                    ImGui::GetColorU32(c::background::filling1),
                    c::background::rounding);

                MouseMovement();

                // Title
                ImGui::SetCursorPos(ImVec2(windowWidth1 / 2 - 50, 30));
                ImGui::PushFont(font::lexend_general_bold);
                ImGui::Text("LOGIN");
                ImGui::PopFont();

                float formStartX = (windowWidth1 - formWidth) / 2;
                float formStartY = 65.0f;

                ImGui::SetCursorPos(ImVec2(formStartX, formStartY));

                edited::BeginChild("##LoginForm", ImVec2(formWidth, 300), ImGuiChildFlags_None);
                {
                    ParticlesCircle();

                    ImDrawList* draw = ImGui::GetWindowDrawList();

                    static ImVec2 title_position = ImVec2(50, 10);
                    static float title_font_size = 25.0f;
                    static ImVec4 title_color_main = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                    static ImVec4 title_color_suffix = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

                    ImGui::PushFont(font::ContiB);
                    ImGui::SetWindowFontScale(title_font_size / ImGui::GetFontSize());

                    const char* brand_main = "REIMAN ";
                    const char* brand_suffix = "PRIME";
                    ImVec2 text_size_main = ImGui::CalcTextSize(brand_main);

                    draw->AddText(
                        ImVec2(pos.x + formStartX + title_position.x, pos.y + formStartY + title_position.y),
                        ImGui::ColorConvertFloat4ToU32(title_color_main),
                        brand_main
                    );
                    draw->AddText(
                        ImVec2(pos.x + formStartX + title_position.x + text_size_main.x, pos.y + formStartY + title_position.y),
                        ImGui::ColorConvertFloat4ToU32(title_color_suffix),
                        brand_suffix
                    );

                    ImGui::SetWindowFontScale(1.0f);
                    ImGui::PopFont();
                    
                    ImGui::SetCursorPos(ImVec2(10, 60));
                    ImGui::Text("Username");
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
                    ImGui::InputTextEx("##Username", "Enter username", username, 64, ImVec2(formWidth - 20, 36), NULL);

                    ImGui::SetCursorPos(ImVec2(10, 120));
                    ImGui::Text("Password");
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
                    ImGui::InputTextEx("##Password", "Enter password", password, 64, ImVec2(formWidth - 20, 36), ImGuiInputTextFlags_Password);

                    // Game Selection Select Combo
                    static int selectedGame = 0;
                    const char* games[] = { "FF NORMAL (ARM)", "FF MAX (x86)" };
                    ImGui::SetCursorPos(ImVec2(10, 180));
                    ImGui::SetNextItemWidth(formWidth - 20);
                    if (edited::Combo("Select Game", "Choose target version", &selectedGame, games, IM_ARRAYSIZE(games))) {
                        g_Globals.General.IsMaxSource = (selectedGame == 1);
                    }

                    if (auth_error) {
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
                        ImGui::PushFont(font::lexend_regular);
                        float errWidth = ImGui::CalcTextSize(error_msg.c_str()).x;
                        ImGui::SetCursorPosX((formWidth - errWidth) * 0.5f);
                        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", error_msg.c_str());
                        ImGui::PopFont();
                    }

                    if (loginInProgress) {
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
                        float loadWidth = ImGui::CalcTextSize("Logging in...").x;
                        ImGui::SetCursorPosX((formWidth - loadWidth) * 0.5f);
                        ImGui::Text("Logging in...");
                    }

                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
                    ImGui::SetCursorPosX((formWidth - 180) / 2);
                    if (ImGui::Button("Login", ImVec2(180, 36)))
                    {
                        loginInProgress = true;
                        auth_error = false;
                        error_msg.clear();

                        std::thread([&]()
                        {
                            if (MVPAuthApp.Init())
                            {
                                bool success = MVPAuthApp.Login(username, password);
                                if (success)
                                {
                                    reiman = true;
                                    positioned = false;
                                    notificationSystem.AddNotification("Done", "Success Login", ImGui::GetColorU32(c11::accent));
                                    std::thread([] { adbInit(); }).detach();
                                }
                                else
                                {
                                    reiman = false;
                                    auth_error = true;
                                    error_msg = !MVPAuthApp.LastMessage.empty() ? MVPAuthApp.LastMessage : "Login failed.";
                                }
                            }
                            else
                            {
                                reiman = false;
                                auth_error = true;
                                error_msg = !MVPAuthApp.LastMessage.empty() ? MVPAuthApp.LastMessage : "Init failed.";
                            }
                            loginInProgress = false;
                        }).detach();
                    }
                }
                edited::EndChild();

                ImGui::PushFont(font::ContiB);
                ImGui::SetCursorPos(ImVec2(0, 385));
                ImGui::Text("Don't have an account? ");
                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
                ImGui::Selectable("Signup.", false, 0, ImGui::CalcTextSize("Signup."));
                ImGui::PopStyleColor();

                if (ImGui::IsItemHovered())
                {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        ShellExecuteA(NULL, "open", "https://discord.gg/WNYg3EVmBc", NULL, NULL, SW_SHOWNORMAL);
                    }
                }
                ImGui::PopFont();
            }
            ImGui::End();
        }
        else // Main Menu
        {
            float windowWidth = 860.0f;
            float windowHeight = 500.0f;

            ImGui::SetNextWindowSize(c::background::size);
            
            // Center the main window once
            if (!positioned) {
                ImGui::SetNextWindowPos(ImVec2(
                    (GetSystemMetrics(SM_CXSCREEN) - windowWidth) * 0.5f,
                    (GetSystemMetrics(SM_CYSCREEN) - windowHeight) * 0.5f
                ));
                positioned = true;
            }

            Begin("IMGUI!", nullptr,
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoScrollWithMouse |
                ImGuiWindowFlags_NoBringToFrontOnFocus);
            {
                const ImVec2& pos = GetWindowPos();

                GetBackgroundDrawList()->AddRectFilled(
                    pos, pos + c::background::size,
                    GetColorU32(c::background::filling),
                    c::background::rounding);

                GetBackgroundDrawList()->AddRect(
                    pos, pos + c::background::size,
                    GetColorU32(c::background::stroke),
                    c::background::rounding);

                MouseMovement();

                ImGui::SetCursorPos(ImVec2(0, 455));

                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                ParticlesCircle();
                float childWidth = 1200.0f;
                ImGui::BeginChild("TabsChildWindow", ImVec2(childWidth, 0), true,
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoDecoration |
                    ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoScrollWithMouse);
                {
                    auto draw = ImGui::GetWindowDrawList();
                    draw->Flags |= ImDrawListFlags_AntiAliasedLines | ImDrawListFlags_AntiAliasedFill;

                    const float boxSize = 40.0f;
                    const float iconSize = 24.0f;
                    const float rounding = 4.5f;

                    const bool enableOuterGlow = true;
                    const float outerGlowOffset = 1.5f;
                    const float outerGlowThickness = 1.5f;
                    const ImColor outerGlowColorActive = IM_COL32(255, 30, 30, 200);

                    const bool enableMainBorder = true;
                    const float mainBorderThicknessActive = 1.8f;
                    const float mainBorderThicknessInactive = 1.4f;
                    const ImColor mainBorderColorActive = IM_COL32(255, 40, 40, 255);
                    const ImColor mainBorderColorInactive = IM_COL32(120, 20, 20, 220);

                    const bool enableInnerBorder = true;
                    const float innerBorderOffset = 2.5f;
                    const float innerBorderThickness = 1.2f;
                    const ImColor innerBorderColorActive = IM_COL32(200, 50, 50, 200);
                    const ImColor innerBorderColorInactive = IM_COL32(80, 25, 25, 150);

                    const bool enableShadow = true;
                    const float shadowOffsetActive1 = 6.0f;
                    const float shadowOffsetActive2 = 4.0f;
                    const float shadowOffsetInactive = 3.5f;

                    struct TabConfig {
                        const char* name;
                        int index;
                        ImVec2 pos;
                        ImTextureID tex;
                    };

                    TabConfig tabs[] = {
                        {"Aimbot", 0, ImVec2(250, 10), (ImTextureID)gun1},
                        {"Visual", 1, ImVec2(300, 10), (ImTextureID)BR91},
                        {"Brutal", 2, ImVec2(350, 10), (ImTextureID)SETI1},
                        {"EXTRA", 3, ImVec2(400, 10), (ImTextureID)VISUALS1},
                        {"Misc",   4, ImVec2(450, 10), (ImTextureID)INFO1},
                    };

                    for (auto& tab : tabs)
                    {
                        ParticlesCircle();
                        ImGui::SetCursorPos(tab.pos);
                        ImVec2 pos = ImGui::GetCursorScreenPos();

                        bool active = (active_tab == tab.index);

                        ImVec2 p1 = pos;
                        ImVec2 p2 = p1 + ImVec2(boxSize, boxSize);

                        if (enableShadow)
                        {
                            if (active)
                            {
                                draw->AddRectFilled(p1 + ImVec2(shadowOffsetActive1, shadowOffsetActive1),
                                    p2 + ImVec2(shadowOffsetActive1, shadowOffsetActive1),
                                    IM_COL32(0, 0, 0, 120), rounding);
                                draw->AddRectFilled(p1 + ImVec2(shadowOffsetActive2, shadowOffsetActive2),
                                    p2 + ImVec2(shadowOffsetActive2, shadowOffsetActive2),
                                    IM_COL32(0, 0, 0, 160), rounding);
                            }
                            else
                            {
                                draw->AddRectFilled(p1 + ImVec2(shadowOffsetInactive, shadowOffsetInactive),
                                    p2 + ImVec2(shadowOffsetInactive, shadowOffsetInactive),
                                    IM_COL32(0, 0, 0, 100), rounding);
                            }
                        }

                        draw->PushClipRect(p1, p2, true);
                        if (active)
                        {
                            draw->AddRectFilledMultiColor(p1, p2,
                                IM_COL32(220, 0, 0, 255),
                                IM_COL32(60, 0, 0, 255),
                                IM_COL32(220, 0, 0, 255),
                                IM_COL32(60, 0, 0, 255)
                            );
                        }
                        else
                        {
                            draw->AddRectFilledMultiColor(p1, p2,
                                IM_COL32(70, 0, 0, 255),
                                IM_COL32(15, 0, 0, 255),
                                IM_COL32(70, 0, 0, 255),
                                IM_COL32(15, 0, 0, 255)
                            );
                        }
                        draw->PopClipRect();

                        if (enableOuterGlow && active)
                        {
                            draw->AddRect(p1 - ImVec2(outerGlowOffset, outerGlowOffset),
                                p2 + ImVec2(outerGlowOffset, outerGlowOffset),
                                outerGlowColorActive, rounding, 0, outerGlowThickness);
                        }

                        if (enableMainBorder)
                        {
                            draw->AddRect(p1, p2,
                                active ? mainBorderColorActive : mainBorderColorInactive,
                                rounding, 0,
                                active ? mainBorderThicknessActive : mainBorderThicknessInactive);
                        }

                        if (enableInnerBorder)
                        {
                            draw->AddRect(p1 + ImVec2(innerBorderOffset, innerBorderOffset),
                                p2 - ImVec2(innerBorderOffset, innerBorderOffset),
                                active ? innerBorderColorActive : innerBorderColorInactive,
                                rounding, 0, innerBorderThickness);
                        }

                        ImGui::SetCursorScreenPos(p1);
                        ImGui::InvisibleButton(tab.name, ImVec2(boxSize, boxSize));

                        ImVec2 iconPos = p1 + ImVec2((boxSize - iconSize) / 2, (boxSize - iconSize) / 2);
                        draw->AddImage(tab.tex, iconPos, iconPos + ImVec2(iconSize, iconSize),
                            ImVec2(0, 0), ImVec2(1, 1),
                            ImColor(255, 255, 255, active ? 255 : (int)(255 * 0.7f)));

                        if (ImGui::IsItemHovered())
                        {
                            draw->AddRectFilled(p1, p2, IM_COL32(255, 255, 255, 20), rounding);
                        }

                        if (ImGui::IsItemClicked())
                            active_tab = tab.index;
                    }
                }
                ImGui::EndChild();
                ImGui::PopStyleColor();

                // Render User info in menu top-right
                std::string user = "User: " + MVPAuthApp.User.Username;
                std::string expdate = "Expiry: " + MVPAuthApp.User.Expiry;

                ImGui::PushFont(font11::lexend_bold);
                float username_x = 530.0f;
                float username_y = 15.0f;
                float expiry_y = 35.0f;

                ImGui::SetCursorPos(ImVec2(username_x, username_y));
                ImGui::TextColored(c::accent, user.c_str());

                ImGui::SetCursorPos(ImVec2(username_x, expiry_y));
                ImGui::TextColored(c::accent - ImVec4(0.f, 0.f, 0.f, 0.6f), expdate.c_str());
                ImGui::PopFont();

                // Brand text in header
                ImDrawList* draw = ImGui::GetWindowDrawList();
                static ImVec2 title_position = ImVec2(260, 20);
                static float title_font_size = 30.0f;
                static ImVec4 title_color_main = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                static ImVec4 title_color_suffix = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

                ImGui::PushFont(font::ContiB);
                ImGui::SetWindowFontScale(title_font_size / ImGui::GetFontSize());

                const char* brand_main = "REIMAN ";
                const char* brand_suffix = "PRIME";
                ImVec2 text_size_main = ImGui::CalcTextSize(brand_main);

                draw->AddText(
                    ImVec2(pos.x + title_position.x, pos.y + title_position.y),
                    ImGui::ColorConvertFloat4ToU32(title_color_main),
                    brand_main
                );
                draw->AddText(
                    ImVec2(pos.x + title_position.x + text_size_main.x, pos.y + title_position.y),
                    ImGui::ColorConvertFloat4ToU32(title_color_suffix),
                    brand_suffix
                );

                ImGui::SetWindowFontScale(1.0f);
                ImGui::PopFont();

                // TAB 0: Aimbot
                if (active_tab == 0)
                {
                    ImGui::SetCursorPos(ImVec2(50, 70));
                    edited::BeginChild("##Container0", ImVec2(300, 380), NULL);
                    {
                        ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "Aimbot Features");
                        ImGui::Separator();

                        edited::Checkbox("Enable Aimbot", "Primary Aim Assist System", &g_Globals.AimBot.Enable);
                        edited::Checkbox("Silent Aim", "Redirects Bullets Invisibly", &g_Globals.AimBot.silentaim);
                        edited::Checkbox("Aim-line nearest", "Target closest to crosshair line", &g_Globals.AimBot.Prevision);
                        edited::Checkbox("Ignore Bot Training", "Skip Bots in Practice Area", &g_Globals.AimBot.IgnoreBots);
                    }
                    edited::EndChild();

                    ImGui::SetCursorPos(ImVec2(400, 70));
                    edited::BeginChild("##Container1", ImVec2(300, 380), NULL);
                    {
                        ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "Aimbot Settings");
                        ImGui::Separator();

                        edited::Checkbox("Show FOV Circle", "Draw boundary lines around crosshair", &g_Globals.Misc.ShowAimbotFov);
                        
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
                        ImGui::TextDisabled("Aimbot FOV Range");
                        edited::SliderFloat("FOV Size", "", &g_Globals.AimBot.Fov, 0.0f, 600.0f, "%.2f");

                        edited::Checkbox("Rapid Fire", "Increases Weapon Fire Rate", &g_Globals.Misc.RapidFire);
                        edited::Checkbox("No Recoil", "Eliminates Gun Shaking/Recoil", &g_Globals.Misc.NoRecoil);
                    }
                    edited::EndChild();
                }

                // TAB 1: Visuals (ESP)
                if (active_tab == 1)
                {
                    ImGui::SetCursorPos(ImVec2(50, 70));
                    edited::BeginChild("##VISUALS", ImVec2(300, 380), NULL);
                    {
                        ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "ESP Features");
                        ImGui::Separator();

                        edited::Checkbox("Enable ESP Overlay", "Master ESP Switcher", &g_Globals.Visuals.Enable);
                        edited::Checkbox("ESP Lines", "Render targeting snap lines", &g_Globals.Visuals.Lines);
                        edited::Checkbox("ESP Box", "Draw bounding boxes around targets", &g_Globals.Visuals.Box);
                        edited::Checkbox("ESP Fill Box", "Fills bounding boxes with color", &g_Globals.Visuals.FillColorBox);
                        edited::Checkbox("ESP Health Bar", "Render target HP status indicator", &g_Globals.Visuals.HealthBar);
                        edited::Checkbox("ESP Name", "Displays player names on target", &g_Globals.Visuals.Name);
                        edited::Checkbox("ESP Distance", "Displays distance metrics next to player", &g_Globals.Visuals.Distance);
                        edited::Checkbox("ESP Weapon", "Displays weapon names on target", &g_Globals.Visuals.esparmas);
                        edited::Checkbox("ESP Icon Weapon", "Displays weapon icon sprites on target", &g_Globals.Visuals.showIcons);
                    }
                    edited::EndChild();

                    ImGui::SetCursorPos(ImVec2(400, 70));
                    edited::BeginChild("##ESP", ImVec2(300, 380), NULL);
                    {
                        ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "ESP Settings");
                        ImGui::Separator();

                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
                        ImGui::TextDisabled("ESP Maximum Distance");
                        edited::SliderInt("ESP Distance", "", &g_Globals.Visuals.DistanceEsp, 0, 500, "%dm");

                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
                        const char* lineOptions[] = { "None", "Top Line", "Bottom Line" };
                        edited::Combo("ESP Line Anchor", "Choose line start anchor point", &g_Globals.Visuals.EspLines, lineOptions, IM_ARRAYSIZE(lineOptions));

                        const char* healthBarOptions[] = { "None", "Left Side", "Right Side", "Down Side" };
                        edited::Combo("ESP Bar Position", "Choose healthbar position", &g_Globals.Visuals.players_healthbar, healthBarOptions, IM_ARRAYSIZE(healthBarOptions));

                        const char* boxTypes[] = { "None", "Full Box", "Cornered Box" };
                        edited::Combo("ESP Box Type", "Choose boundary box render mode", &g_Globals.Visuals.players_box, boxTypes, IM_ARRAYSIZE(boxTypes));
                    }
                    edited::EndChild();
                }

                // TAB 2: Brutal (Aggressive features)
                if (active_tab == 2)
                {
                    ImGui::SetCursorPos(ImVec2(50, 70));
                    edited::BeginChild("##BrutalTab", ImVec2(300, 380), NULL);
                    {
                        ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "Aggressive Target Adjustments");
                        ImGui::Separator();

                        edited::Checkbox("Shake Kill", "Shakes crosshair to secure hits", &g_Globals.AimBot.ShakeKill);
                        edited::Checkbox("Up Player", "Adjusts target height upward", &g_Globals.AimBot.UPPlayer);
                        edited::Checkbox("Down Player", "Adjusts target height downward", &g_Globals.AimBot.DownPlayer);
                    }
                    edited::EndChild();

                    ImGui::SetCursorPos(ImVec2(400, 70));
                    edited::BeginChild("##BrutalTabRight", ImVec2(300, 380), NULL);
                    {
                        ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "Teleport & Magnet Exploits");
                        ImGui::Separator();

                        edited::Checkbox("TeleKill", "Secure kills from range instantly", &g_Globals.AimBot.telekill);
                        edited::Checkbox("Teleport (200m)", "Teleports bullets up to 200m", &g_Globals.AimBot.teleport);
                        edited::Checkbox("Magnet Pull Head", "Pulls bullets to target's head", &g_Globals.AimBot.MagnetHead);
                        edited::Checkbox("Magnet Pull Body", "Pulls bullets to target's body", &g_Globals.AimBot.MagnetBody);
                    }
                    edited::EndChild();
                }

                // TAB 3: EXTRA (Color Picker & Settings)
                if (active_tab == 3)
                {
                    ImGui::SetCursorPos(ImVec2(50, 70));
                    edited::BeginChild("##BrutalTab_Left", ImVec2(300, 380), NULL);
                    {
                        ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "ESP Colors Left");
                        ImGui::Separator();

                        auto ColorPick = [&](const char* name, const char* desc, float* col) {
                            ImGui::TextDisabled("%s", name);
                            edited::ColorEdit4(name, desc, col);
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
                        };

                        ColorPick("Name Color", "Color of player name", g_Globals.Visuals.NameColor);
                        ColorPick("Distance Color", "Color of distance indicator", g_Globals.Visuals.DistColor);
                        ColorPick("Box Color", "Color of bounding boxes", g_Globals.Visuals.BoxColor);
                        ColorPick("Fill Color", "Color of bounding boxes filling", g_Globals.Visuals.FillColor);
                    }
                    edited::EndChild();

                    ImGui::SetCursorPos(ImVec2(400, 70));
                    edited::BeginChild("##BrutalTab_Right", ImVec2(300, 380), NULL);
                    {
                        ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "ESP Colors Right");
                        ImGui::Separator();

                        auto ColorPick = [&](const char* name, const char* desc, float* col) {
                            ImGui::TextDisabled("%s", name);
                            edited::ColorEdit4(name, desc, col);
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
                        };

                        ColorPick("Skeleton Color", "Color of target bone lines", g_Globals.Visuals.SkeletonColor);
                        ColorPick("Weapon Color", "Color of active weapon label", g_Globals.Visuals.GunColor);
                        ColorPick("Snap-Line Color", "Color of snap lines", g_Globals.Visuals.LinesColor);

                        edited::Checkbox("Enable RGB Spectrum Effect", "Cycles colors dynamically", &g_Globals.Visuals.RGB);
                    }
                    edited::EndChild();
                }

                // TAB 4: Misc (Settings, Binds, Unload)
                if (active_tab == 4)
                {
                    ImGui::SetCursorPos(ImVec2(50, 70));
                    edited::BeginChild("##Misc_Left", ImVec2(300, 380), NULL);
                    {
                        ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "Cheat Core Settings");
                        ImGui::Separator();

                        edited::Keybind("Hide / Show Panel", "Shortcut key to hide menu", &g_Globals.General.KeyBindINT);
                        
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
                        ImGui::TextDisabled("Loop Execution Delay");
                        edited::SliderInt("Execution Delay", "", &g_Globals.General.Delay, 0, 100, "%dms");

                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 30);
                        if (ImGui::Button("UNLOAD CHEAT", ImVec2(240, 35))) {
                            g_Globals.General.ShutDown = true;
                        }
                    }
                    edited::EndChild();

                    ImGui::SetCursorPos(ImVec2(400, 70));
                    edited::BeginChild("##Misc_Right", ImVec2(300, 380), NULL);
                    {
                        ImGui::TextColored(ImColor(GetColorU32(c::elements::text)), "Other Settings");
                        ImGui::Separator();

                        edited::Checkbox("Stream Mode", "Hide cheat drawings on streaming windows", &g_Globals.General.Capture);
                    }
                    edited::EndChild();
                }
            }
            End();
        }
    }

    void Interface::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_SIZE:
            if (wParam != SIZE_MINIMIZED) {
                ResizeWidht = (UINT)LOWORD(lParam);
                ResizeHeight = (UINT)HIWORD(lParam);
            }
            break;
        }

        if (bIsMenuOpen) {
            ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        }
    }

    void Interface::HandleMenuKey()
    {
        static bool MenuKeyDown = false;
        if (GetAsyncKeyState(g_Globals.General.KeyBindINT) & 0x8000)
        {
            if (!MenuKeyDown)
            {
                MenuKeyDown = true;
                bIsMenuOpen = !bIsMenuOpen;

                if (bIsMenuOpen) {
                    SetWindowLong(hWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
                    SetForegroundWindow(hWindow);
                }
                else {
                    SetWindowLong(hWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_NOACTIVATE);
                    SetForegroundWindow(hTargetWindow);
                }
                SetWindowPos(hWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
            }
        }
        else {
            MenuKeyDown = false;
        }
    }

    void Interface::ShutDown() {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        Overlay::ShutDown();
    }
}