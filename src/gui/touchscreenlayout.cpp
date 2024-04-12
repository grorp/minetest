#include "touchscreenlayout.h"
#include "ITexture.h"
#include "client/texturesource.h"
#include "debug.h"
#include "log.h"
#include "settings.h"
#include "client/renderingengine.h"
#include <optional>
#include <unordered_map>

#define SETTINGS_BAR_Y_OFFSET 5
#define RARE_CONTROLS_BAR_Y_OFFSET 5

ButtonLayout ButtonLayout::getDefault(v2u32 screensize) {
	u32 button_size = std::min(screensize.Y / 4.5f,
			RenderingEngine::getDisplayDensity() * 65.0f * g_settings->getFloat("hud_scaling"));
	ButtonLayout l = {{
		{rare_controls_starter_id, {
			.pos = v2s32(0.25f * button_size, screensize.Y - (RARE_CONTROLS_BAR_Y_OFFSET + 1.0f) * button_size + 0.5f * button_size),
			.height = button_size,
		}},
		{settings_starter_id, {
			.pos = v2s32(screensize.X - 1.25f * button_size, screensize.Y - (SETTINGS_BAR_Y_OFFSET + 1.0f) * button_size + 0.5f * button_size),
			.height = button_size,
		}},
		{joystick_off_id, {
			.pos = v2s32(button_size, screensize.Y - button_size * 4),
			.height = button_size * 3,
		}},
		{jump_id, {
			.pos = v2s32(screensize.X - 1.75f * button_size, screensize.Y - button_size),
			.height = button_size,
		}},
		{sneak_id, {
			.pos = v2s32(screensize.X - 3.25f * button_size, screensize.Y - button_size),
			.height = button_size,
		}},
		{zoom_id, {
			.pos = v2s32(screensize.X - 1.25f * button_size, screensize.Y - 4 * button_size),
			.height = button_size,
		}},
		{aux1_id, {
			.pos = v2s32(screensize.X - 1.25f * button_size, screensize.Y - 2.5f * button_size),
			.height = button_size,
		}},
	}};

	l.layout[settings_starter_id].bar.emplace();
	l.layout[settings_starter_id].bar->dir = ButtonBarDir::Left;
	l.layout[settings_starter_id].bar->content = {fly_id, noclip_id, fast_id, debug_id, camera_id, range_id, minimap_id, toggle_chat_id};

	l.layout[rare_controls_starter_id].bar.emplace();
	l.layout[rare_controls_starter_id].bar->dir = ButtonBarDir::Right;
	l.layout[rare_controls_starter_id].bar->content = {chat_id, inventory_id, drop_id, exit_id};

	return l;
}


video::ITexture *ButtonLayout::getTexture(touch_gui_button_id btn, ISimpleTextureSource *tsrc)
{
	return tsrc->getTexture(button_image_names[btn]);
}

core::rect<s32> ButtonLayout::getRectSimple(touch_gui_button_id btn, const ButtonMeta &meta, ISimpleTextureSource *tsrc)
{
	v2u32 orig_size = getTexture(btn, tsrc)->getOriginalSize();
	return core::rect<s32>(
		meta.pos.X,
		meta.pos.Y,
		meta.pos.X + (f32)orig_size.X / (f32)orig_size.Y * meta.height,
		meta.pos.Y + meta.height);
}

core::rect<s32> ButtonLayout::getRectSimple(touch_gui_button_id btn, ISimpleTextureSource *tsrc) const
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

core::rect<s32> ButtonLayout::getRect(touch_gui_button_id btn, ISimpleTextureSource *tsrc, std::optional<touch_gui_button_id> dragged_btn) const
{
	if (layout.count(btn) == 1)
		return getRectSimple(btn, layout.at(btn), tsrc);

	for (const auto &[launcher_btn, launcher_meta] : layout) {
		if (launcher_meta.bar.has_value()) {
			std::optional<core::rect<s32>> rect = std::nullopt;
			iterate_buttonbar(launcher_btn, launcher_meta, dragged_btn, [&](touch_gui_button_id inner_btn, core::rect<s32> inner_rect) {
				if (inner_btn == btn) {
					rect = inner_rect;
					return true;
				}
				return false;
			}, tsrc);
			if (rect.has_value())
				return rect.value();
		}
	}

	throw std::out_of_range("button doesn't exist in layout");
}

// dragged_btn is used for replacing occurrences of placeholder_id. If no
// occurrences of placeholder_id are expected, dragged_btn should be
// std::nullopt.
void iterate_buttonbar(touch_gui_button_id launcher_btn, const ButtonMeta& launcher_meta,
		std::optional<touch_gui_button_id> dragged_btn,
		const std::function<bool(touch_gui_button_id, core::rect<s32>)>& cb, ISimpleTextureSource *tsrc)
{
	core::rect<s32> rect = ButtonLayout::getRectSimple(launcher_btn, launcher_meta, tsrc);
	const ButtonBar &bar = launcher_meta.bar.value();

	if (bar.dir == ButtonBarDir::Right || bar.dir == ButtonBarDir::Down)
		rect = apply_offset(bar.dir, rect);

	for (const auto &inner_btn : bar.content) {
		touch_gui_button_id effective = inner_btn == placeholder_id ? dragged_btn.value() : inner_btn;
		v2u32 orig_size = ButtonLayout::getTexture(effective, tsrc)->getOriginalSize();

		rect = resize_for_different_button(bar.dir, rect, orig_size);
		if (bar.dir == ButtonBarDir::Left || bar.dir == ButtonBarDir::Up)
			rect = apply_offset(bar.dir, rect);

		if (cb(inner_btn, rect))
			return;

		if (bar.dir == ButtonBarDir::Right || bar.dir == ButtonBarDir::Down)
			rect = apply_offset(bar.dir, rect);
	}
}

bool ButtonLayout::shouldRender(touch_gui_button_id btn, std::optional<touch_gui_button_id> expanded_bar) const
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

bool ButtonLayout::shouldInterpolate(touch_gui_button_id btn, std::optional<touch_gui_button_id> dragged_btn) const
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

void ButtonLayout::remove(touch_gui_button_id btn)
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

v2f32 getRectCenter(const core::rect<s32> &rect) {
	return core::rect<f32>(rect.UpperLeftCorner.X, rect.UpperLeftCorner.Y,
			rect.LowerRightCorner.X, rect.LowerRightCorner.Y).getCenter();
}

void addRect(core::rect<s32> &rect, const core::rect<s32> &other) {
	rect.addInternalPoint(other.UpperLeftCorner);
	rect.addInternalPoint(other.LowerRightCorner);
}

std::vector<core::rect<s32>> ButtonLayout::add(touch_gui_button_id btn, const ButtonMeta &meta, ISimpleTextureSource *tsrc, bool really, std::optional<touch_gui_button_id> expanded_bar)
{
	core::rect<s32> our_rect = getRectSimple(btn, meta, tsrc);
	v2f32 our_center = getRectCenter(our_rect);

	// We may not add the joystick to a buttonbar.
	// We may not add a buttonbar to a buttonbar.
	bool can_add_to_bar = btn != joystick_off_id && !meta.bar.has_value();

	// Rectangles for collision detection in stage 2
	std::unordered_map<touch_gui_button_id, core::rect<s32>> other_rects;
	std::unordered_map<touch_gui_button_id, core::rect<s32>> other_full_rects;

	/*
		Stage 1: Try to add the button to a buttonbar
		This also needs to run if !can_add_to_bar because it collects
		other_rects and other_full_rects.
	*/
	for (auto &[other_btn, other_meta] : layout) {
		core::rect<s32> other_rect = getRectSimple(other_btn, other_meta, tsrc);
		other_rects.emplace(other_btn, other_rect);
	}

	for (auto &[other_btn, other_meta] : layout) {
		// The rectangle of (just) the button
		core::rect<s32> other_rect = other_rects.at(other_btn);
		// The rectangle of the button, including the contents of the attached
		// buttonbar if there is one
		core::rect<s32> other_full_rect = other_rect;

		if (other_meta.bar.has_value()) {
			size_t closest_index = 0;
			f32 closest_distance_sq = std::numeric_limits<f32>::max();

			// The new rectangle of the button + attached buttonbar if we would
			// add our button to it
			core::rect<s32> other_new_full_rect = other_rect;

			// Pretend that the button we want to insert exists at the end of
			// the buttonbar so that there is a drop slot at the end of the
			// buttonbar as well.
			// This is also useful for calculating other_new_full_rect.
			auto meta_bar_extended = other_meta;
			meta_bar_extended.bar->content.emplace_back(btn);

			size_t i = 0;
			iterate_buttonbar(other_btn, meta_bar_extended, std::nullopt, [&](touch_gui_button_id inner_btn, core::rect<s32> inner_rect) {
				if (inner_btn != btn) {
					// Don't include the fake button at the end of the buttonbar
					// in other_full_rect. This is necessary to allow adding a
					// button outside the buttonbar just past the end of the
					// buttonbar.
					addRect(other_full_rect, inner_rect);
				}
				addRect(other_new_full_rect, inner_rect);

				v2f32 inner_center = getRectCenter(inner_rect);
				f32 distance_sq = our_center.getDistanceFromSQ(inner_center);
				if (distance_sq < closest_distance_sq) {
					closest_index = i;
					closest_distance_sq = distance_sq;
				}

				i++;
				return false;
			}, tsrc);

			if (can_add_to_bar && other_btn == expanded_bar && our_rect.isRectCollided(other_full_rect)) {
				std::vector<core::rect<s32>> bad_rects;
				for (auto [other2_btn, other2_rect] : other_rects) {
					if (other2_btn != other_btn && other_new_full_rect.isRectCollided(other2_rect))
						// The buttonbar would start to intersect with another
						// button if we added our button to it.
						bad_rects.emplace_back(other2_rect);
				}
				if (!bad_rects.empty())
					return bad_rects; // failure

				auto &bar = other_meta.bar.value();
				if (really) {
					bar.content.insert(bar.content.begin() + closest_index, btn);
				} else {
					bar.content.insert(bar.content.begin() + closest_index, placeholder_id);
				}
				return {}; // success
			}
		}

		other_full_rects.emplace(other_btn, other_full_rect);
	}

	/*
		The button can't be added to a buttonbar.
		Stage 2: Try to add the button normally.
	*/

	core::rect<s32> our_full_rect = our_rect;
	if (meta.bar.has_value()) {
		iterate_buttonbar(btn, meta, std::nullopt, [&](touch_gui_button_id inner_btn, core::rect<s32> inner_rect) {
			our_full_rect.addInternalPoint(inner_rect.UpperLeftCorner);
			our_full_rect.addInternalPoint(inner_rect.LowerRightCorner);
			return false;
		}, tsrc);
	}

	std::vector<core::rect<s32>> bad_rects;
	for (auto &[other_btn, other_full_rect] : other_full_rects) {
		if (our_rect.isRectCollided(other_full_rect))
			// Our button would intersect with the contents of another buttonbar
			// (or with another button) if we added it here.
			bad_rects.emplace_back(other_full_rect);
	}
	for (auto &[other_btn, other_rect] : other_rects) {
		if (our_full_rect.isRectCollided(other_rect)) {
			// The contents of our buttonbar (or our button) would intersect
			// with another button if we added it here.
			auto it = std::find(bad_rects.begin(), bad_rects.end(),
					other_full_rects.at(other_btn));
			// No need to add other_rect if we've already added other_full_rect.
			if (it == bad_rects.end())
				bad_rects.emplace_back(other_rect);
		}
	}
	if (!bad_rects.empty())
		return bad_rects; // failure

	if (really)
		layout.emplace(btn, meta);
	return {}; // success
}
