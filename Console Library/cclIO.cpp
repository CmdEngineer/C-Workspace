#include "ccl.h"

void io::IoEventHandler::onMousePressedEvent(events::MouseButton button, Point pos)
{
	for (Element* elem : elements)
		if (elem->onclick)
			if (elem->region.contains(pos))
				elem->onclick(pos);
}
io::Button::Button(Region _region, BrushEx brush)
{
	graphicBrush = brush;
	region = _region;
	elements.push_back(this);
}
io::Button::~Button()
{
	elements.remove(this);
}

extern DWORD eventsRead;
extern INPUT_RECORD eventsBuffer[128];

void events::getEvents()
{
	DWORD fdwSaveOldMode; DWORD fdwMode;
	HANDLE input = console::getInput();
	GetConsoleMode(input, &fdwSaveOldMode);
	fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	SetConsoleMode(input, fdwMode);
	while (true)
	{
		ReadConsoleInput(input, eventsBuffer, 128, &eventsRead);
		for (size_t i = 0; i < eventsRead; i++)
		{
			for (i = 0; i < eventsRead; i++)
			{
				switch (eventsBuffer[i].EventType)
				{
				case KEY_EVENT:
					parseKeyEvent(eventsBuffer[i].Event.KeyEvent);
					break;
				case MOUSE_EVENT:
					parseMouseEvent(eventsBuffer[i].Event.MouseEvent);
					break;
				case WINDOW_BUFFER_SIZE_EVENT:
					RAISE(*Event::getInstance()).WindowResizedEvent(eventsBuffer[i].Event.WindowBufferSizeEvent.dwSize);
					break;
				case FOCUS_EVENT:
					// IGNORE INTERNAL EVENT
				case MENU_EVENT:
					// IGNORE INTERNAL EVENT
					break;
				default:
					// Unknown event type;
					break;
				}
			}
		}
	}
	SetConsoleMode(input, fdwSaveOldMode);
}
events::Event* events::Event::instance = 0;
events::Event* events::Event::getInstance()
{
	if (!instance)
	{
		instance = new Event();
	}
	return instance;
}

void events::parseKeyEvent(KEY_EVENT_RECORD e)
{
	RAISE(*Event::getInstance()).KeyEvent(e);
	if (e.bKeyDown)
	{
		RAISE(*Event::getInstance()).KeyPressedEvent(e.wVirtualKeyCode);
	}
	else
	{
		RAISE(*Event::getInstance()).KeyReleasedEvent(e.wVirtualKeyCode);
	}
}
void events::parseMouseEvent(MOUSE_EVENT_RECORD e)
{
	RAISE(*Event::getInstance()).MouseEvent(e);
#ifndef MOUSE_HWHEELED
#define MOUSE_HWHEELED 0x0008
#endif
	switch (e.dwEventFlags)
	{
	case 0:
		if (e.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			RAISE(*Event::getInstance()).MousePressedEvent(LEFT_MB, Point(e.dwMousePosition));
		}
		else if (e.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
		{
			RAISE(*Event::getInstance()).MousePressedEvent(RIGHT_MB, Point(e.dwMousePosition));
		}
		else
		{
			RAISE(*Event::getInstance()).MousePressedEvent(ANY_MB, Point(e.dwMousePosition));
		}
		break;
	case DOUBLE_CLICK:
		if (e.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			RAISE(*Event::getInstance()).MouseDoubleClickEvent(LEFT_MB, Point(e.dwMousePosition));
		}
		else if (e.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
		{
			RAISE(*Event::getInstance()).MouseDoubleClickEvent(RIGHT_MB, Point(e.dwMousePosition));
		}
		else
		{
			RAISE(*Event::getInstance()).MouseDoubleClickEvent(ANY_MB, Point(e.dwMousePosition));
		}
		break;
	case MOUSE_HWHEELED:
		RAISE(*Event::getInstance()).MouseWheeledEvent(HORIZONTAL_MW, Point(e.dwMousePosition));
		break;
	case MOUSE_MOVED:
		RAISE(*Event::getInstance()).MouseMoveEvent(Point(e.dwMousePosition));
		break;
	case MOUSE_WHEELED:
		RAISE(*Event::getInstance()).MouseWheeledEvent(VERTICAL_MW, Point(e.dwMousePosition));
		break;
	default:
		break;
	}
}
events::EventHandler::EventHandler()
{
	std::future<void> result(std::async(getEvents));
	HOOK(&Event::KeyEvent, Event::getInstance(), &EventHandler::onKeyEvent);
	HOOK(&Event::KeyPressedEvent, Event::getInstance(), &EventHandler::onKeyPressedEvent);
	HOOK(&Event::KeyReleasedEvent, Event::getInstance(), &EventHandler::onKeyReleasedEvent);
	HOOK(&Event::MouseEvent, Event::getInstance(), &EventHandler::onMouseEvent);
	HOOK(&Event::MouseMoveEvent, Event::getInstance(), &EventHandler::onMouseMoveEvent);
	HOOK(&Event::MousePressedEvent, Event::getInstance(), &EventHandler::onMousePressedEvent);
	HOOK(&Event::MouseWheeledEvent, Event::getInstance(), &EventHandler::onMouseWheeledEvent);
	HOOK(&Event::MouseDoubleClickEvent, Event::getInstance(), &EventHandler::onMouseDoubleClickEvent);
	HOOK(&Event::WindowResizedEvent, Event::getInstance(), &EventHandler::onWindowResizedEvent);
	thread = result.share();
}
events::EventHandler::~EventHandler()
{
	UNHOOK(&Event::KeyEvent, Event::getInstance(), &EventHandler::onKeyEvent);
	UNHOOK(&Event::KeyPressedEvent, Event::getInstance(), &EventHandler::onKeyPressedEvent);
	UNHOOK(&Event::KeyReleasedEvent, Event::getInstance(), &EventHandler::onKeyReleasedEvent);
	UNHOOK(&Event::MouseEvent, Event::getInstance(), &EventHandler::onMouseEvent);
	UNHOOK(&Event::MouseMoveEvent, Event::getInstance(), &EventHandler::onMouseMoveEvent);
	UNHOOK(&Event::MousePressedEvent, Event::getInstance(), &EventHandler::onMousePressedEvent);
	UNHOOK(&Event::MouseWheeledEvent, Event::getInstance(), &EventHandler::onMouseWheeledEvent);
	UNHOOK(&Event::MouseDoubleClickEvent, Event::getInstance(), &EventHandler::onMouseDoubleClickEvent);
	thread.get();
};