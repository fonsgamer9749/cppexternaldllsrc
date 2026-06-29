#include <thread>
#include <iostream>
#include <atomic> 
#include <string>
#include <vector>

#include "Visual.hpp"
#include <src/Globals.hpp>
#include <EspLines/Math/WordToScreen.hpp>
#include <EspLines/Math/Vector/Vector2.hpp>
#include <Windows.h>
#include <imgui.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <imgui_internal.h>
#include <src/Fonts/Fonts.hpp>
#include <src/Fonts/FontAwesome.hpp>
#include "NameGUn.h"
#include <src/Overlay/Overlay.hpp>
#include <sstream>
#include <iomanip>

void DrawVerticalHealthBar(short CurrentHealth, short MaxHealth, ImVec2 Position, float TotalHeight, int GlowRadius = 10) {
	ImDrawList* DrawList = ImGui::GetForegroundDrawList();

	float HealthPercentage = static_cast<float>(CurrentHealth) / MaxHealth;
	float FilledBarHeight = TotalHeight * HealthPercentage;
	float BarWidth = 2.5f;

	ImVec4 GreenColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	ImVec4 YellowColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
	ImVec4 RedColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

	ImVec4 HealthBarColor;
	if (HealthPercentage > 0.5f) {
		HealthBarColor = ImLerp(GreenColor, YellowColor, (1.0f - HealthPercentage) * 2.0f);
	}
	else {
		HealthBarColor = ImLerp(YellowColor, RedColor, (0.5f - HealthPercentage) * 2.0f);
	}

	ImU32 BarColorU32 = ImGui::ColorConvertFloat4ToU32(HealthBarColor);

	DrawList->AddRectFilled(
		ImVec2(Position.x, Position.y),
		ImVec2(Position.x + BarWidth, Position.y + TotalHeight),
		IM_COL32(0, 0, 0, 128)
	);

	ImVec4 GlowColor;
	if (HealthPercentage > 0.5f) {
		GlowColor = GreenColor;
	}
	else if (HealthPercentage > 0.25f) {
		GlowColor = YellowColor;
	}
	else {
		GlowColor = RedColor;
	}

	for (int i = GlowRadius; i > 0; --i) {
		float Alpha = 15.0f * (1.0f - ((float)i / GlowRadius));
		DrawList->AddRectFilled(
			ImVec2(Position.x - i, Position.y + (TotalHeight - FilledBarHeight) - i),
			ImVec2(Position.x + BarWidth + i, Position.y + TotalHeight + i),
			IM_COL32((int)(GlowColor.x * 255), (int)(GlowColor.y * 255), (int)(GlowColor.z * 255), (int)Alpha)
		);
	}

	DrawList->AddRectFilled(
		ImVec2(Position.x, Position.y + (TotalHeight - FilledBarHeight)),
		ImVec2(Position.x + BarWidth, Position.y + TotalHeight),
		BarColorU32
	);
}

void DrawHealthBarBelowFullBox(short CurrentHealth, short MaxHealth, float x, float y, float w, float h) {
	ImDrawList* DrawList = ImGui::GetForegroundDrawList();

	float HealthPercentage = static_cast<float>(CurrentHealth) / MaxHealth;
	float FilledBarWidth = w * HealthPercentage;
	float BarHeight = 3.5f;
	float offsetY = 3.5f;
	float rounding = 0.8f;
	float outlineSize = 1.5f;
	int GlowRadius = 8;

	ImVec4 GreenColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	ImVec4 YellowColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
	ImVec4 RedColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

	ImVec4 HealthBarColor;
	if (HealthPercentage > 0.5f) {
		HealthBarColor = ImLerp(GreenColor, YellowColor, (1.0f - HealthPercentage) * 2.0f);
	}
	else {
		HealthBarColor = ImLerp(YellowColor, RedColor, (0.5f - HealthPercentage) * 2.0f);
	}

	ImVec4 GlowColor;
	if (HealthPercentage > 0.5f) {
		GlowColor = GreenColor;
	}
	else if (HealthPercentage > 0.25f) {
		GlowColor = YellowColor;
	}
	else {
		GlowColor = RedColor;
	}

	ImU32 BarColorU32 = ImGui::ColorConvertFloat4ToU32(HealthBarColor);
	ImU32 OutlineColor = IM_COL32(0, 0, 0, 200);

	for (int i = GlowRadius; i > 0; --i) {
		float Alpha = 15.0f * (1.0f - ((float)i / GlowRadius));
		DrawList->AddRectFilled(
			ImVec2(x - i, y + h + offsetY - i),
			ImVec2(x + FilledBarWidth + i, y + h + offsetY + BarHeight + i),
			IM_COL32((int)(GlowColor.x * 255), (int)(GlowColor.y * 255), (int)(GlowColor.z * 255), (int)Alpha),
			rounding
		);
	}

	DrawList->AddRect(
		ImVec2(x, y + h + offsetY),
		ImVec2(x + w, y + h + offsetY + BarHeight),
		OutlineColor,
		rounding,
		ImDrawFlags_RoundCornersAll,
		outlineSize
	);

	if (FilledBarWidth > 0) {
		float actualRounding = (FilledBarWidth < rounding * 2) ? FilledBarWidth / 2 : rounding;

		DrawList->AddRectFilled(
			ImVec2(x, y + h + offsetY),
			ImVec2(x + FilledBarWidth, y + h + offsetY + BarHeight),
			BarColorU32,
			actualRounding,
			ImDrawFlags_RoundCornersAll
		);

		if (FilledBarWidth < w) {
			DrawList->AddRectFilled(
				ImVec2(x + FilledBarWidth - 0.5f, y + h + offsetY),
				ImVec2(x + FilledBarWidth + 0.5f, y + h + offsetY + BarHeight),
				OutlineColor
			);
		}
	}
}

void DrawGlowCorneredBox(float x, float y, float w, float h, ImColor color, float thickness, float glowRadius, float feather, bool fillEnabled) {
	auto drawList = ImGui::GetForegroundDrawList();

	float newWidth = w;
	float newHeight = h * 1.15f;
	float newY = y - (newHeight - h);

	if (g_Globals.Visuals.FillColorBox) {
		drawList->AddRectFilled(
			ImVec2(x, newY),
			ImVec2(x + newWidth, newY + newHeight),
			ImColor(
				g_Globals.Visuals.FillColor[0],
				g_Globals.Visuals.FillColor[1],
				g_Globals.Visuals.FillColor[2],
				g_Globals.Visuals.FillColor[3]
			)
		);
	}

	for (int i = (int)glowRadius; i > 0; --i) {
		float alphaFactor = 25.0f * (1.0f - ((float)i / glowRadius));
		ImColor glowColor = ImColor(color.Value.x, color.Value.y, color.Value.z, alphaFactor / 255.0f);

		drawList->AddLine(ImVec2(x - i, newY - i), ImVec2(x + newWidth / 6 + i, newY - i), glowColor, thickness);
		drawList->AddLine(ImVec2(x - i, newY - i), ImVec2(x - i, newY + newHeight / 6 + i), glowColor, thickness);

		drawList->AddLine(ImVec2(x + newWidth + i, newY - i), ImVec2(x + newWidth - newWidth / 6 - i, newY - i), glowColor, thickness);
		drawList->AddLine(ImVec2(x + newWidth + i, newY - i), ImVec2(x + newWidth + i, newY + newHeight / 6 + i), glowColor, thickness);

		drawList->AddLine(ImVec2(x - i, newY + newHeight + i), ImVec2(x + newWidth / 3 + i, newY + newHeight + i), glowColor, thickness);
		drawList->AddLine(ImVec2(x - i, newY + newHeight + i), ImVec2(x - i, newY + newHeight - newHeight / 6 - i), glowColor, thickness);

		drawList->AddLine(ImVec2(x + newWidth + i, newY + newHeight + i), ImVec2(x + newWidth - newWidth / 6 - i, newY + newHeight + i), glowColor, thickness);
		drawList->AddLine(ImVec2(x + newWidth + i, newY + newHeight + i), ImVec2(x + newWidth + i, newY + newHeight - newHeight / 6 - i), glowColor, thickness);
	}

	drawList->AddLine(ImVec2(x, newY), ImVec2(x + newWidth / 6, newY), color, thickness);
	drawList->AddLine(ImVec2(x, newY), ImVec2(x, newY + newHeight / 6), color, thickness);

	drawList->AddLine(ImVec2(x + newWidth, newY), ImVec2(x + newWidth - newWidth / 6, newY), color, thickness);
	drawList->AddLine(ImVec2(x + newWidth, newY), ImVec2(x + newWidth, newY + newHeight / 6), color, thickness);

	drawList->AddLine(ImVec2(x, newY + newHeight), ImVec2(x + newWidth / 6, newY + newHeight), color, thickness);
	drawList->AddLine(ImVec2(x, newY + newHeight), ImVec2(x, newY + newHeight - newHeight / 6), color, thickness);

	drawList->AddLine(ImVec2(x + newWidth, newY + newHeight), ImVec2(x + newWidth - newWidth / 6, newY + newHeight), color, thickness);
	drawList->AddLine(ImVec2(x + newWidth, newY + newHeight), ImVec2(x + newWidth, newY + newHeight - newHeight / 6), color, thickness);
}

void DrawFullBox(float x, float y, float w, float h, ImColor color, float thickness) {
	auto drawList = ImGui::GetForegroundDrawList();

	float newWidth = w * 0.90f;
	float newHeight = h * 1.17f;
	float newX = x + (w - newWidth) / 2.0f;
	float newY = y - (newHeight - h) + 3.0f; // +10 píxeles hacia abajo

	if (g_Globals.Visuals.FillColorBox) {
		drawList->AddRectFilled(
			ImVec2(newX, newY),
			ImVec2(newX + newWidth, newY + newHeight),
			ImColor(
				g_Globals.Visuals.FillColor[0],
				g_Globals.Visuals.FillColor[1],
				g_Globals.Visuals.FillColor[2],
				g_Globals.Visuals.FillColor[3]
			)
		);
	}

	drawList->AddRect(
		ImVec2(newX, newY),
		ImVec2(newX + newWidth, newY + newHeight),
		color,
		0.9f,
		ImDrawFlags_None,
		thickness * 0.5f
	);
}

void ESP::Players() {
	HWND targetWnd = FWork::Overlay::GetTargetWindow();
	if (!targetWnd || !IsWindow(targetWnd) || IsIconic(targetWnd)) return;
	if (g_Globals.EspConfig.Width <= 0 || g_Globals.EspConfig.Height <= 0) return;

	if (!g_Globals.EspConfig.Matrix) return;

	// Count detected enemies (only those that would be displayed by ESP)
	int enemyCount = 0;
	for (auto& [entityID, player] : g_Globals.EspConfig.Entities) {
		if (!player.IsDead && player.IsKnown &&
			!(g_Globals.AimBot.IgnoreBots && player.Name.find("BOT") != std::string::npos) &&
			player.Distance <= g_Globals.Visuals.DistanceEsp) {
			enemyCount++;
		}
	}

	// Draw enemy detection counter at top
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	ImVec2 screenSize = ImGui::GetIO().DisplaySize;

	// -- CLIP ESP TO EMULATOR AREA --
	drawList->PushClipRect(
		ImVec2(g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop),
		ImVec2(g_Globals.EspConfig.TargetLeft + g_Globals.EspConfig.Width, g_Globals.EspConfig.TargetTop + g_Globals.EspConfig.Height),
		true
	);

	// AdiCheats text at top center (moved down a bit)
	// AdiCheats text at top center of emulator area
	std::string adiCheatsText = "YxN CHEATS x86 INTERNAL";
	ImVec2 adiCheatsSize = ImGui::CalcTextSize(adiCheatsText.c_str());
	ImVec2 adiCheatsPos = ImVec2(g_Globals.EspConfig.TargetLeft + g_Globals.EspConfig.Width / 2 - adiCheatsSize.x / 2, g_Globals.EspConfig.TargetTop + 20);
	drawList->AddText(adiCheatsPos, IM_COL32(255, 255, 255, 255), adiCheatsText.c_str());

	// Enemy detection counter below AdiCheats text
	std::string enemyText = "Enemy Detected: " + std::to_string(enemyCount);
	ImVec2 enemySize = ImGui::CalcTextSize(enemyText.c_str());
	ImVec2 enemyPos = ImVec2(g_Globals.EspConfig.TargetLeft + g_Globals.EspConfig.Width / 2 - enemySize.x / 2, g_Globals.EspConfig.TargetTop + 35); // Centered below AdiCheats
	drawList->AddText(enemyPos, IM_COL32(255, 255, 255, 255), enemyText.c_str());

	for (auto& [entityID, player] : g_Globals.EspConfig.Entities) {

		if (player.IsDead || !player.IsKnown) {
			continue;
		}

		if (g_Globals.AimBot.IgnoreBots && player.Name.find("BOT") != std::string::npos) {
			continue;
		}

		float dist = player.Distance;
		if (dist > g_Globals.Visuals.DistanceEsp) {
			continue;
		}

		ImVec2 headPosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.Head, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Cabeça
		ImVec2 neckPosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.Neck, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Pescoço
		ImVec2 leftShoulderPosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.LeftShoulder, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Ombro Esquerdo
		ImVec2 rightShoulderPosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.RightShoulder, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Ombro Direito
		ImVec2 leftElbowPosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.LeftElbow, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Cotovelo Esquerdo
		ImVec2 rightElbowPosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.RightElbow, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Cotovelo Direito
		ImVec2 leftWristPosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.LeftWrist, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Pulso Esquerdo
		ImVec2 rightWristPosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.RightWrist, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Pulso Direito
		ImVec2 leftHandPosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.LeftHand, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Mão Esquerda
		ImVec2 rightHandPosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.RightHand, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Mão Direita

		ImVec2 hipPosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.Hip, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Quadril
		ImVec2 groinPosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.Groin, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Virilha

		ImVec2 leftAnklePosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.LeftAnkle, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Tornozelo Esquerdo
		ImVec2 rightAnklePosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.RightAnkle, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Tornozelo Direito
		ImVec2 leftFootPosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.LeftFoot, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Pé Esquerdo
		ImVec2 rightFootPosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.RightFoot, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Pé Direito

		ImVec2 rootPosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.Root, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Raiz do Corpo
		ImVec2 rootBonePosition = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, player.RootBone, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop); // Osso Raiz

		std::vector<ImVec2> screenPositions = {
			headPosition, neckPosition, leftShoulderPosition, rightShoulderPosition,
			leftElbowPosition, rightElbowPosition, leftWristPosition, rightWristPosition,
			leftHandPosition, rightHandPosition, hipPosition, groinPosition, leftAnklePosition,
			rightAnklePosition, leftFootPosition, rightFootPosition, rootPosition, rootBonePosition
		};

		bool invalidPosition = std::any_of(screenPositions.begin(), screenPositions.end(), [](const ImVec2& pos) {
			return pos.x < 1 || pos.y < 1;
			});

		if (invalidPosition) continue;

		float boxHeight = fabsf(headPosition.y - rootBonePosition.y);
		float boxWidth = boxHeight * 0.65f;

		if (g_Globals.Visuals.Lines) {
			ImColor snapLineColor;

			if (player.IsVisible) {
				snapLineColor = ImColor(g_Globals.Visuals.LinesColor[0],
					g_Globals.Visuals.LinesColor[1],
					g_Globals.Visuals.LinesColor[2],
					g_Globals.Visuals.LinesColor[3]);
			}
			else {
				snapLineColor = ImColor(0.6f, 0.0f, 1.0f, 1.0f);
			}

			ImColor lineColor = player.Pose == XPose::Knocked ? ImColor(1.f, 0.f, 0.f, 1.f) : snapLineColor;

			constexpr float topOffset = 50.0f;
			constexpr float bottomOffset = 50.0f;

			ImVec2 screenTop(g_Globals.EspConfig.TargetLeft + g_Globals.EspConfig.Width / 2.f, g_Globals.EspConfig.TargetTop + topOffset);
			ImVec2 screenBottom(g_Globals.EspConfig.TargetLeft + g_Globals.EspConfig.Width / 2.f,
				g_Globals.EspConfig.TargetTop + static_cast<float>(g_Globals.EspConfig.Height) - bottomOffset);

			float boxWidth = boxHeight * 0.65f;
			float boxX = headPosition.x - (boxWidth / 2.0f);
			float boxY = headPosition.y;
			float boxW = boxWidth;
			float boxH = boxHeight;

			float newWidth = boxW * 0.95f;
			float newHeight = boxH * 1.15f;
			float newX = boxX + (boxW - newWidth) / 2.0f;
			float newY = boxY - (newHeight - boxH);

			ImVec2 topConnectionPoint(newX + newWidth / 2.0f, newY);
			ImVec2 bottomConnectionPoint(newX + newWidth / 2.0f, newY + newHeight);

			switch (g_Globals.Visuals.EspLines) {
			case 1:
				ImGui::GetBackgroundDrawList()->AddLine(
					topConnectionPoint,
					screenTop,
					lineColor);
				break;
			case 2:
				ImGui::GetBackgroundDrawList()->AddLine(
					bottomConnectionPoint,
					screenBottom,
					lineColor);
				break;
			}
		}

		if (g_Globals.AimBot.Prevision) {
			Player* closestEnemy = nullptr;
			float closestDistance = FLT_MAX;

			for (auto& [entityID, player] : g_Globals.EspConfig.Entities) {
				if (player.IsDead || !player.IsKnown || player.IsTeam == Bool3::True) continue;

				if (player.Distance < closestDistance) {
					closestDistance = player.Distance;
					closestEnemy = &player;
				}
			}

			if (closestEnemy) {
				ImVec2 headPos = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, closestEnemy->Head, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height, g_Globals.EspConfig.TargetLeft, g_Globals.EspConfig.TargetTop);

				if (headPos.x > 0 && headPos.y > 0) {
					ImColor lineColor = ImColor(
						g_Globals.Visuals.LinesColor[0],
						g_Globals.Visuals.LinesColor[1],
						g_Globals.Visuals.LinesColor[2],
						g_Globals.Visuals.LinesColor[3]
					);

					ImVec2 centerScreen = ImVec2(g_Globals.EspConfig.TargetLeft + g_Globals.EspConfig.Width / 2.0f, g_Globals.EspConfig.TargetTop + g_Globals.EspConfig.Height / 2.0f);

					ImGui::GetBackgroundDrawList()->AddLine(
						headPos,
						centerScreen,
						lineColor,
						0.5f
					);
				}
			}
		}

		if (g_Globals.Visuals.HealthBar) {

			float scaledBoxHeight = boxHeight * 1.15f;
			float boxTopY = headPosition.y - 1.0f - (scaledBoxHeight - boxHeight);
			float boxLeftX = headPosition.x - (boxWidth / 2);
			float widerBoxWidth = boxWidth * 1.1f;

			switch (g_Globals.Visuals.players_healthbar) {
			case 0:
				break;
			case 1:
				DrawVerticalHealthBar(player.Health, 200, ImVec2(boxLeftX - 5, boxTopY), scaledBoxHeight);
				break;
			case 2:
				DrawVerticalHealthBar(player.Health, 200, ImVec2(boxLeftX + boxWidth + 5, boxTopY), scaledBoxHeight);
				break;
			case 3:
				DrawHealthBarBelowFullBox(
					player.Health,
					200,
					headPosition.x - (widerBoxWidth / 2),
					headPosition.y,
					widerBoxWidth,
					boxHeight
				);
				break;
			}
		}

		if (g_Globals.Visuals.Box) {

			ImColor boxColor = player.Pose == XPose::Knocked ? ImColor(1.f, 0.f, 0.f, 1.f) :
				ImColor(
					g_Globals.Visuals.BoxColor[0],
					g_Globals.Visuals.BoxColor[1],
					g_Globals.Visuals.BoxColor[2],
					g_Globals.Visuals.BoxColor[3]
				);

			float glowRadius = 12.0f;
			float feather = 2.0f;

			switch (g_Globals.Visuals.players_box) {
			case 0:
				break;
			case 1:
				DrawFullBox(headPosition.x - (boxWidth / 2), headPosition.y - 1.0f, boxWidth, boxHeight, boxColor, 0.5f);
				break;
			case 2:
				DrawGlowCorneredBox(headPosition.x - (boxWidth / 2), headPosition.y, boxWidth, boxHeight, boxColor, 0.5f, glowRadius, feather, false);
				break;
			}
		}

		if (g_Globals.Visuals.Name) {

			ImColor NameColor = player.Pose == XPose::Knocked ? ImColor(1.f, 0.f, 0.f, 1.f) :
				ImColor(
					g_Globals.Visuals.NameColor[0],
					g_Globals.Visuals.NameColor[1],
					g_Globals.Visuals.NameColor[2],
					g_Globals.Visuals.NameColor[3]
				);

			if (g_Globals.Visuals.RGB) {
				float time = ImGui::GetTime();
				float speedMultiplier = 2.0f;
				float hue = fmod(time * speedMultiplier, 1.0f);
				NameColor = ImColor::HSV(hue, 1.0f, 1.0f);
			}

			ImColor nombreColor = player.Pose == XPose::Knocked ? ImColor(1.f, 0.f, 0.f, 1.f) : NameColor;

			if (player.Name.empty()) {
				player.Name = "BOT";
			}

			ImVec2 textSize = ImGui::CalcTextSize(player.Name.c_str());
			auto drawList = ImGui::GetForegroundDrawList();
			ImColor outlineColor = ImColor(0, 0, 0, 180);
			float outlineSize = 0.8f;

			constexpr float namePosAbove = 50.0f;
			constexpr float namePosBelow = -35.0f;

			ImVec2 namePos(
				headPosition.x - (textSize.x / 2),
				g_Globals.Visuals.Distance
				? headPosition.y - namePosAbove
				: headPosition.y + namePosBelow
			);

			ImFont* nameFontToUse = FWork::Fonts::NotoSansRegular ? FWork::Fonts::NotoSansRegular :
				(FWork::Fonts::NotoSansSymbolsRegular ? FWork::Fonts::NotoSansSymbolsRegular :
					(FWork::Fonts::NotoEmojiRegular ? FWork::Fonts::NotoEmojiRegular :
						(FWork::Fonts::LexendRegular ? FWork::Fonts::LexendRegular :
							(FWork::Fonts::ArialUnicode ? FWork::Fonts::ArialUnicode : FWork::Fonts::InterRegular))));

			ImGui::PushFont(nameFontToUse);
			for (float xOffset = -outlineSize; xOffset <= outlineSize; xOffset += outlineSize)
				for (float yOffset = -outlineSize; yOffset <= outlineSize; yOffset += outlineSize)
					if (xOffset != 0.0f || yOffset != 0.0f)
						drawList->AddText(ImVec2(namePos.x + xOffset, namePos.y + yOffset), outlineColor, player.Name.c_str());

			drawList->AddText(namePos, NameColor, player.Name.c_str());
			ImGui::PopFont();
		}

		if (g_Globals.Visuals.Distance) {

			ImColor DistColor = ImColor(
				g_Globals.Visuals.DistColor[0],
				g_Globals.Visuals.DistColor[1],
				g_Globals.Visuals.DistColor[2],
				g_Globals.Visuals.DistColor[3]
			);

			if (g_Globals.Visuals.RGB) {
				float time = ImGui::GetTime();
				float speedMultiplier = 2.0f;
				float hue = fmod(time * speedMultiplier + 0.5f, 1.0f);
				DistColor = ImColor::HSV(hue, 1.0f, 1.0f);
			}

			ImColor distanciaColor = player.Pose == XPose::Knocked ? ImColor(1.f, 0.f, 0.f, 1.f) : DistColor;

			std::string distanceText = std::to_string(static_cast<int>(std::round(dist))) + "m";
			ImVec2 textSizeDistance = ImGui::CalcTextSize(distanceText.c_str());
			auto drawList = ImGui::GetForegroundDrawList();
			ImColor outlineColor = ImColor(0, 0, 0, 180);
			float outlineSize = 0.8f;

			constexpr float distOffset = -35.0f;
			ImVec2 distPos(headPosition.x - (textSizeDistance.x / 2), headPosition.y + distOffset);

			ImGui::PushFont(FWork::Fonts::InterBold);
			for (float xOffset = -outlineSize; xOffset <= outlineSize; xOffset += outlineSize)
				for (float yOffset = -outlineSize; yOffset <= outlineSize; yOffset += outlineSize)
					if (xOffset != 0.0f || yOffset != 0.0f)
						drawList->AddText(ImVec2(distPos.x + xOffset, distPos.y + yOffset), outlineColor, distanceText.c_str());

			drawList->AddText(distPos, DistColor, distanceText.c_str());
			ImGui::PopFont();
		}

		if (g_Globals.Visuals.Skeleton) {
			auto drawList = ImGui::GetForegroundDrawList();
			float baseCircleSize = 2.f;
			float distanceFactor = std::clamp(100.0f / dist, 0.5f, 1.5f);
			float circleSize = baseCircleSize * distanceFactor;

			ImColor skeletonColor = player.Pose == XPose::Knocked ? ImColor(1.f, 0.f, 0.f, 1.f) :
				ImColor(
					g_Globals.Visuals.SkeletonColor[0],
					g_Globals.Visuals.SkeletonColor[1],
					g_Globals.Visuals.SkeletonColor[2],
					g_Globals.Visuals.SkeletonColor[3]
				);

			auto DrawBone = [&](const ImVec2& from, const ImVec2& to) {
				drawList->AddLine(from, to, skeletonColor, g_Globals.Visuals.SkeletonThickness);
				};

			drawList->AddCircle(headPosition, circleSize, skeletonColor, 0, 1.f);
			DrawBone(headPosition, neckPosition);
			DrawBone(neckPosition, leftShoulderPosition);
			DrawBone(neckPosition, rightShoulderPosition);
			DrawBone(leftShoulderPosition, leftElbowPosition);
			DrawBone(rightShoulderPosition, rightElbowPosition);
			DrawBone(leftElbowPosition, leftWristPosition);
			DrawBone(rightElbowPosition, rightWristPosition);
			DrawBone(leftWristPosition, leftHandPosition);
			DrawBone(rightWristPosition, rightHandPosition);
			DrawBone(neckPosition, hipPosition);
			DrawBone(hipPosition, groinPosition);
			DrawBone(groinPosition, leftAnklePosition);
			DrawBone(groinPosition, rightAnklePosition);
			DrawBone(leftAnklePosition, leftFootPosition);
			DrawBone(rightAnklePosition, rightFootPosition);
		}

		if (g_Globals.Visuals.esparmas) {
			auto drawList = ImGui::GetForegroundDrawList();
			Namegun::Init();
			std::string gunName = Namegun::GetGunName(player.Gun);

			ImColor gunNameColor = player.Pose == XPose::Knocked ? ImColor(1.f, 0.f, 0.f, 1.f) :
				ImColor(
					g_Globals.Visuals.NameColor[0],
					g_Globals.Visuals.NameColor[1],
					g_Globals.Visuals.NameColor[2],
					g_Globals.Visuals.NameColor[3]
				);

			ImVec2 textSize = ImGui::CalcTextSize(gunName.c_str());

			ImVec2 textPos(rootPosition.x - (textSize.x / 2), rootPosition.y + 30); 

			auto outlineColor = ImColor(0, 0, 0, 180);
			float outlineSize = 1.0f;

			ImGui::PushFont(FWork::Fonts::InterBold);

			for (float xOffset = -outlineSize; xOffset <= outlineSize; xOffset += outlineSize)
				for (float yOffset = -outlineSize; yOffset <= outlineSize; yOffset += outlineSize)
					if (xOffset != 0.0f || yOffset != 0.0f)
						drawList->AddText(ImVec2(textPos.x + xOffset, textPos.y + yOffset), outlineColor, gunName.c_str());

			drawList->AddText(textPos, gunNameColor, gunName.c_str());

			ImGui::PopFont();
		}

		if (g_Globals.Visuals.showIcons) {
			ImColor GunColor = player.Pose == XPose::Knocked ? ImColor(1.f, 0.f, 0.f, 1.f) :
				(g_Globals.Visuals.RGB
					? ImColor::HSV(fmod(ImGui::GetTime() * 2.0f, 1.0f), 1.0f, 1.0f)
					: ImColor(
						g_Globals.Visuals.GunColor[0],
						g_Globals.Visuals.GunColor[1],
						g_Globals.Visuals.GunColor[2],
						g_Globals.Visuals.GunColor[3]
					));

			auto drawList = ImGui::GetForegroundDrawList();
			Namegun::Init();

			std::string TextAndIcon;
			bool isIcon = false;
			bool hasLevel = false;
			std::string suffix;
			std::string baseIcon;

			baseIcon = Namegun::GetGunIcon(player.Gun);

			std::string fullName = Namegun::GetGunName(player.Gun);
			std::string baseName = Namegun::GetBaseName(fullName);

			if (baseName != fullName) {
				suffix = fullName.substr(baseName.length());
				hasLevel = true;
			}

			if (hasLevel) {
				size_t iconSuffixPos = baseIcon.find(suffix);
				if (iconSuffixPos != std::string::npos) {
					baseIcon = baseIcon.substr(0, iconSuffixPos);
				}
			}

			TextAndIcon = baseIcon;
			isIcon = true;

			ImVec2 iconSize = ImGui::CalcTextSize(TextAndIcon.c_str());

			ImVec2 iconPos(rootPosition.x - iconSize.x * 2.7f, rootPosition.y + 25);

			ImFont* BestFont = isIcon ? FWork::Fonts::IconWeapon : FWork::Fonts::InterBold;
			ImGui::PushFont(BestFont);

			auto outlineColor = ImColor(0, 0, 0, 255);

			float outlineSize = 1.0f;

			for (float xOffset = -outlineSize; xOffset <= outlineSize; xOffset += outlineSize) {
				for (float yOffset = -outlineSize; yOffset <= outlineSize; yOffset += outlineSize) {
					if (xOffset != 0.0f || yOffset != 0.0f) {
						drawList->AddText(ImVec2(iconPos.x + xOffset, iconPos.y + yOffset), outlineColor, TextAndIcon.c_str());
					}
				}
			}

			ImU32 iconColor = GunColor;
			drawList->AddText(iconPos, iconColor, TextAndIcon.c_str());

			ImGui::PopFont();
		}
	}
	drawList->PopClipRect();
}