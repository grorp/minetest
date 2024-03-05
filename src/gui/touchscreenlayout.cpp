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

#include "touchscreenlayout.h"
#include "IEventReceiver.h"
#include "IGUIElement.h"
#include "ITexture.h"
#include "client/texturesource.h"
#include "debug.h"
#include "dimension2d.h"
#include "guiButton.h"
#include "guiScrollBar.h"
#include "irr_v2d.h"
#include "log.h"
#include "rect.h"
#include "serialization.h"
#include <optional>
#include <string>
#include <IGUICheckBox.h>
#include <IGUIButton.h>
#include <IGUIStaticText.h>
#include <IGUIFont.h>
#include <unordered_map>
#include "settings.h"
#include "client/renderingengine.h"
#include "touchscreengui.h"

#include "gettext.h"
#include "IGUIImage.h"

constexpr s32 ID_OFFSET = 737;

const struct EnumString es_TouchButton[] =
{
	{BTN_RARE_CONTROLS_BAR, "rare_controls_bar"},
	{BTN_SETTINGS_BAR, "settings_bar"},
	{BTN_JOYSTICK, "joystick"},
	{BTN_JUMP, "jump"},
	{BTN_SNEAK, "sneak"},
	{BTN_ZOOM, "zoom"},
	{BTN_AUX1, "aux1"},
	{0, NULL},
};

const std::string touch_button_images[] =
{
	"rare_controls.png",
	"gear_icon.png",
	"joystick_off.png",
	"jump_btn.png",
	"down.png",
	"zoom.png",
	"aux1_btn.png",

	"fly_btn.png",
	"noclip_btn.png",
	"fast_btn.png",
	"debug_btn.png",
	"camera_btn.png",
	"rangeview_btn.png",
	"minimap_btn.png",
	"chat_hide_btn.png",

	"chat_btn.png",
	"inventory_btn.png",
	"drop_btn.png",
	"exit_btn.png",
};

button_layout get_default_layout(v2u32 screensize) {
	u32 button_size = MYMIN(screensize.Y / 4.5f,
			RenderingEngine::getDisplayDensity() * 65.0f *
					g_settings->getFloat("hud_scaling"));
	button_layout l = {{
		{BTN_RARE_CONTROLS_BAR, {
			.pos = v2s32(0.25f * button_size, screensize.Y - (RARE_CONTROLS_BAR_Y_OFFSET + 1.0f) * button_size + 0.5f * button_size),
			.height = button_size,
		}},
		{BTN_SETTINGS_BAR, {
			.pos = v2s32(screensize.X - 1.25f * button_size, screensize.Y - (SETTINGS_BAR_Y_OFFSET + 1.0f) * button_size + 0.5f * button_size),
			.height = button_size,
		}},
		{BTN_JOYSTICK, {
			.pos = v2s32(button_size, screensize.Y - button_size * 4),
			.height = button_size * 3,
		}},
		{BTN_JUMP, {
			.pos = v2s32(screensize.X - 1.75f * button_size, screensize.Y - button_size),
			.height = button_size,
		}},
		{BTN_SNEAK, {
			.pos = v2s32(screensize.X - 3.25f * button_size, screensize.Y - button_size),
			.height = button_size,
		}},
		{BTN_ZOOM, {
			.pos = v2s32(screensize.X - 1.25f * button_size, screensize.Y - 4 * button_size),
			.height = button_size,
		}},
		{BTN_AUX1, {
			.pos = v2s32(screensize.X - 1.25f * button_size, screensize.Y - 2.5f * button_size),
			.height = button_size,
		}},
	}};

	l.layout[BTN_SETTINGS_BAR].bar.emplace();
	l.layout[BTN_SETTINGS_BAR].bar->dir = BarDir::Left;
	l.layout[BTN_SETTINGS_BAR].bar->content = {{BTN_FLY, {}}, {BTN_NOCLIP, {}}, {BTN_FAST, {}}, {BTN_DEBUG, {}}, {BTN_CAMERA_MODE, {}}, {BTN_RANGESELET, {}}, {BTN_MINIMAP, {}}, {BTN_TOGGLE_CHAT, {}}};

	l.layout[BTN_RARE_CONTROLS_BAR].bar.emplace();
	l.layout[BTN_RARE_CONTROLS_BAR].bar->dir = BarDir::Right;
	l.layout[BTN_RARE_CONTROLS_BAR].bar->content = {{BTN_CHAT, {}}, {BTN_INVENTORY, {}}, {BTN_DROP, {}}, {BTN_EXIT, {}}};

	return l;
}

core::rect<s32> button_layout::getRect(TouchButton btn, ISimpleTextureSource *tsrc) const
{

	video::ITexture *tex = tsrc->getTexture(touch_button_images[btn]);
	dimension2du orig_size = tex->getOriginalSize();
	button_meta meta = layout.at(btn);

	return core::rect<s32>(
		meta.pos.X,
		meta.pos.Y,
		meta.pos.X + (f32)orig_size.Width / (f32)orig_size.Height * meta.height,
		meta.pos.Y + meta.height);

}

GUITouchscreenLayout::GUITouchscreenLayout(gui::IGUIEnvironment* env,
		gui::IGUIElement* parent, s32 id,
		IMenuManager *menumgr, ISimpleTextureSource *tsrc
):
	GUIModalMenu(env, parent, id, menumgr),
	m_tsrc(tsrc)
{
	m_cur_layout = g_touchscreengui->m_layout;

	// We need all the FPS we can get if we want drag & drop to be smooth.
	if (g_settings->existsLocal("fps_max_unfocused"))
		m_old_fps_max_unfocused = g_settings->get("fps_max_unfocused");
	else
		m_old_fps_max_unfocused = std::nullopt;
	g_settings->set("fps_max_unfocused", g_settings->get("fps_max"));
}

GUITouchscreenLayout::~GUITouchscreenLayout() {
	if (m_old_fps_max_unfocused.has_value())
		g_settings->set("fps_max_unfocused", *m_old_fps_max_unfocused);
	else
		g_settings->remove("fps_max_unfocused");

}

void GUITouchscreenLayout::regenerateGui(v2u32 screensize)
{
	removeAllChildren();
	DesiredRect = core::rect<s32>(0, 0, screensize.X, screensize.Y);
	recalculateAbsolutePosition(false);

	for (u8 i = 0; i < TouchButton_END; i++) {
		TouchButton btn = (TouchButton)i;
		if (m_cur_layout.layout.count(btn) != 1)
			continue;

		IGUIImage *irrimg = Environment->addImage(m_cur_layout.getRect(btn, m_tsrc), this, ID_OFFSET + btn);
		video::ITexture *tex = m_tsrc->getTexture(touch_button_images[btn]);
		irrimg->setImage(tex);
		irrimg->setScaleImage(true);
		m_gui_buttons[btn] = irrimg;
	}

	const wchar_t *btn_text = L"Done";
	s32 pad = RenderingEngine::getDisplayDensity() * 16;
	m_gui_done_btn = Environment->addButton(core::rect<s32>(), this, -1, btn_text);
	IGUIFont *font = m_gui_done_btn->getActiveFont();
	dimension2du dim = font->getDimension(btn_text);
	m_gui_done_btn->setRelativePosition(core::rect<s32>(
		screensize.X / 2 - dim.Width / 2 - pad,
		pad,
		screensize.X / 2 + dim.Width / 2 + pad,
		dim.Height + 3 * pad
	));
}

void GUITouchscreenLayout::drawMenu()
{
	gui::IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;

	video::IVideoDriver* driver = Environment->getVideoDriver();
	video::SColor bgcolor(200, 0, 0, 0);
	video::SColor highlight(255, 128, 128, 128);

	driver->draw2DRectangle(bgcolor, AbsoluteRect, &AbsoluteClippingRect);

	bool valid_selection = m_sel_btn != TouchButton_END && m_gui_buttons.count(m_sel_btn) == 1;
	if (valid_selection) {
		driver->draw2DRectangle(highlight, m_gui_buttons[m_sel_btn]->getAbsolutePosition(), &AbsoluteClippingRect);
	}
	m_gui_done_btn->setVisible(!valid_selection);
	
	gui::IGUIElement::draw();
}

bool GUITouchscreenLayout::OnEvent(const SEvent& event)
{
	if (event.EventType == EET_KEY_INPUT_EVENT) {
		if ((event.KeyInput.Key == KEY_ESCAPE || event.KeyInput.Key == KEY_CANCEL) && event.KeyInput.PressedDown) {
			quitMenu();
			return true;
		}
	} else if (event.EventType == EET_MOUSE_INPUT_EVENT) {
		switch (event.MouseInput.Event) {
		case EMIE_LMOUSE_PRESSED_DOWN: {
			v2s32 mouse_pos = v2s32(event.MouseInput.X, event.MouseInput.Y);
			IGUIElement *el = Environment->getRootGUIElement()->getElementFromPoint(mouse_pos);
			s32 id = el->getID();
			if (id >= ID_OFFSET && id < ID_OFFSET + TouchButton_END) {
				m_sel_btn = (TouchButton)(id - ID_OFFSET);
				m_last_mouse_pos = mouse_pos;
				m_mouse_down = true;
			} else {
				m_sel_btn = TouchButton_END;
			}
			return true;
		}
		case EMIE_MOUSE_MOVED: {
			if (m_mouse_down && m_sel_btn != TouchButton_END) {
				v2s32 mouse_pos = v2s32(event.MouseInput.X, event.MouseInput.Y);
				m_cur_layout.layout[m_sel_btn].pos += mouse_pos - m_last_mouse_pos;
				m_last_mouse_pos = mouse_pos;
				regenerateGui(Environment->getVideoDriver()->getScreenSize());
				return true;
			}
			break;
		}
		case EMIE_LMOUSE_LEFT_UP: {
			m_mouse_down = false;
			return true;
		}
		default:
			break;
		}
	} else if (event.EventType == EET_GUI_EVENT) {
		if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED) {
			if (event.GUIEvent.Caller == m_gui_done_btn) {
				g_touchscreengui->removeButtons();
				g_touchscreengui->createButtons(m_cur_layout);
				quitMenu();
				return true;
			}
		}
	}

	return Parent ? Parent->OnEvent(event) : false;
}
