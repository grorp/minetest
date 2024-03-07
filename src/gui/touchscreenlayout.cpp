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

	"",
	"placeholder_btn.png",
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

static core::rect<s32> resize_for_different_button(BarDir dir, core::rect<s32> rect, v2u32 orig_size) {
	switch (dir) {
	case BarDir::Left:
	case BarDir::Right: {
		rect.LowerRightCorner.X = rect.UpperLeftCorner.X +
				(f32)orig_size.X / (f32)orig_size.Y * rect.getHeight();
		break;
	}
	case BarDir::Down:
	case BarDir::Up:
		rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y +
				(f32)orig_size.Y / (f32)orig_size.X * rect.getWidth();
		break;
	}
	return rect;
}

static core::rect<s32> apply_offset(BarDir dir, core::rect<s32> rect) {
	s32 offset;
	switch (dir) {
	case BarDir::Left:
	case BarDir::Right:
		offset = rect.getWidth();
		break;
	case BarDir::Down:
	case BarDir::Up:
		offset = rect.getHeight();
		break;
	}

	switch (dir) {
	case BarDir::Left:
		rect.UpperLeftCorner.X  -= offset;
		rect.LowerRightCorner.X -= offset;
		break;
	case BarDir::Right:
		rect.UpperLeftCorner.X  += offset;
		rect.LowerRightCorner.X += offset;
		break;
	case BarDir::Down:
		rect.UpperLeftCorner.Y  += offset;
		rect.LowerRightCorner.Y += offset;
		break;
	case BarDir::Up:
		rect.UpperLeftCorner.Y  -= offset;
		rect.LowerRightCorner.Y -= offset;
		break;
	}
	return rect;
}

v2u32 button_layout::getOrigSize(TouchButton btn, ISimpleTextureSource *tsrc) const
{
	video::ITexture *tex = tsrc->getTexture(touch_button_images[btn]);
	return tex->getOriginalSize();
}

static core::rect<s32> get_rect_simple_meta(TouchButton btn, const button_meta &btn_meta, ISimpleTextureSource *tsrc)
{
	video::ITexture *tex = tsrc->getTexture(touch_button_images[btn]);
	v2u32 orig_size = tex->getOriginalSize();

	return core::rect<s32>(
		btn_meta.pos.X,
		btn_meta.pos.Y,
		btn_meta.pos.X + (f32)orig_size.X / (f32)orig_size.Y * btn_meta.height,
		btn_meta.pos.Y + btn_meta.height);
}

core::rect<s32> button_layout::getRectSimple(TouchButton btn, ISimpleTextureSource *tsrc) const
{
	button_meta meta = layout.at(btn);
	return get_rect_simple_meta(btn, meta, tsrc);
}

core::rect<s32> button_layout::getRect(TouchButton btn, ISimpleTextureSource *tsrc) const
{
	if (layout.count(btn) == 1)
		return getRectSimple(btn, tsrc);

	for (auto &v : layout) {
		TouchButton outer_btn = v.first;
		const button_meta &outer_meta = v.second;

		if (outer_meta.bar.has_value()) {
			core::rect<s32> rect = getRectSimple(outer_btn, tsrc);
			const button_meta::bar_props &bar = *outer_meta.bar;

			if (bar.dir == BarDir::Right || bar.dir == BarDir::Down)
				rect = apply_offset(bar.dir, rect);

			for (auto &w : bar.content) {
				TouchButton inner_btn = w.first;

				v2u32 orig_size = getOrigSize(inner_btn, tsrc);
				rect = resize_for_different_button(bar.dir, rect, orig_size);
				if (bar.dir == BarDir::Left || bar.dir == BarDir::Up)
					rect = apply_offset(bar.dir, rect);

				if (inner_btn == btn)
					return rect;

				if (bar.dir == BarDir::Right || bar.dir == BarDir::Down)
					rect = apply_offset(bar.dir, rect);
			}
		}
	}

	throw std::out_of_range("button doesn't exist in layout");
}

static bool button_render_please(const button_layout *layout, TouchButton btn, std::optional<TouchButton> expanded_bar) {
	if (layout->layout.count(btn) == 1)
		return true;

	for (auto &v : layout->layout) {
		if (v.first == expanded_bar && v.second.bar.has_value()) {
			for (auto &w : v.second.bar->content) {
				if (w.first == btn)
					return true;
			}
		}
	}
	
	return false;
}

static void button_remove(button_layout *layout, TouchButton btn) {
	if (layout->layout.count(btn) == 1) {
		layout->layout.erase(btn);
		return;
	}

	for (auto &v : layout->layout) {
		if (v.second.bar.has_value()) {
			button_meta::bar_props &bar = *v.second.bar;
			std::vector<std::pair<TouchButton, bar_button_meta>> &content = bar.content;
			auto it = std::find_if(content.begin(), content.end(), 
                [&](std::pair<TouchButton, bar_button_meta> val) { return val.first == btn; });
			if (it != content.end()) {
				content.erase(it);
				return;
			}
		}
	}

	throw std::out_of_range("button doesn't exist in layout");
}

static void button_drop(button_layout *layout, TouchButton btn, button_meta meta, ISimpleTextureSource *tsrc, bool for_real) {
	errorstream << "[button_drop] called for " << touch_button_images[btn] << std::endl;
	core::rect<s32> our_rect = get_rect_simple_meta(btn, meta, tsrc);
	v2f32 our_center = core::rect<f32>(our_rect.UpperLeftCorner.X, our_rect.UpperLeftCorner.Y,
			our_rect.LowerRightCorner.X, our_rect.LowerRightCorner.Y).getCenter();

	for (auto &v : layout->layout) {
		if (v.second.bar.has_value()) {
			button_meta::bar_props &bar = *v.second.bar;
			
			core::rect<s32> full_rect = core::rect<s32>();
			TouchButton closest_button_in_bar = TouchButton_END;
			size_t closest_index = 0;
			f32 closest_distance_sq = std::numeric_limits<f32>::max();

			size_t i = 0;
			for (auto &w : bar.content) {
				TouchButton inner_btn = w.first;
				core::rect<s32> rect = layout->getRect(inner_btn, tsrc);
				if (full_rect.getArea() == 0) {
					full_rect = rect;
				} else {
					full_rect.addInternalPoint(rect.UpperLeftCorner);
					full_rect.addInternalPoint(rect.LowerRightCorner);
				}
				v2f32 inner_center = core::rect<f32>(rect.UpperLeftCorner.X, rect.UpperLeftCorner.Y,
						rect.LowerRightCorner.X, rect.LowerRightCorner.Y).getCenter();
				f32 distance_sq = our_center.getDistanceFromSQ(inner_center);
				if (distance_sq < closest_distance_sq) {
					closest_button_in_bar = inner_btn;
					closest_index = i;
					closest_distance_sq = distance_sq;
				}
				i++;
			}

			if (full_rect.isPointInside(our_rect.getCenter())) {
				errorstream << "button is contained in bar launched by " << touch_button_images[v.first] << std::endl;
				errorstream << "button is closest to " << touch_button_images[closest_button_in_bar] << std::endl;
				errorstream << "inserting at index " << closest_index << std::endl;
				if (for_real) {
					bar.content.insert(bar.content.begin() + closest_index, {btn, {}});
				} else {
					// FIXME: Placeholder has wrong aspect ratio!!!
					bar.content.insert(bar.content.begin() + closest_index, {BTN_PLACEHOLDER, {}});
					layout->layout[btn] = meta;
				}
				errorstream << "[BAR CONTENT AFTER INSERTION]" << std::endl;
				for (auto &lol : bar.content) {
					errorstream << "    - " << touch_button_images[lol.first] << std::endl;
				}
				return; // dropped
			}
		}
	}

	errorstream << "button not contained in bar, adding normally" << std::endl;
	layout->layout[btn] = meta;
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
	// g_settings->set("fps_max_unfocused", g_settings->get("fps_max"));
}

GUITouchscreenLayout::~GUITouchscreenLayout() {
	if (m_old_fps_max_unfocused.has_value())
		g_settings->set("fps_max_unfocused", *m_old_fps_max_unfocused);
	else
		g_settings->remove("fps_max_unfocused");

}

void GUITouchscreenLayout::addButton(TouchButton btn, core::rect<s32> rect) {
	if (m_gui_buttons.count(btn) == 0) {
		IGUIImage *irrimg = Environment->addImage(rect, this, ID_OFFSET + btn);
		video::ITexture *tex = m_tsrc->getTexture(touch_button_images[btn]);
		irrimg->setImage(tex);
		irrimg->setScaleImage(true);
		m_gui_buttons[btn] = irrimg;
	}
}

void GUITouchscreenLayout::regenerateGui(v2u32 screensize)
{
	DesiredRect = core::rect<s32>(0, 0, screensize.X, screensize.Y);
	recalculateAbsolutePosition(false);

	button_layout clone = m_cur_layout;
	if (m_dragged_button.has_value()) {
		button_drop(&clone, m_dragged_button->first, m_dragged_button->second, m_tsrc, false);
	}

	for (u8 i = 0; i < BTN_PLACEHOLDER + 1; i++) {
		TouchButton btn = (TouchButton)i;
		if (btn == TouchButton_END)
			continue;
		if (button_render_please(&clone, btn, m_expanded_bar)) {
			core::rect<s32> rect = clone.getRect(btn, m_tsrc);
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
	dimension2du dim = font->getDimension(btn_text);
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
		errorstream << "moving" << std::endl;

		v2f realp(v.second->getRelativePosition().UpperLeftCorner.X, v.second->getRelativePosition().UpperLeftCorner.Y);
		v2f tgt_p(m_tgt_pos.at(v.first).X, m_tgt_pos.at(v.first).Y);
		if (realp.getDistanceFrom(tgt_p) > 2.0f) {
			errorstream << "distance > 2.0f" << std::endl;
			v2f interpol = realp.getInterpolated(tgt_p, 0.9f);
			v.second->setRelativePosition(v2s32(interpol.X, interpol.Y));
		} else {
			errorstream << "distance <= 2.0f" << std::endl;
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
					errorstream << "starting to drag" << std::endl;
					core::rect<s32> sel_rect = m_cur_layout.getRect(m_sel_btn, m_tsrc);
					m_dragged_button = {m_sel_btn, {.pos = sel_rect.UpperLeftCorner, .height = (u32)sel_rect.getHeight()}};
					if (m_cur_layout.layout.count(m_sel_btn) == 1) { // is this a top-level button?
						m_dragged_button->second.bar = m_cur_layout.layout[m_sel_btn].bar;
					}
					button_remove(&m_cur_layout, m_sel_btn);
				} else {
					errorstream << "continuing to drag" << std::endl;
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
				errorstream << "DROPPING BUTTON" << std::endl;
				button_drop(&m_cur_layout, m_dragged_button->first, m_dragged_button->second, m_tsrc, true);
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
