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

#include "touchscreenlayout.h"
#include "IGUIButton.h"
#include "IGUIImage.h"
#include "common/c_types.h"
#include "hud.h"
#include "irrlichttypes_extrabloated.h"
#include "modalMenu.h"

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
	std::optional<std::pair<TouchButton, ButtonMeta>> m_dragged_button;
	std::unordered_map<TouchButton, gui::IGUIImage *> m_gui_buttons;
	gui::IGUIButton *m_gui_done_btn = nullptr;

	ButtonLayout m_cur_layout;
	ButtonLayout m_last_render_layout;
	ButtonLayout m_layout_before_dragging; // put this into some "m_dragging_data" with m_dragged_button?
	std::optional<std::string> m_old_fps_max_unfocused;
	std::optional<TouchButton> m_expanded_bar;
	std::optional<core::rect<s32>> m_failure_rect;

	std::unordered_map<TouchButton, v2s32> m_tgt_pos;
};
