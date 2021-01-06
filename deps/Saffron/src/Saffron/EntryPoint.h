#pragma once

#ifdef _MSC_VER
#pragma warning(disable: 4244)
#pragma warning(disable: 4715)
#endif

#include "Saffron/Core/Core.h"

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
