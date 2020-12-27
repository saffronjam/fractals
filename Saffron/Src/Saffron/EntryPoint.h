#pragma once

#include "Saffron/Core/Core.h"

#ifdef SE_PLATFORM_WINDOWS

extern Se::Application *Se::CreateApplication();

int main(int argc, char **argv)
{
	Se::Core::Init();
	Se::Application *app = Se::CreateApplication();
	SE_CORE_ASSERT(app, "Client Application is null!");
	app->Run();
	delete app;
	Se::Core::Shutdown();
}

#endif
