#include "quakedef.hpp"
#include "vr.hpp"
#include "vrgameplay_menu.hpp"
#include "cmd.hpp"
#include "util.hpp"

#include <string>
#include <cassert>
#include <array>

static int vrgameplay_options_cursor = 0;

extern void M_DrawSlider(int x, int y, float range);

static void VRGameplay_MenuPlaySound(const char* sound, float fvol)
{
    if(sfx_t* sfx = S_PrecacheSound(sound))
    {
        S_StartSound(cl.viewentity, 0, sfx, vec3_origin, fvol, 1);
    }
}

static void VRGameplay_MenuPrintOptionValue(
    int cx, int cy, VRGameplayMenuOpt option)
{
    char value_buffer[32] = {0};
    const char* value_string = nullptr;

    const auto printAsStr = [&](const auto& cvar) {
        snprintf(value_buffer, sizeof(value_buffer), "%.4f", cvar.value);
        M_Print(cx, cy, value_buffer);
    };

    switch(option)
    {
        case VRGameplayMenuOpt::e_MeleeThreshold:
            printAsStr(vr_melee_threshold);
            break;
        case VRGameplayMenuOpt::e_RoomscaleJump:
            M_Print(cx, cy, vr_roomscale_jump.value == 0 ? "Off" : "On");
            break;
        case VRGameplayMenuOpt::e_RoomscaleJumpThreshold:
            printAsStr(vr_roomscale_jump_threshold);
            break;
        case VRGameplayMenuOpt::e_CalibrateHeight:
            printAsStr(vr_height_calibration);
            break;
        case VRGameplayMenuOpt::e_MeleeDmgMultiplier:
            printAsStr(vr_melee_dmg_multiplier);
            break;
        case VRGameplayMenuOpt::e_MeleeRangeMultiplier:
            printAsStr(vr_melee_range_multiplier);
            break;
        case VRGameplayMenuOpt::e_BodyInteractions:
            M_Print(cx, cy, vr_body_interactions.value == 0 ? "Off" : "On");
            break;
        case VRGameplayMenuOpt::e_ForwardSpeed:
            printAsStr(cl_forwardspeed);
            break;
        case VRGameplayMenuOpt::e_SpeedBtnMultiplier:
            printAsStr(cl_movespeedkey);
            break;
        default: assert(false); break;
    }

    if(value_string)
    {
        M_Print(cx, cy, value_string);
    }
}

static void M_VRGameplay_KeyOption(int key, VRGameplayMenuOpt option)
{
    const bool isLeft = (key == K_LEFTARROW);

    const auto adjustF = [&isLeft](const cvar_t& cvar, auto incr, auto min,
                             auto max) {
        Cvar_SetValue(cvar.name,
            CLAMP(min, isLeft ? cvar.value - incr : cvar.value + incr, max));
    };

    const auto adjustI = [&isLeft](const cvar_t& cvar, auto incr, auto min,
                             auto max) {
        Cvar_SetValue(cvar.name,
            (int)CLAMP(
                min, isLeft ? cvar.value - incr : cvar.value + incr, max));
    };

    switch(option)
    {
        case VRGameplayMenuOpt::e_MeleeThreshold:
            adjustF(vr_melee_threshold, 0.5f, 4.f, 18.f);
            break;
        case VRGameplayMenuOpt::e_RoomscaleJump:
            adjustI(vr_roomscale_jump, 1, 0, 1);
            break;
        case VRGameplayMenuOpt::e_RoomscaleJumpThreshold:
            adjustF(vr_roomscale_jump_threshold, 0.05f, 0.05f, 3.f);
            break;
        case VRGameplayMenuOpt::e_CalibrateHeight: VR_CalibrateHeight(); break;
        case VRGameplayMenuOpt::e_MeleeDmgMultiplier:
            adjustF(vr_melee_dmg_multiplier, 0.25f, 0.25f, 15.f);
            break;
        case VRGameplayMenuOpt::e_MeleeRangeMultiplier:
            adjustF(vr_melee_range_multiplier, 0.25f, 0.25f, 15.f);
            break;
        case VRGameplayMenuOpt::e_BodyInteractions:
            adjustI(vr_body_interactions, 1, 0, 1);
            break;
        case VRGameplayMenuOpt::e_ForwardSpeed:
            adjustI(cl_forwardspeed, 25, 100, 400);
            break;
        case VRGameplayMenuOpt::e_SpeedBtnMultiplier:
            adjustF(cl_movespeedkey, 0.05f, 0.1f, 1.f);
            break;
        default: assert(false); break;
    }
}

void M_VRGameplay_Key(int key)
{
    switch(key)
    {
        case K_ESCAPE:
            VID_SyncCvars(); // sync cvars before leaving menu. FIXME: there are
                             // other ways to leave menu
            S_LocalSound("misc/menu1.wav");
            M_Menu_Options_f();
            break;

        case K_UPARROW:
            S_LocalSound("misc/menu1.wav");
            vrgameplay_options_cursor--;
            if(vrgameplay_options_cursor < 0)
            {
                vrgameplay_options_cursor = (int)VRGameplayMenuOpt::k_Max - 1;
            }
            break;

        case K_DOWNARROW:
            S_LocalSound("misc/menu1.wav");
            vrgameplay_options_cursor++;
            if(vrgameplay_options_cursor >= (int)VRGameplayMenuOpt::k_Max)
            {
                vrgameplay_options_cursor = 0;
            }
            break;

        case K_LEFTARROW: [[fallthrough]];
        case K_RIGHTARROW:
            S_LocalSound("misc/menu3.wav");
            M_VRGameplay_KeyOption(
                key, (VRGameplayMenuOpt)vrgameplay_options_cursor);
            break;

        case K_ENTER:
            m_entersound = true;
            M_VRGameplay_KeyOption(
                key, (VRGameplayMenuOpt)vrgameplay_options_cursor);
            break;

        default: break;
    }
}

void M_VRGameplay_Draw()
{
    int y = 4;

    // plaque
    M_DrawTransPic(16, y, Draw_CachePic("gfx/qplaque.lmp"));

    // customize header
    qpic_t* p = Draw_CachePic("gfx/ttl_cstm.lmp");
    M_DrawPic((320 - p->width) / 2, y, p);

    y += 28;

    // title
    const char* title = "VR GAMEPLAY OPTIONS";
    M_PrintWhite((320 - 8 * strlen(title)) / 2, y, title);

    y += 16;
    int idx = 0;

    static const auto adjustedLabels = quake::util::makeAdjustedMenuLabels(
        "Melee Threshold", "Roomscale Jump", "Roomscale Jump Threshold",
        "Calibrate Height", "Melee Damage Multiplier", "Melee Range Multiplier",
        "Body-Item Interactions", "Movement Speed", "Speed Button Multiplier");

    static_assert(adjustedLabels.size() == (int)VRGameplayMenuOpt::k_Max);

    for(const std::string& label : adjustedLabels)
    {
        M_Print(16, y, label.data());
        VRGameplay_MenuPrintOptionValue(240, y, (VRGameplayMenuOpt)idx);

        // draw the blinking cursor
        if(vrgameplay_options_cursor == idx)
        {
            M_DrawCharacter(220, y, 12 + ((int)(realtime * 4) & 1));
        }

        ++idx;
        y += 8;
    }
}

void M_Menu_VRGameplay_f()
{
    const char* sound = "items/r_item1.wav";

    IN_Deactivate(modestate == MS_WINDOWED);
    key_dest = key_menu;
    m_state = m_vrgameplay;
    m_entersound = true;

    VRGameplay_MenuPlaySound(sound, 0.5);
}
