#include "SaffronPCH.h"

#include "Saffron/Gui/ViewportPane.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/Libraries/GenUtils.h"

namespace Se
{
SignalAggregate<void> ViewportPane::Signals::OnPostRender;
SignalAggregate<const sf::Vector2f &> ViewportPane::Signals::OnWantRenderTargetResize;

ViewportPane::ViewportPane(String windowTitle, const ControllableRenderTexture &target) :
	_windowTitle(Move(windowTitle)),
	_target(&target),
	//_fallbackTexture(Factory::Create<Texture2D>("Resources/Assets/Editor/FallbackViewportPaneTexture.png")),
	_topLeft(0.0f, 0.0f),
	_bottomRight(100.0f, 100.0f),
	_hovered(false),
	_focused(false)
{
}

void ViewportPane::OnGuiRender(bool *open, UUID uuid)
{
	const auto &tl = GetTopLeft();
	const auto &br = GetBottomRight();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	OutputStringStream oss;
	oss << _windowTitle << "##" << uuid;

	ImGui::Begin(oss.str().c_str(), open);

	_dockID = ImGui::GetWindowDockID();

	/*if ( ImGui::IsWindowDocked() )
	{
		auto *wnd = ImGui::FindWindowByName(_windowTitle.c_str());
		if ( wnd )
		{
			ImGuiDockNode *node = wnd->DockNode;
			if ( node && !node->IsHiddenTabBar() )
			{
				node->WantHiddenTabBarToggle = true;
			}
		}
	}*/

	_hovered = ImGui::IsWindowHovered();
	_focused = ImGui::IsWindowFocused();

	const auto viewportOffset = ImGui::GetCursorPos(); // includes tab bar
	ImVec2 minBound = ImGui::GetWindowPos();
	minBound.x += viewportOffset.x;
	minBound.y += viewportOffset.y;

	const auto windowSize = ImGui::GetWindowSize();
	const ImVec2 maxBound = { minBound.x + windowSize.x - viewportOffset.x, minBound.y + windowSize.y - viewportOffset.y };
	_topLeft = { minBound.x, minBound.y };
	_bottomRight = { maxBound.x, maxBound.y };

	const auto viewportSize = GetViewportSize();

	if ( _target->IsEnabled() )
	{
		Gui::Image(*_target, { viewportSize.x, viewportSize.y }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
	}
	else
	{
		Gui::Image(_fallbackTexture, { viewportSize.x, viewportSize.y }, sf::FloatRect{ 0.0f, 0.0f, 1.0f, 1.0f });
	}

	ImGui::GetWindowDrawList()->AddRect(ImVec2(_topLeft.x, tl.y), ImVec2(br.x, br.y), _focused ? IM_COL32(255, 140, 0, 180) : IM_COL32(255, 140, 0, 80), 0.0f, ImDrawCornerFlags_All, 4);

	GetSignals().Emit(Signals::OnPostRender);

	ImGui::End();
	ImGui::PopStyleVar();

	if ( GenUtils::ConvertTo<unsigned int>(viewportSize) != _target->GetRenderTexture().getSize() )
	{
		GetSignals().Emit(Signals::OnWantRenderTargetResize, viewportSize);
	}
}

bool ViewportPane::InViewport(sf::Vector2f positionNDC) const
{
	positionNDC.x -= _topLeft.x;
	positionNDC.y -= _topLeft.y;
	return positionNDC.x < _bottomRight.x &&positionNDC.y < _bottomRight.y;
}

sf::Vector2f ViewportPane::GetMousePosition(bool normalized) const
{
	sf::Vector2f position = Mouse::GetPosition(false);
	position.x -= _topLeft.x;
	position.y -= _topLeft.y;

	if ( normalized )
	{
		const auto viewportWidth = _bottomRight.x - _topLeft.x;
		const auto viewportHeight = _bottomRight.y - _topLeft.y;
		return { position.x / viewportWidth * 2.0f - 1.0f, (position.y / viewportHeight * 2.0f - 1.0f) * -1.0f };
	}
	return { position.x, position.y };
}

sf::Vector2f ViewportPane::GetViewportSize() const
{
	return GetBottomRight() - GetTopLeft();
}
}
