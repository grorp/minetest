#include "touchscreenlayout.h"
#include "ITexture.h"
#include "client/texturesource.h"
#include "debug.h"
#include "log.h"
#include "settings.h"
#include "client/renderingengine.h"
#include <optional>

#define SETTINGS_BAR_Y_OFFSET 5
#define RARE_CONTROLS_BAR_Y_OFFSET 5

ButtonLayout ButtonLayout::getDefault(v2u32 screensize) {
	u32 button_size = std::min(screensize.Y / 4.5f,
			RenderingEngine::getDisplayDensity() * 65.0f * g_settings->getFloat("hud_scaling"));
	ButtonLayout l = {{
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
	l.layout[BTN_SETTINGS_BAR].bar->dir = ButtonBarDir::Left;
	l.layout[BTN_SETTINGS_BAR].bar->content = {BTN_FLY, BTN_NOCLIP, BTN_FAST, BTN_DEBUG, BTN_CAMERA_MODE, BTN_RANGESELET, BTN_MINIMAP, BTN_TOGGLE_CHAT};

	l.layout[BTN_RARE_CONTROLS_BAR].bar.emplace();
	l.layout[BTN_RARE_CONTROLS_BAR].bar->dir = ButtonBarDir::Right;
	l.layout[BTN_RARE_CONTROLS_BAR].bar->content = {BTN_CHAT, BTN_INVENTORY, BTN_DROP, BTN_EXIT};

	return l;
}


video::ITexture *ButtonLayout::getTexture(TouchButton btn, ISimpleTextureSource *tsrc)
{
    static const std::string filenames[] = {
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

        "placeholder_btn.png",
    };
	return tsrc->getTexture(filenames[btn]);
}

core::rect<s32> ButtonLayout::getRectSimple(TouchButton btn, const ButtonMeta &meta, ISimpleTextureSource *tsrc)
{
	v2u32 orig_size = getTexture(btn, tsrc)->getOriginalSize();
	return core::rect<s32>(
		meta.pos.X,
		meta.pos.Y,
		meta.pos.X + (f32)orig_size.X / (f32)orig_size.Y * meta.height,
		meta.pos.Y + meta.height);
}

core::rect<s32> ButtonLayout::getRectSimple(TouchButton btn, ISimpleTextureSource *tsrc) const
{
	ButtonMeta meta = layout.at(btn);
	return getRectSimple(btn, meta, tsrc);
}

static core::rect<s32> resize_for_different_button(ButtonBarDir dir, core::rect<s32> rect, v2u32 orig_size) {
	switch (dir) {
	case ButtonBarDir::Left:
	case ButtonBarDir::Right: {
		rect.LowerRightCorner.X = rect.UpperLeftCorner.X +
				(f32)orig_size.X / (f32)orig_size.Y * rect.getHeight();
		break;
	}
	case ButtonBarDir::Down:
	case ButtonBarDir::Up:
		rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y +
				(f32)orig_size.Y / (f32)orig_size.X * rect.getWidth();
		break;
	}
	return rect;
}

static core::rect<s32> apply_offset(ButtonBarDir dir, core::rect<s32> rect) {
	s32 offset;
	switch (dir) {
	case ButtonBarDir::Left:
	case ButtonBarDir::Right:
		offset = rect.getWidth();
		break;
	case ButtonBarDir::Down:
	case ButtonBarDir::Up:
		offset = rect.getHeight();
		break;
	}

	switch (dir) {
	case ButtonBarDir::Left:
		rect.UpperLeftCorner.X  -= offset;
		rect.LowerRightCorner.X -= offset;
		break;
	case ButtonBarDir::Right:
		rect.UpperLeftCorner.X  += offset;
		rect.LowerRightCorner.X += offset;
		break;
	case ButtonBarDir::Down:
		rect.UpperLeftCorner.Y  += offset;
		rect.LowerRightCorner.Y += offset;
		break;
	case ButtonBarDir::Up:
		rect.UpperLeftCorner.Y  -= offset;
		rect.LowerRightCorner.Y -= offset;
		break;
	}
	return rect;
}

core::rect<s32> ButtonLayout::getRect(TouchButton btn, ISimpleTextureSource *tsrc, std::optional<TouchButton> dragged_btn) const
{
	if (layout.count(btn) == 1)
		return getRectSimple(btn, tsrc);

	for (auto &v : layout) {
		TouchButton outer_btn = v.first;
		const ButtonMeta &outer_meta = v.second;

		if (outer_meta.bar.has_value()) {
			core::rect<s32> rect = getRectSimple(outer_btn, tsrc);
			const ButtonBar &bar = *outer_meta.bar;

			if (bar.dir == ButtonBarDir::Right || bar.dir == ButtonBarDir::Down)
				rect = apply_offset(bar.dir, rect);

			for (auto &w : bar.content) {
				TouchButton inner_btn = w;

				v2u32 orig_size = getTexture(inner_btn == BTN_PLACEHOLDER ? *dragged_btn : inner_btn, tsrc)->getOriginalSize();
				rect = resize_for_different_button(bar.dir, rect, orig_size);
				if (bar.dir == ButtonBarDir::Left || bar.dir == ButtonBarDir::Up)
					rect = apply_offset(bar.dir, rect);

				if (inner_btn == btn)
					return rect;

				if (bar.dir == ButtonBarDir::Right || bar.dir == ButtonBarDir::Down)
					rect = apply_offset(bar.dir, rect);
			}
		}
	}

	throw std::out_of_range("button doesn't exist in layout");
}


static void iterate_buttonbar(TouchButton launcher_btn, const ButtonMeta& launcher_meta,
		std::optional<TouchButton> dragged_btn,
		const std::function<void(TouchButton, core::rect<s32>)>& cb, ISimpleTextureSource *tsrc)
{
	core::rect<s32> rect = ButtonLayout::getRectSimple(launcher_btn, launcher_meta, tsrc);
	const ButtonBar &bar = launcher_meta.bar.value();

	if (bar.dir == ButtonBarDir::Right || bar.dir == ButtonBarDir::Down)
		rect = apply_offset(bar.dir, rect);

	for (const auto &inner_btn : bar.content) {
		TouchButton effective = inner_btn == BTN_PLACEHOLDER ? dragged_btn.value() : inner_btn;
		v2u32 orig_size = ButtonLayout::getTexture(effective, tsrc)->getOriginalSize();

		rect = resize_for_different_button(bar.dir, rect, orig_size);
		if (bar.dir == ButtonBarDir::Left || bar.dir == ButtonBarDir::Up)
			rect = apply_offset(bar.dir, rect);

		cb(inner_btn, rect);

		if (bar.dir == ButtonBarDir::Right || bar.dir == ButtonBarDir::Down)
			rect = apply_offset(bar.dir, rect);
	}
}

bool ButtonLayout::shouldRender(TouchButton btn, std::optional<TouchButton> expanded_bar) const
{
	if (layout.count(btn) == 1)
		return true;

	for (auto &v : layout) {
		if (v.first == expanded_bar && v.second.bar.has_value()) {
			for (auto &w : v.second.bar->content) {
				if (w == btn)
					return true;
			}
		}
	}
	
	return false;
}

bool ButtonLayout::shouldInterpolate(TouchButton btn, std::optional<TouchButton> dragged_btn) const
{
	if (btn == dragged_btn)
		return false;

	for (auto &v : layout) {
		if (v.first == dragged_btn && v.second.bar.has_value()) {
			for (auto &w : v.second.bar->content) {
				if (w == btn)
					return false;
			}
		}
	}

	return true;
}

void ButtonLayout::remove(TouchButton btn)
{
	if (layout.count(btn) == 1) {
		layout.erase(btn);
		return;
	}

	for (auto &v : layout) {
		if (v.second.bar.has_value()) {
			auto &content = v.second.bar->content;
			auto it = std::find(content.begin(), content.end(), btn);
			if (it != content.end()) {
				content.erase(it);
				return;
			}
		}
	}

	throw std::out_of_range("button doesn't exist in layout");
}

std::optional<core::rect<s32>> ButtonLayout::add(TouchButton btn, const ButtonMeta &meta, ISimpleTextureSource *tsrc, bool really, std::optional<TouchButton> expanded_bar)
{
	core::rect<s32> our_rect = getRectSimple(btn, meta, tsrc);
	v2f32 our_center = core::rect<f32>(our_rect.UpperLeftCorner.X, our_rect.UpperLeftCorner.Y,
			our_rect.LowerRightCorner.X, our_rect.LowerRightCorner.Y).getCenter();

	bool can_add_to_bar = btn != BTN_JOYSTICK && !meta.bar.has_value();

	std::unordered_map<TouchButton, core::rect<s32>> notfull_rects;
	std::unordered_map<TouchButton, core::rect<s32>> full_rects;

	for (auto &v : layout) {
		core::rect<s32> notfull_rect = getRectSimple(v.first, v.second, tsrc);
		core::rect<s32> full_rect = notfull_rect;

		if (v.second.bar.has_value()) {
			size_t closest_index = 0;
			f32 closest_distance_sq = std::numeric_limits<f32>::max();

			// Pretend that the button we want to insert exists at the end of
			// the buttonbar so that there is a drop slot at the end of the
			// buttonbar as well.
			auto meta_bar_extended = v.second;
			meta_bar_extended.bar->content.emplace_back(btn);

			size_t i = 0;
			iterate_buttonbar(v.first, meta_bar_extended, btn, [&](TouchButton inner_btn, core::rect<s32> inner_rect) {
				if (inner_btn != btn) {
					// Don't include the fake button at the end of the buttonbar
					// in full_rect. This is necessary to allow adding a button
					// outside the buttonbar just past the end of the buttonbar.
					// Since the fake button is fake, it shouldn't block the
					// addition of other buttons.
					full_rect.addInternalPoint(inner_rect.UpperLeftCorner);
					full_rect.addInternalPoint(inner_rect.LowerRightCorner);
				}

				v2f32 inner_center = core::rect<f32>(inner_rect.UpperLeftCorner.X, inner_rect.UpperLeftCorner.Y,
						inner_rect.LowerRightCorner.X, inner_rect.LowerRightCorner.Y).getCenter();
				f32 distance_sq = our_center.getDistanceFromSQ(inner_center);
				if (distance_sq < closest_distance_sq) {
					closest_index = i;
					closest_distance_sq = distance_sq;
				}

				i++;
			}, tsrc);

			if (can_add_to_bar && v.first == expanded_bar && our_rect.isRectCollided(full_rect)) {
				auto &bar = v.second.bar.value();
				if (really) {
					bar.content.insert(bar.content.begin() + closest_index, btn);
				} else {
					bar.content.insert(bar.content.begin() + closest_index, BTN_PLACEHOLDER);
					layout[btn] = meta;
				}
				return std::nullopt; // dropped
			}
		}

		notfull_rects[v.first] = notfull_rect;
		full_rects[v.first] = full_rect;
	}


	for (auto &v : full_rects) {
		if (v.second.isRectCollided(our_rect))
			return v.second; // can't drop!!!
	}
	layout[btn] = meta;
	return std::nullopt;
}
