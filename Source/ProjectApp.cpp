#define SAFFRON_ENTRY_POINT
#include "ProjectApp.h"

namespace Se
{
auto CreateApplication() -> std::unique_ptr<App>
{
	return std::make_unique<ProjectApp>(AppProperties::CreateFullscreen("Fractals"));
}

ProjectApp::ProjectApp(const AppProperties& properties) :
	App(properties),
	_projectLayer(std::make_shared<ProjectLayer>())
{
}

void ProjectApp::OnInit()
{
	PushLayer(_projectLayer);
}

void ProjectApp::OnUpdate()
{
	App::OnUpdate();
}
}
