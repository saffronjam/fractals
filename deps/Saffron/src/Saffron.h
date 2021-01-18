//
// Note:	this file is to be included in client applications ONLY
//			NEVER include this file anywhere in the engine codebase
//
#pragma once

#include "Saffron/Base.h"

#include "Saffron/Core/Application.h"
#include "Saffron/Core/Global.h"
#include "Saffron/Core/Voronoi.h"

#include "Saffron/Graphics/ControllableRenderTexture.h"
#include "Saffron/Graphics/RenderTargetManager.h"
#include "Saffron/Graphics/Scene.h"

#include "Saffron/Gui/DockSpace.h"
#include "Saffron/Gui/Terminal.h"
#include "Saffron/Gui/ViewportPane.h"

#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"

#include "Saffron/Libraries/GenUtils.h"
#include "Saffron/Libraries/VecUtils.h"

#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Math/Random.h"

#include "Saffron/Resource/FontStore.h"
#include "Saffron/Resource/ImageStore.h"
#include "Saffron/Resource/MusicStore.h"
#include "Saffron/Resource/SoundBufferStore.h"
#include "Saffron/Resource/TextureStore.h"

#ifdef SAFFRON_ENTRY_POINT
#include <Saffron/EntryPoint.h>
#endif