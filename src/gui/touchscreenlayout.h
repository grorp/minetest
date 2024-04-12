#pragma once

#include <vector>
#include <optional>
#include <unordered_map>
#include <map>
#include "irrlichttypes_bloated.h"
#include "client/texturesource.h"

typedef enum
{
	jump_id = 0,
	sneak_id,
	zoom_id,
	aux1_id,
	settings_starter_id,
	rare_controls_starter_id,

	// usually in the "settings bar"
	fly_id,
	noclip_id,
	fast_id,
	debug_id,
	camera_id,
	range_id,
	minimap_id,
	toggle_chat_id,

	// usually in the "rare controls bar"
	chat_id,
	inventory_id,
	drop_id,
	exit_id,

	// the joystick
	joystick_off_id,
	joystick_bg_id,
	joystick_center_id,

	placeholder_id,
	touch_gui_button_id_END,
} touch_gui_button_id;

const std::string button_image_names[] = {
	"jump_btn.png",
	"down.png",
	"zoom.png",
	"aux1_btn.png",
	"gear_icon.png",
	"rare_controls.png",

	"fly_btn.png",
	"noclip_btn.png",
	"fast_btn.png",
	"debug_btn.png",
	"camera_btn.png",
	"rangeview_btn.png",
	"minimap_btn.png",
	"chat_hide_btn.png", // FIXME

	"chat_btn.png",
	"inventory_btn.png",
	"drop_btn.png",
	"exit_btn.png",

	"joystick_off.png",
	"joystick_bg.png",
	"joystick_center.png",
};

typedef enum
{
	AHBB_Dir_Top_Bottom,
	AHBB_Dir_Bottom_Top,
	AHBB_Dir_Left_Right,
	AHBB_Dir_Right_Left
} autohide_button_bar_dir;


struct ButtonBar {
	autohide_button_bar_dir dir;
	std::vector<touch_gui_button_id> content;
};

struct ButtonMeta {
	v2s32 pos;
	u32 height;
	std::optional<ButtonBar> bar;
};

struct ButtonLayout {
	std::unordered_map<touch_gui_button_id, ButtonMeta> layout;

    static ButtonLayout getDefault(v2u32 screensize);

    static video::ITexture *getTexture(touch_gui_button_id btn, ISimpleTextureSource *tsrc);
	static core::rect<s32> getRectSimple(touch_gui_button_id btn, const ButtonMeta &meta, ISimpleTextureSource *tsrc);
	core::rect<s32> getRectSimple(touch_gui_button_id btn, ISimpleTextureSource *tsrc) const;
	core::rect<s32> getRect(touch_gui_button_id btn, ISimpleTextureSource *tsrc, std::optional<touch_gui_button_id> dragged_btn = std::nullopt) const;

	bool shouldRender(touch_gui_button_id btn, std::optional<touch_gui_button_id> expanded_bar) const;
    bool shouldInterpolate(touch_gui_button_id btn, std::optional<touch_gui_button_id> dragged_btn) const;
	void remove(touch_gui_button_id btn);
	// returns std::nullopt on success, or the touch_gui_button_id that prevented the addition on failure
	std::vector<core::rect<s32>> add(touch_gui_button_id btn, const ButtonMeta &meta, ISimpleTextureSource *tsrc, bool really, std::optional<touch_gui_button_id> expanded_bar);
};

void iterate_buttonbar(touch_gui_button_id launcher_btn, const ButtonMeta& launcher_meta,
		std::optional<touch_gui_button_id> dragged_btn,
		const std::function<bool(touch_gui_button_id, core::rect<s32>)>& cb, ISimpleTextureSource *tsrc);
