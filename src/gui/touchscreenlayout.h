/*
Copyright (C) 2024 grorp, Gregor Parzefall
		<gregor.parzefall@posteo.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#pragma once

#include "IGUIButton.h"
#include "IGUIImage.h"
#include "common/c_types.h"
#include "hud.h"
#include "irrlichttypes_extrabloated.h"
#include "modalMenu.h"

class ISimpleTextureSource;

enum TouchButton : u8 {
	BTN_RARE_CONTROLS_BAR,
	BTN_SETTINGS_BAR,
	BTN_JOYSTICK,
	BTN_JUMP,
	BTN_SNEAK,
	BTN_ZOOM,
	BTN_AUX1,
	TouchButton_END,
};

extern const EnumString es_TouchButton[];

struct button_meta {
	v2s32 pos;
	u32 height;
};

struct button_layout {
	std::unordered_map<TouchButton, button_meta> layout;
};

class GUITouchscreenLayout : public GUIModalMenu
{
public:
	GUITouchscreenLayout(gui::IGUIEnvironment* env,
            gui::IGUIElement* parent, s32 id,
            IMenuManager *menumgr, ISimpleTextureSource *tsrc);

	/*
		Remove and re-add (or reposition) stuff
	*/
	void regenerateGui(v2u32 screensize);

	void drawMenu();

	bool OnEvent(const SEvent& event);

	bool pausesGame() { return true; }

protected:
	std::wstring getLabelByID(s32 id) { return L""; }
	std::string getNameByID(s32 id) { return ""; }

private:
	ISimpleTextureSource *m_tsrc;

	TouchButton m_sel_btn = TouchButton_END;
	v2s32 m_last_mouse_pos;
	bool m_mouse_down = false;
	std::unordered_map<TouchButton, gui::IGUIImage *> m_gui_buttons;

	button_layout m_cur_layout;
};
