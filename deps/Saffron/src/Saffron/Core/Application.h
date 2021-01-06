#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Window.h"
#include "Saffron/Layer/LayerStack.h"
#include "Saffron/Gui/FadePane.h"

namespace Se
{
class Scene;

class Application
{
	friend class ApplicationSerializer;

public:
	struct Properties
	{
		String Name;
		Uint32 WindowWidth, WindowHeight;
	};

public:
	explicit Application(const Properties &properties = { "Saffron 2D", 1280, 720 });
	virtual ~Application();

	void Run();
	void Exit();

	virtual void OnInit() {}
	virtual void OnShutdown() {}
	virtual void OnUpdate() {}
	virtual void OnGuiRender();
	virtual void OnEvent(const sf::Event &event);

	void PushLayer(std::shared_ptr<Layer> layer);
	void PushOverlay(std::shared_ptr<Layer> overlay);
	void PopLayer(int count = 1);
	void PopOverlay(int count = 1);
	void EraseLayer(std::shared_ptr<Layer> layer);
	void EraseOverlay(std::shared_ptr<Layer> overlay);

	Window &GetWindow() { return _window; }

	static Application &Get() { return *s_Instance; }

	static String GetConfigurationName();
	static String GetPlatformName();

private:
	bool OnWindowClose();

	void RunSplashScreen();

protected:
	std::shared_ptr<BatchLoader> _preLoader;

private:
	Window _window;
	bool _running = true, _minimized = false;
	LayerStack _layerStack;
	Mutex _finalPreloaderMessageMutex;

	sf::Time _fpsTimer = sf::Time::Zero;
	int _cachedFps = 0;
	sf::Time _cachedSpf = sf::Time::Zero;
	sf::Time _storedFrametime = sf::Time::Zero;
	int _storedFrameCount = 0;

	FadePane _fadeIn;

	static Application *s_Instance;
};

// Implemented by client
Application *CreateApplication();

}