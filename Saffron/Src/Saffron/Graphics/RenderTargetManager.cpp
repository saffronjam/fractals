#include "SaffronPCH.h"

#include "Saffron/Graphics/RenderTargetManager.h"

namespace Se
{
ArrayList<ControllableRenderTexture *> RenderTargetManager::_targets;

void RenderTargetManager::Add(ControllableRenderTexture *target)
{
	_targets.push_back(target);
}

void RenderTargetManager::Remove(ControllableRenderTexture *target)
{
	const auto result = std::find(_targets.begin(), _targets.end(), target);
	if ( result != _targets.end() )
	{
		_targets.erase(result);
	}
}

void RenderTargetManager::ClearAll()
{
	for ( auto *target : _targets )
	{
		if ( target->IsEnabled() )
		{
			target->GetRenderTexture().clear(target->GetClearColor());
		}
	}
}

void RenderTargetManager::DisplayAll()
{
	for ( auto *target : _targets )
	{
		if ( target->IsEnabled() )
		{
			target->GetRenderTexture().display();
		}
	}
}
}
