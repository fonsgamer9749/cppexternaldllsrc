#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imgui.h"
#include <string>
#include <vector>

namespace font11
{
    inline ImFont* icomoon = nullptr;
    inline ImFont* picomoon = nullptr;

    inline ImFont* primary_font = nullptr;
    inline ImFont* second_font = nullptr;
    inline ImFont* icon_font = nullptr;
    inline ImFont* tab_font = nullptr;

    inline ImFont* lexend_bold = nullptr;
    inline ImFont* lexend_regular = nullptr;
    inline ImFont* lexend_general_bold = nullptr;
    inline ImFont* lexend_general_bold2 = nullptr;

    inline ImFont* icomoon_widget = nullptr;
    inline ImFont* icomoon_widget2 = nullptr;
    inline ImFont* icon_font2 = nullptr;
    inline ImFont* Nevan = nullptr;
    inline ImFont* ContiB = nullptr;
    inline ImFont* ContiM = nullptr;
    inline ImColor main_color(168, 35, 155);


}

inline ImGuiWindow* main_window;
inline char search[64];
inline float anim_speed = 8.f;


namespace c11
{

    inline ImVec4 accent = ImColor(230, 25, 25);

    inline ImVec4 white = ImColor(255, 255, 255);

    namespace background
    {

        inline ImVec4 filling = ImColor(5, 5, 5);
        inline ImVec4 stroke = ImColor(24, 26, 36);
        inline ImVec2 size = ImVec2(860, 500);
        inline ImVec2 size2 = ImVec2(335, 425);

        inline float rounding = 6;

    }

    namespace elements
    {
        inline ImVec4 mark = ImColor(255, 255, 255);

        inline ImVec4 stroke = ImColor(28, 26, 37);
        inline ImVec4 background = ImColor(15, 15, 17);
        inline ImVec4 filled_circle = ImColor(25, 25, 25);
        inline ImVec4 circle = ImColor(35, 35, 35);
        inline ImVec4 background_widget = ImColor(21, 23, 26);


        inline ImVec4 text_active = ImColor(255, 255, 255);
        inline ImVec4 text_hov = ImColor(81, 92, 109);
        inline ImVec4 text = ImColor(255, 255, 255, 255 / 2);
        inline ImVec4 text1 = ImColor(200, 200, 200);

        inline float rounding = 4;
    }

    namespace child
    {

    }

    namespace tab
    {
        inline ImVec4 tab_active = ImColor(22, 22, 22);

        inline ImVec4 border = ImColor(14, 14, 14);
    }

}

inline ImColor text_color[2] = { ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 150) };

inline ImColor GetColorWithAlpha(ImColor color, float alpha)
{
    return ImColor(color.Value.x, color.Value.y, color.Value.z, alpha);
}

inline ImVec2 center_text(ImVec2 min, ImVec2 max, const char* text)
{
    return min + (max - min) / 2 - ImGui::CalcTextSize(text) / 2;
}

struct Notification
{
    std::string message, information;
    ImColor color;
    ImRect bb;
    ImVec2 offset = ImVec2(0, 400);
    ImVec2 target_offset = ImVec2(0, 400);
    float timer = 0;
    float shrink_progress = 1.0f;

    float y_position = 0;
    float target_y_position = 0;
};

class NotificationSystem
{
private:
    std::vector<Notification> notifications;
    ImVec2 start_position = ImVec2(475, 390); // Direct control of start position
    ImVec2 notification_size = ImVec2(270, 52);
    float spacing = 80.0f;

public:
    // Set the starting position for the first notification
    void SetStartPosition(const ImVec2& position) {
        start_position = position;
    }

    // Set notification size
    void SetNotificationSize(const ImVec2& size) {
        notification_size = size;
    }

    // Set spacing between notifications
    void SetSpacing(float new_spacing) {
        spacing = new_spacing;
    }

    void AddNotification(std::string message, std::string information, ImColor color)
    {
        Notification notif;
        notif.message = message;
        notif.information = information;
        notif.color = color;
        notif.timer = 0;
        notif.offset = ImVec2(notification_size.x, 0);
        notif.target_offset = ImVec2(notification_size.x, 0);
        notif.shrink_progress = 1.0f;
        notif.y_position = 0;
        notif.target_y_position = 0;
        notifications.push_back(notif);
    }

    void DrawNotifications()
    {
        if (notifications.empty()) return;

        float baseY = start_position.y;

        // Calculate Y positions for all notifications (stack downwards)
        for (int i = notifications.size() - 1; i >= 0; --i)
        {
            Notification& notif = notifications[i];
            notif.target_y_position = baseY;
            baseY += spacing;
        }

        for (int i = 0; i < notifications.size(); )
        {
            Notification& notif = notifications[i];
            notif.timer += ImGui::GetIO().DeltaTime * 10.f;

            notif.target_offset = notif.timer > 30.f ? ImVec2(notification_size.x, 0.f) : ImVec2(0.f, 0.f);
            notif.offset = ImLerp(notif.offset, notif.target_offset, ImGui::GetIO().DeltaTime * 10.f);

            notif.y_position = ImLerp(notif.y_position, notif.target_y_position, ImGui::GetIO().DeltaTime * 10.f);

            // Use the start X position
            notif.bb = ImRect(
                ImVec2(start_position.x + notif.offset.x, notif.y_position),
                ImVec2(start_position.x + notification_size.x + notif.offset.x, notif.y_position + notification_size.y)
            );

            ImGui::GetForegroundDrawList()->AddRectFilled(notif.bb.Min, notif.bb.Max, ImColor(10, 10, 10, 220), 2);

            ImGui::GetForegroundDrawList()->AddText(notif.bb.Min + ImVec2(12.5f, 6.f), ImGui::GetColorU32(c11::accent), notif.message.c_str());
            ImGui::PushFont(font11::lexend_regular);
            ImGui::GetForegroundDrawList()->AddText(notif.bb.Min + ImVec2(12.5f, 28.f), text_color[1], notif.information.c_str());
            ImGui::PopFont();

            notif.shrink_progress = fmax(notif.shrink_progress - ImGui::GetIO().DeltaTime * 0.35f, 0.0f);
            float shrink_width = notif.bb.GetWidth() * notif.shrink_progress;
            ImRect shrink_rect(notif.bb.Min + ImVec2(0.f, notification_size.y - 3.f), notif.bb.Min + ImVec2(shrink_width, notification_size.y));
            ImGui::GetForegroundDrawList()->AddRectFilled(shrink_rect.Min, shrink_rect.Max, ImGui::GetColorU32(c11::accent));
            ImGui::GetForegroundDrawList()->AddShadowRect(shrink_rect.Min, shrink_rect.Max, ImGui::GetColorU32(c11::accent), 30.f, ImVec2(0, 0), 0, 0.f);

            if (notif.timer > 35.f && notif.offset.x >= (notification_size.x - 1.f))
            {
                notifications.erase(notifications.begin() + i);
                continue;
            }

            ++i;
        }
    }
};

inline NotificationSystem notificationSystem;

