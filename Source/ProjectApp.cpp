#define SAFFRON_ENTRY_POINT
#include "ProjectApp.h"

namespace Se
{
auto CreateApplication() -> Unique<App>
{
	return CreateUnique<ProjectApp>(AppProperties::CreateCentered("Fractals", 1000, 700));
}

ProjectApp::ProjectApp(const AppProperties& properties) :
	App(properties),
	_projectLayer(CreateShared<ProjectLayer>())
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
