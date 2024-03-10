#pragma once

#include <vector>
#include <optional>
#include <unordered_map>
#include <map>
#include "irrlichttypes_bloated.h"
#include "client/texturesource.h"

enum TouchButton : u8 {
	BTN_RARE_CONTROLS_BAR,
	BTN_SETTINGS_BAR,
	BTN_JOYSTICK,
	BTN_JUMP,
	BTN_SNEAK,
	BTN_ZOOM,
	BTN_AUX1,

	/* in the settings bar by default */
	BTN_FLY,
	BTN_NOCLIP,
	BTN_FAST,
	BTN_DEBUG,
	BTN_CAMERA_MODE,
	BTN_RANGESELET,
	BTN_MINIMAP,
	BTN_TOGGLE_CHAT,

	/* in the rare controls bar by default */
	BTN_CHAT,
	BTN_INVENTORY,
	BTN_DROP,
	BTN_EXIT,

	/* dummy placeholder */
	BTN_PLACEHOLDER,

	/* not a button btw */
	TouchButton_END,
};

enum class ButtonBarDir {
	Left,
	Up,
	Right,
	Down,
};

struct ButtonBar {
	ButtonBarDir dir;
	std::vector<TouchButton> content;
};

struct ButtonMeta {
	v2s32 pos;
	u32 height;
	std::optional<ButtonBar> bar;
};

struct ButtonLayout {
	std::unordered_map<TouchButton, ButtonMeta> layout;

    static ButtonLayout getDefault(v2u32 screensize);

    static video::ITexture *getTexture(TouchButton btn, ISimpleTextureSource *tsrc);
	static core::rect<s32> getRectSimple(TouchButton btn, const ButtonMeta &meta, ISimpleTextureSource *tsrc);
	core::rect<s32> getRectSimple(TouchButton btn, ISimpleTextureSource *tsrc) const;
	core::rect<s32> getRect(TouchButton btn, ISimpleTextureSource *tsrc, std::optional<TouchButton> dragged_btn = std::nullopt) const;

	bool shouldRender(TouchButton btn, std::optional<TouchButton> expanded_bar) const;
    bool shouldInterpolate(TouchButton btn, std::optional<TouchButton> dragged_btn) const;
	void remove(TouchButton btn);
	// returns std::nullopt on success, or the TouchButton that prevented the addition on failure
	std::vector<core::rect<s32>> add(TouchButton btn, const ButtonMeta &meta, ISimpleTextureSource *tsrc, bool really, std::optional<TouchButton> expanded_bar);
};

void iterate_buttonbar(TouchButton launcher_btn, const ButtonMeta& launcher_meta,
		std::optional<TouchButton> dragged_btn,
		const std::function<bool(TouchButton, core::rect<s32>)>& cb, ISimpleTextureSource *tsrc);
