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
#include "ITexture.h"
#include "debug.h"
#include "dimension2d.h"
#include "guiButton.h"
#include "guiScrollBar.h"
#include "serialization.h"
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
	"aux1_btn.png"
};

struct button_meta {
	v2s32 pos;
	u32 height;
};

struct button_layout {
	std::unordered_map<TouchButton, button_meta> layout;
};

button_layout get_default_layout(v2u32 screensize) {
	u32 button_size = MYMIN(screensize.Y / 4.5f,
			RenderingEngine::getDisplayDensity() * 65.0f *
					g_settings->getFloat("hud_scaling"));
	return {{
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
}

GUITouchscreenLayout::GUITouchscreenLayout(gui::IGUIEnvironment* env,
		gui::IGUIElement* parent, s32 id,
		IMenuManager *menumgr, ISimpleTextureSource *tsrc
):
	GUIModalMenu(env, parent, id, menumgr),
	m_tsrc(tsrc)
{
}

void GUITouchscreenLayout::regenerateGui(v2u32 screensize)
{
	removeAllChildren();
	DesiredRect = core::rect<s32>(0, 0, screensize.X, screensize.Y);
	recalculateAbsolutePosition(false);

	button_layout layout = get_default_layout(screensize);
	for (u8 i = 0; i < TouchButton_END; i++) {
		TouchButton btn = (TouchButton)i;
		if (layout.layout.count(btn) != 1)
			continue;

		button_meta meta = layout.layout.at(btn);
		video::ITexture *tex = m_tsrc->getTexture(touch_button_images[btn]);
		dimension2du orig_size = tex->getOriginalSize();

		core::rect<s32> rect(
			meta.pos.X,
			meta.pos.Y,
			meta.pos.X + (f32)orig_size.Width / (f32)orig_size.Height * meta.height,
			meta.pos.Y + meta.height);
		IGUIButton *irrbtn = Environment->addButton(rect, this, 77 + btn);
		load_button_texture(irrbtn, touch_button_images[btn], rect, m_tsrc, RenderingEngine::get_video_driver());
	}
}

void GUITouchscreenLayout::drawMenu()
{
	gui::IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;
	video::IVideoDriver* driver = Environment->getVideoDriver();
	video::SColor bgcolor(200, 0, 0, 0);
	driver->draw2DRectangle(bgcolor, AbsoluteRect, &AbsoluteClippingRect);
	gui::IGUIElement::draw();
}

bool GUITouchscreenLayout::OnEvent(const SEvent& event)
{
	if (event.EventType == EET_KEY_INPUT_EVENT) {
		if (event.KeyInput.Key == KEY_ESCAPE && event.KeyInput.PressedDown) {
			quitMenu();
			return true;
		}
	}

	return Parent ? Parent->OnEvent(event) : false;
}
