#pragma once

#define VEWND_EXCEPT(errorCode) Window::Exception(__LINE__, __FILE__, (const char *)(errorCode))
#define VEWND_LAST_EXCEPT() VEWND_EXCEPT(SDL_GetError())