#pragma once

#include "IGUIElement.h"
#include "gui/guiScrollBar.h"

using namespace irr;
using namespace gui;

class GUIScroller : public IGUIElement {
public:
	using IGUIElement::IGUIElement;

	// preconditions:
	// 1. no nullptrs
	// 2. scrollbar is vertical
	bool handleSwipeEvent(GUIScrollBar *scrollbar, f32 scrollfactor, const irr::SEvent &event);

private:
	bool m_swipe_started = false;
	s32 m_swipe_start_y = -1;
	bool m_last_lmouse_up_was_swipe = false;
};
