#define SAFFRON_ENTRY_POINT
#include "ProjectApplication.h"

namespace Se
{
Application *CreateApplication()
{
	return new ProjectApplication({ "Saffron 2D", 1700, 720 });
}

ProjectApplication::ProjectApplication(const Properties &properties)
	: Application(properties),
	_projectLayer(new ProjectLayer)
{
}

void ProjectApplication::OnInit()
{
	PushLayer(_projectLayer);
}

void ProjectApplication::OnUpdate()
{
}
}
