#pragma once

#define VEAPP_EXCEPT(errorCode) throw IApp::Exception(__LINE__, __FILE__, (const char *)(errorCode))
#define VEAPP_LAST_EXCEPT() VEAPP_EXCEPT(SDL_GetError())