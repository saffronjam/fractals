#pragma once

#include <imgui.h>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

#include "Saffron/Base.h"
#include "Saffron/Math/SaffronMath.h"

namespace Se
{
using Font = ImFont;

class Gui
{
public:
	enum class Style : int
	{
		Dark = 0, Light = 1
	};

	typedef Uint32 PropertyFlag;
	enum PropertyFlag_ : Uint32
	{
		PropertyFlag_None = 0u,
		PropertyFlag_Color = 1u << 4u,
		PropertyFlag_Drag = 1u << 5u,
		PropertyFlag_Slider = 1u << 6u,
		PropertyFlag_Logarithmic = 1u << 7u
	};

public:
	static void Init(Filepath iniFilepath);

	static void Shutdown();

	static void OnEvent(const sf::Event &event);

	static void Begin();

	static void End();

	static void BeginPropertyGrid(String id = "", float width = -1.0);

	static void EndPropertyGrid();

	static bool BeginTreeNode(const String &name, bool defaultOpen = true);

	static void EndTreeNode();

	////////////////////////////////
	/// Bool and string
	////////////////////////////////

	static void Property(const String &name, const Function<void()> &onClick, bool secondColumn = false);

	static void Property(const String &name, const String &value);

	static bool Property(const String &name, String &value);

	static bool Property(const String &name, bool &value);

	static bool Property(const String &name, const String &text,
						 const String &buttonName, const Function<void()> &onButtonPress);

	////////////////////////////////
	/// Single value
	////////////////////////////////

	static bool Property(const String &name, int &value, int min = -1, int max = 1, float step = 1,
						 PropertyFlag flags = PropertyFlag_None);

	static bool Property(const String &name, int &value, const char *format,
						 int min = -1, int max = 1, float step = 1,
						 PropertyFlag flags = PropertyFlag_None);

	static bool Property(const String &name, float &value, float min = -1.0f, float max = 1.0f, float step = 1.0f,
						 PropertyFlag flags = PropertyFlag_None);

	static bool Property(const String &name, float &value, const char *format,
						 float min = -1.0f, float max = 1.0f, float step = 1.0f,
						 PropertyFlag flags = PropertyFlag_None);

	////////////////////////////////
	/// Vector2
	////////////////////////////////

	static bool Property(const String &name, sf::Vector2f &value, PropertyFlag flags);

	static bool Property(const String &name, sf::Vector2f &value,
						 float min = -1.0f, float max = 1.0f, float step = 1.0f,
						 PropertyFlag flags = PropertyFlag_None);

	static bool Property(const String &name, sf::Vector2f &value, const char *format,
						 float min = -1.0f, float max = 1.0f, float step = 1.0f,
						 PropertyFlag flags = PropertyFlag_None);

	////////////////////////////////
	/// Vector3
	////////////////////////////////

	static bool Property(const String &name, sf::Vector3f &value, PropertyFlag flags);

	static bool Property(const String &name, sf::Vector3f &value,
						 float min = -1.0f, float max = 1.0f, float step = 1.0f,
						 PropertyFlag flags = PropertyFlag_None,
						 Optional <std::function<void()>> fn = {});

	static bool Property(const String &name, sf::Vector3f &value, const char *format,
						 float min = -1.0f, float max = 1.0f, float step = 1.0f,
						 PropertyFlag flags = PropertyFlag_None,
						 Optional <std::function<void()>> fn = {});

	////////////////////////////////
	/// Vector4
	////////////////////////////////

	static bool Property(const String &name, sf::Vector4f &value, PropertyFlag flags);

	static bool Property(const String &name, sf::Vector4f &value,
						 float min = -1.0f, float max = 1.0f, float step = 1.0f,
						 PropertyFlag flags = PropertyFlag_None);

	static bool Property(const String &name, sf::Vector4f &value, const char *format,
						 float min = -1.0f, float max = 1.0f, float step = 1.0f,
						 PropertyFlag flags = PropertyFlag_None);


	static void Image(const sf::Texture &texture, const sf::Color &tintColor = sf::Color::White,
					  const sf::Color &borderColor = sf::Color::Transparent);

	static void Image(const sf::Texture &texture, const sf::Vector2f &size,
					  const sf::Color &tintColor = sf::Color::White,
					  const sf::Color &borderColor = sf::Color::Transparent);

	static void Image(const sf::Texture &texture, const sf::FloatRect &uvRect,
					  const sf::Color &tintColor = sf::Color::White,
					  const sf::Color &borderColor = sf::Color::Transparent);

	static void Image(const sf::Texture &texture, const sf::Vector2f &size, const sf::FloatRect &uvRect,
					  const sf::Color &tintColor = sf::Color::White,
					  const sf::Color &borderColor = sf::Color::Transparent);

	static void Image(const sf::Texture &texture, const sf::Vector2f &size,
					  const sf::Vector2f &uv0, const sf::Vector2f &uv1,
					  const sf::Color &tintColor = sf::Color::White,
					  const sf::Color &borderColor = sf::Color::Transparent);

	static void Image(const sf::Sprite &sprite, const sf::Color &tintColor = sf::Color::White,
					  const sf::Color &borderColor = sf::Color::Transparent);

	static void Image(const sf::Sprite &sprite, const sf::Vector2f &size, const sf::Color &tintColor = sf::Color::White,
					  const sf::Color &borderColor = sf::Color::Transparent);

	// ImageButton overloads
	static bool ImageButton(const sf::Texture &texture, int framePadding = -1,
							const sf::Color &bgColor = sf::Color::Transparent,
							const sf::Color &tintColor = sf::Color::White);

	static bool ImageButton(const sf::Texture &texture, const sf::Vector2f &size, int framePadding = -1,
							const sf::Color &bgColor = sf::Color::Transparent,
							const sf::Color &tintColor = sf::Color::White);

	static bool ImageButton(const sf::Sprite &sprite, int framePadding = -1,
							const sf::Color &bgColor = sf::Color::Transparent,
							const sf::Color &tintColor = sf::Color::White);

	static bool ImageButton(const sf::Sprite &sprite, const sf::Vector2f &size, int framePadding = -1,
							const sf::Color &bgColor = sf::Color::Transparent,
							const sf::Color &tintColor = sf::Color::White);

	static void HelpMarker(const String &desc);

	static void InfoModal(const char *title, const char *text, bool &open);

	static int GetFontSize();

	static void SetStyle(Style style);

	static void SetFontSize(int size);

	static Font *AddFont(const Filepath &path, int size);

	static void ForceHideBarTab();

	static sf::Vector4f GetSaffronOrange(float opacity = 1.0f);

	static sf::Vector4f GetSaffronPurple(float opacity = 1.0f);

private:
	static void PushID();

	static void PopID();

	static Font *GetAppropriateFont(int size);

	static ImGuiSliderFlags GetImGuiSliderFlags(PropertyFlag flags);

private:
	static Filepath _iniFilepath;
	static Style _currentStyle;
	static Map<int, ImFont *> _fonts;
	static Pair<int, ImFont *> _currentFont;

};
}