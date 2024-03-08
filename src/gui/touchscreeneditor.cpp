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

#include "touchscreeneditor.h"
#include "IEventReceiver.h"
#include "IGUIElement.h"
#include "ITexture.h"
#include "client/texturesource.h"
#include "debug.h"
#include "dimension2d.h"
#include "gui/touchscreenlayout.h"
#include "guiButton.h"
#include "guiScrollBar.h"
#include "irr_v2d.h"
#include "log.h"
#include "rect.h"
#include "serialization.h"
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <IGUICheckBox.h>
#include <IGUIButton.h>
#include <IGUIStaticText.h>
#include <IGUIFont.h>
#include <unordered_map>
#include <utility>
#include "settings.h"
#include "client/renderingengine.h"

#include "gettext.h"
#include "IGUIImage.h"
#include "touchscreengui.h"

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

void GUITouchscreenLayout::addButton(TouchButton btn, core::rect<s32> rect) {
	IGUIImage *irrimg;
	if (m_gui_buttons.count(btn) == 0) {
		irrimg = Environment->addImage(rect, this, ID_OFFSET + btn);
		m_gui_buttons[btn] = irrimg;
	} else {
		irrimg = m_gui_buttons.at(btn);
		// size must be updated by us, but position should stay as is
		core::rect<s32> pos = irrimg->getRelativePosition();
		pos.LowerRightCorner = pos.UpperLeftCorner + rect.getSize();
		irrimg->setRelativePosition(pos);
	};
	irrimg->setImage(ButtonLayout::getTexture(btn, m_tsrc));
	irrimg->setScaleImage(true);
}

void GUITouchscreenLayout::regenerateGui(v2u32 screensize)
{
	DesiredRect = core::rect<s32>(0, 0, screensize.X, screensize.Y);
	recalculateAbsolutePosition(false);

	m_last_render_layout = m_cur_layout;
	if (m_dragged_button.has_value()) {
		m_last_render_layout.add(m_dragged_button->first, m_dragged_button->second, m_tsrc, false);
	}

	for (u8 i = 0; i < TouchButton_END; i++) {
		TouchButton btn = (TouchButton)i;
		if (m_last_render_layout.shouldRender(btn, m_expanded_bar)) {
			core::rect<s32> rect = m_last_render_layout.getRect(btn, m_tsrc);
			addButton(btn, rect);
			m_tgt_pos[btn] = rect.UpperLeftCorner;
		} else {
			if (m_gui_buttons.count(btn) != 0) {
				m_gui_buttons.at(btn)->remove();
				m_gui_buttons.erase(btn);
			}
		}
	}

	const wchar_t *btn_text = L"Done";
	if (!m_gui_done_btn) {
		m_gui_done_btn = Environment->addButton(core::rect<s32>(), this, -1, btn_text);
	}
	s32 pad = RenderingEngine::getDisplayDensity() * 16;
	IGUIFont *font = m_gui_done_btn->getActiveFont();
	core::dimension2du dim = font->getDimension(btn_text);
	m_gui_done_btn->setRelativePosition(core::rect<s32>(
		screensize.X / 2 - dim.Width / 2 - pad,
		pad,
		screensize.X / 2 + dim.Width / 2 + pad,
		dim.Height + 3 * pad
	));

	m_sel_movement = v2s32();
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

	for (auto &v: m_gui_buttons) {
		v2f realp(v.second->getRelativePosition().UpperLeftCorner.X, v.second->getRelativePosition().UpperLeftCorner.Y);
		v2f tgt_p(m_tgt_pos.at(v.first).X, m_tgt_pos.at(v.first).Y);
		bool should_interp = m_last_render_layout.shouldInterpolate(v.first, m_dragged_button.has_value() ? std::make_optional(m_dragged_button->first) : std::nullopt);
		if (realp.getDistanceFrom(tgt_p) > 2.0f && should_interp) {
			v2f interpol = realp.getInterpolated(tgt_p, 0.667f);
			// round needed to make the interpolation work even if the steps get very small
			v.second->setRelativePosition(v2s32(core::round32(interpol.X), core::round32(interpol.Y)));
		} else {
			v.second->setRelativePosition(m_tgt_pos.at(v.first));
		}
	}

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

				if (m_cur_layout.layout.count(m_sel_btn) == 1 &&
						m_cur_layout.layout[m_sel_btn].bar.has_value()) {
					if (m_expanded_bar != m_sel_btn)
						m_expanded_bar = m_sel_btn;
					else
						m_expanded_bar = std::nullopt;
					regenerateGui(Environment->getVideoDriver()->getScreenSize());
				}
			} else {
				m_sel_btn = TouchButton_END;
			}
			return true;
		}
		case EMIE_MOUSE_MOVED: {
			if (m_mouse_down && m_sel_btn != TouchButton_END) {
				v2s32 mouse_pos = v2s32(event.MouseInput.X, event.MouseInput.Y);

				if (!m_dragged_button.has_value()) {
					core::rect<s32> sel_rect = m_cur_layout.getRect(m_sel_btn, m_tsrc);
					m_dragged_button = {m_sel_btn, {.pos = sel_rect.UpperLeftCorner, .height = (u32)sel_rect.getHeight()}};
					if (m_cur_layout.layout.count(m_sel_btn) == 1) { // is this a top-level button?
						m_dragged_button->second.bar = m_cur_layout.layout[m_sel_btn].bar;
					}
					m_cur_layout.remove(m_sel_btn);
				}
				m_dragged_button->second.pos += mouse_pos - m_last_mouse_pos;
				m_last_mouse_pos = mouse_pos;
				regenerateGui(Environment->getVideoDriver()->getScreenSize());
				return true;
			}
			break;
		}
		case EMIE_LMOUSE_LEFT_UP: {
			if (m_dragged_button.has_value()) {
				m_cur_layout.add(m_dragged_button->first, m_dragged_button->second, m_tsrc, true);
				m_dragged_button = std::nullopt;
				regenerateGui(Environment->getVideoDriver()->getScreenSize());
			}
			m_mouse_down = false;
			return true;
		}
		default:
			break;
		}
	} else if (event.EventType == EET_GUI_EVENT) {
		switch (event.GUIEvent.EventType) {
		case EGET_BUTTON_CLICKED: {
			if (event.GUIEvent.Caller == m_gui_done_btn) {
				g_touchscreengui->removeButtons();
				g_touchscreengui->createButtons(m_cur_layout);
				quitMenu();
				return true;
			}
			break;
		}
		case EGET_ELEMENT_FOCUS_LOST: {
			// Don't allow focusing the images, it breaks drag & drop with real mouse input
			if (this->isMyChild(event.GUIEvent.Element) && event.GUIEvent.Element->getType() == EGUIET_IMAGE) {
				// Returning true disables focus change
				return true;
			}
			if (!canTakeFocus(event.GUIEvent.Element))
				// Returning true disables focus change
				return true;
			break;
		}
		default:
			break;
		}
	}

	return Parent ? Parent->OnEvent(event) : false;
}
