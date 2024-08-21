#include "guiScroller.h"
#include "client/renderingengine.h"

bool GUIScroller::handleSwipeEvent(GUIScrollBar *scrollbar, f32 scrollfactor, const irr::SEvent &event)
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
			f32 min_movement = RenderingEngine::getDisplayDensity() * 10.0f;

			if (!m_swipe_started && m_swipe_start_y != -1 &&
					std::abs(m_swipe_start_y - pos.Y + scrollbar->getPos() * scrollfactor) >
							min_movement) {
				m_swipe_started = true;
				Environment->setFocus(this);
			}

			if (m_swipe_started) {
				// interpolated since the initial jump is quite ... jumpy otherwise
				scrollbar->setPosInterpolated((float)(pos.Y - m_swipe_start_y) / scrollfactor);
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
