#pragma once

namespace events
{
	enum MouseButton { ANY_MB, LEFT_MB, RIGHT_MB };
	enum MouseWheel { ANY_MW, HORIZONTAL_MW, VERTICAL_MW };

	static DWORD eventsRead;
	static INPUT_RECORD eventsBuffer[128];
	void getEvents();
	void parseKeyEvent(KEY_EVENT_RECORD e);
	void parseMouseEvent(MOUSE_EVENT_RECORD e);

	class Event
	{
	private:
		static Event* instance;
		Event() {};
	public:
		static Event* getInstance();
		virtual EVENT void KeyEvent(KEY_EVENT_RECORD e);
		virtual EVENT void MouseEvent(MOUSE_EVENT_RECORD e);
		virtual EVENT void KeyPressedEvent(WORD key);
		virtual EVENT void KeyReleasedEvent(WORD key);
		virtual EVENT void MouseMoveEvent(Point pos);
		virtual EVENT void MousePressedEvent(MouseButton button, Point pos);
		virtual EVENT void MouseDoubleClickEvent(MouseButton button, Point pos);
		virtual EVENT void MouseWheeledEvent(MouseWheel wheel, Point pos);
		virtual EVENT void WindowResizedEvent(Point size);
	};
	class EventHandler
	{
	private:
		std::shared_future<void> thread;
	public:
		virtual void onMouseEvent(MOUSE_EVENT_RECORD e) {};
		virtual void onKeyEvent(KEY_EVENT_RECORD e) {};
		virtual void onKeyPressedEvent(WORD key) {};
		virtual void onKeyReleasedEvent(WORD key) {};
		virtual void onMouseMoveEvent(Point pos) {};
		virtual void onMousePressedEvent(MouseButton button, Point pos) {};
		virtual void onMouseDoubleClickEvent(MouseButton button, Point pos) {};
		virtual void onMouseWheeledEvent(MouseWheel wheel, Point pos) {};
		virtual void onWindowResizedEvent(Point size) {};
		void loop() { while (true) {} };
		EventHandler();
		~EventHandler();
	};
}

namespace io
{

	class Element
	{
	public:
		Region region;
		void(*onclick)(Point);
		virtual void draw() {};
	};

	static std::list<Element*> elements = {};

	class IoEventHandler : public events::EventHandler
	{
	public:
		void onMousePressedEvent(events::MouseButton button, Point pos);
	};

	static IoEventHandler ioHandler = IoEventHandler();

	class Button : public Element
	{
	public:
		void draw() override
		{
			console::drawRegion(this->region, graphicBrush);
		}
		Button(Region _region, BrushEx brush);
		~Button();
	private:
		BrushEx graphicBrush;
	};
}