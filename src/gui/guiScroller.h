#pragma once

#include "IEventReceiver.h"
#include "IGUIElement.h"
#include "IGUIEnvironment.h"
#include "client/renderingengine.h"
#include "gui/guiScrollBar.h"

using namespace irr;
using namespace gui;

class GUIScroller : public IGUIElement {
public:
	using IGUIElement::IGUIElement;

	// preconditions:
	// 1. no nullptrs
	// 2. scrollbar is vertical
	bool handleSwipeEvent(GUIScrollBar *scrollbar, f32 scrollfactor, const irr::SEvent &event)
	{
		if (event.EventType == EET_MOUSE_INPUT_EVENT) {
			v2s32 pos(event.MouseInput.X, event.MouseInput.Y);

			switch (event.MouseInput.Event) {
			case EMIE_LMOUSE_PRESSED_DOWN: {
				if (isPointInside(pos))
					m_swipe_start_y = pos.Y - scrollbar->getPos() * scrollfactor;
				break;
			}
			case EMIE_MOUSE_MOVED: {
				double screen_dpi = RenderingEngine::getDisplayDensity() * 96;

				if (!m_swipe_started && m_swipe_start_y != -1 &&
						std::abs(m_swipe_start_y - pos.Y + scrollbar->getPos() * scrollfactor) >
								0.1 * screen_dpi) {
					m_swipe_started = true;
					Environment->setFocus(this);
				}

				if (m_swipe_started) {
					scrollbar->setPosAndSend((float)(pos.Y - m_swipe_start_y) / scrollfactor);
					return true;
				}
				break;
			}
			case EMIE_LMOUSE_LEFT_UP: {
				m_swipe_start_y = -1;
				if (m_swipe_started) {
					m_swipe_started = false;
					return true;
				}
				break;
			}
			default:
				break;
			}
        }
		return false;
	}

private:
	bool m_swipe_started = false;
	s32 m_swipe_start_y = -1;
};
