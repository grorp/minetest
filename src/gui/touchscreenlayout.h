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
#include "touchscreengui.h"

class ISimpleTextureSource;

extern const EnumString es_TouchButton[];

class GUITouchscreenLayout : public GUIModalMenu
{
public:
	GUITouchscreenLayout(gui::IGUIEnvironment* env,
            gui::IGUIElement* parent, s32 id,
            IMenuManager *menumgr, ISimpleTextureSource *tsrc);
	~GUITouchscreenLayout();


	void addButton(TouchButton btn, core::rect<s32> rect);
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
	v2s32 m_sel_movement{};
	v2s32 m_last_mouse_pos;
	bool m_mouse_down = false;
	std::optional<std::pair<TouchButton, button_meta>> m_dragged_button;
	std::unordered_map<TouchButton, gui::IGUIImage *> m_gui_buttons;
	gui::IGUIButton *m_gui_done_btn = nullptr;

	button_layout m_cur_layout;
	std::optional<std::string> m_old_fps_max_unfocused;
	std::optional<TouchButton> m_expanded_bar;

	std::unordered_map<TouchButton, v2s32> m_tgt_pos;
};
