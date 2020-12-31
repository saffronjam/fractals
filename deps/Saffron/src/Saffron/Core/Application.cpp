#include "SaffronPCH.h"

#include "Saffron/Core/Application.h"
#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Core/Global.h"
#include "Saffron/Core/Run.h"
#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Graphics/Scene.h"
#include "Saffron/Graphics/RenderTargetManager.h"

namespace Se {

Application *Application::s_Instance = nullptr;


Application::Application(const Properties &properties)
	: _preLoader(CreateShared<BatchLoader>("Preloader")),
	_window(properties.Name, properties.WindowWidth, properties.WindowHeight)
{
	SE_ASSERT(!s_Instance, "Application already exist");
	s_Instance = this;

	SE_CORE_INFO("--- Saffron 2D Framework ---");
	SE_CORE_INFO("Creating application {0}", properties.Name);

	_window.SetEventCallback([this](const sf::Event &event) {OnEvent(event); });

	FileIOManager::Init(_window);
    Gui::Init(Filepath("../../../imgui.ini"));

	_preLoader->Submit([]
					   {
						   Gui::SetStyle(Gui::Style::Dark);
					   }, "Initializing GUI");

	Global::Clock::Restart();
}

Application::~Application()
{
	Gui::Shutdown();
	SE_CORE_INFO("Shutting down");
}

void Application::PushLayer(std::shared_ptr<Layer> layer)
{
	_layerStack.PushLayer(layer, _preLoader);
}

void Application::PushOverlay(std::shared_ptr<Layer> overlay)
{
	_layerStack.PushOverlay(overlay, _preLoader);
}

void Application::PopLayer(int count)
{
	_layerStack.PopLayer(count);
}

void Application::PopOverlay(int count)
{
	_layerStack.PopOverlay(count);
}

void Application::EraseLayer(std::shared_ptr<Layer> layer)
{
	_layerStack.EraseLayer(layer);
}

void Application::EraseOverlay(std::shared_ptr<Layer> overlay)
{
	_layerStack.EraseOverlay(overlay);
}

void Application::RenderGui()
{
	Gui::Begin();

	for ( const auto &layer : _layerStack )
	{
		layer->OnGuiRender();
	}

	Gui::End();
}

void Application::Run()
{
	OnInit();

	while ( _running )
	{
		Global::Clock::Restart();
		_window.HandleBufferedEvents();
		_window.Clear();
		RenderTargetManager::ClearAll();
		if ( !_minimized )
		{
			for ( const auto &layer : _layerStack )
			{
				layer->OnUpdate();
			}
			RenderGui();
			Keyboard::OnUpdate();
			Mouse::OnUpdate();
		}
		OnUpdate();
		Run::Execute();
		RenderTargetManager::DisplayAll();
		_window.Display();
	}

	_layerStack.Clear();

	OnShutdown();
}

void Application::Exit()
{
	_preLoader->ForceExit();
	_running = false;
}

void Application::OnGuiRender()
{
	if ( ImGui::Begin("Stats") )
	{
		const auto dt = Global::Clock::GetFrameTime();
		_fpsTimer += dt;
		if ( _fpsTimer.asSeconds() < 1.0f )
		{
			_storedFrameCount++;
			_storedFrametime += dt;
		}
		else
		{
			_cachedFps = static_cast<int>(static_cast<float>(_storedFrameCount) / _storedFrametime.asSeconds());
			_cachedSpf = sf::seconds(_storedFrametime.asSeconds() / static_cast<float>(_storedFrameCount));
			_storedFrameCount = 0;
			_storedFrametime = sf::Time::Zero;
			_fpsTimer = sf::Time::Zero;
		}

		Gui::BeginPropertyGrid();

		Gui::Property("Vendor", "SFML v.2.5.2");
		Gui::Property("Frametime", std::to_string(_cachedSpf.asMicroseconds() / 1000.0f) + " ms");
		Gui::Property("FPS", std::to_string(_cachedFps));

		Gui::EndPropertyGrid();
	}
	ImGui::End();
}

void Application::OnEvent(const sf::Event &event)
{
	if ( event.type == event.Closed )
	{
		OnWindowClose();
	}

	Gui::OnEvent(event);
	Keyboard::OnEvent(event);
	Mouse::OnEvent(event);

	for ( auto it = _layerStack.end(); it != _layerStack.begin(); )
	{
		(*--it)->OnEvent(event);
	}
}

bool Application::OnWindowClose()
{
	Exit();
	return true;
}

String Application::GetConfigurationName()
{
#if defined(SE_DEBUG)
	return "Debug";
#elif defined(SE_RELEASE)
	return "Release";
#elif defined(SE_DIST)
	return "Dist";
#else
#error Undefined configuration?
#endif
}

String Application::GetPlatformName()
{
#if defined(SE_PLATFORM_WINDOWS)
	return "Windows x64";
#elif defined(SE_PLATFORM_LINUX)
    return "Linux x64";
#endif
}
}
