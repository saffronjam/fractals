#pragma once

#include "Saffron/Base.h"
#include "Saffron/Graphics/ControllableRenderTexture.h"

namespace Se
{
class RenderTargetManager
{
public:
	static void Add(ControllableRenderTexture *target);

	static void Remove(ControllableRenderTexture *target);

	static void ClearAll();

	static void DisplayAll();

private:
	static ArrayList<ControllableRenderTexture *> _targets;

};
}
