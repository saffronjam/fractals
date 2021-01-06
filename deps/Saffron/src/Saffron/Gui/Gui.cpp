#include "SaffronPCH.h"

#include "Saffron/Core/Application.h"
#include "Saffron/Core/Global.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Gui/GuiImpl.h"

#include <imgui_internal.h>

namespace Se
{

Filepath Gui::_iniFilepath;
Gui::Style Gui::_currentStyle = Style::Light;
Map<int, ImFont *> Gui::_fonts;
Pair<int, ImFont *> Gui::_currentFont;

static int s_UIContextID = 0;

void Gui::Init(Filepath iniFilepath)
{
	_iniFilepath = Move(iniFilepath);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.IniFilename = "../../../imgui.ini";//reinterpret_cast<const char*>(_iniFilepath.c_str());
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge

	auto *newFont = AddFont("res/Fonts/segoeui.ttf", 18);
	io.FontDefault = newFont;
	_currentFont = { 18, newFont };

	AddFont("res/Fonts/segoeui.ttf", 8);
	AddFont("res/Fonts/segoeui.ttf", 12);
	AddFont("res/Fonts/segoeui.ttf", 14);
	AddFont("res/Fonts/segoeui.ttf", 20);
	AddFont("res/Fonts/segoeui.ttf", 22);
	AddFont("res/Fonts/segoeui.ttf", 24);
	AddFont("res/Fonts/segoeui.ttf", 28);
	AddFont("res/Fonts/segoeui.ttf", 32);
	AddFont("res/Fonts/segoeui.ttf", 56);
	AddFont("res/Fonts/segoeui.ttf", 72);

	Application &app = Application::Get();
	auto &window = app.GetWindow().GetNativeWindow();
	GuiImpl::SFML::Init(window);

	ImGui::StyleColorsDark();
	ImGuiStyle &style = ImGui::GetStyle();
	if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		style.WindowRounding = 0.0f;
	}

	// Main Blue
	//const sf::Vector3f mainVibrant = { 0.26f, 0.59f, 0.98f };
	//const sf::Vector3f mainVibrantDark = { 0.24f, 0.52f, 0.88f };
	//const sf::Vector3f mainLessVibrant = { 0.06f, 0.53f, 0.98f

	// Main Orange
	//const sf::Vector3f mainVibrant = { 0.89f, 0.46f, 0.16f };
	//const sf::Vector3f mainVibrantDark = { 0.79f, 0.38f, 0.14f };
	//const sf::Vector3f mainLessVibrant = { 0.89f, 0.39f, 0.02f };

	// Main Purple
	const sf::Vector3f mainVibrant = { 0.29f, 0.13f, 0.42f };
	const sf::Vector3f mainVibrantDark = { 0.19f, 0.15f, 0.23f };
	const sf::Vector3f mainLessVibrant = { 0.33f, 0.18f, 0.48f };

	const sf::Vector4f mainNoTint = { mainVibrant, 1.00f }; //3	Main no tint
	const sf::Vector4f mainTint1 = { mainVibrant, 0.95f }; //9	Main tinted1
	const sf::Vector4f mainTint2 = { mainVibrant, 0.80f }; //8	Main tinted2
	const sf::Vector4f mainTint3 = { mainVibrant, 0.67f }; //2	Main tinted3
	const sf::Vector4f mainTint4 = { mainVibrant, 0.40f }; //1	Main tinted4
	const sf::Vector4f mainTint5 = { mainVibrant, 0.35f }; //13	Main tinted5

	const sf::Vector4f mainDark = { mainVibrantDark, 1.00f }; //4	Main dark1 no tint

	const sf::Vector4f mainLessVibrantNoTint = { mainLessVibrant, 1.00f }; //6	Less blue no tint
	const sf::Vector4f mainLessVibrantTint1 = { mainLessVibrant, 0.60f }; //14 Less blue tinted1

	const sf::Vector4f coMain = { 1.00f, 0.43f, 0.35f, 1.00f }; //10	2ndMain no tint
	const sf::Vector4f coMainDark = { 0.90f, 0.70f, 0.00f, 1.00f }; //11	3rdMain no tint
	const sf::Vector4f coMainRed = { 1.00f, 0.60f, 0.00f, 1.00f }; //12	Co3rdMain

	const auto ToImVec4 = [](const sf::Vector4f &vector)
	{
		return ImVec4{ vector.x, vector.y, vector.z, vector.w };
	};

	style.Alpha = 1.0f;
	style.FrameRounding = 3.0f;
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ToImVec4(mainTint4);                                    //1
	style.Colors[ImGuiCol_FrameBgActive] = ToImVec4(mainTint3);                                        //2
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ToImVec4(mainNoTint);                                        //3
	style.Colors[ImGuiCol_SliderGrab] = ToImVec4(mainDark);                                            //4
	style.Colors[ImGuiCol_SliderGrabActive] = ToImVec4(mainDark);                                    //4
	style.Colors[ImGuiCol_Button] = ToImVec4(mainTint2);                                            //1
	style.Colors[ImGuiCol_ButtonHovered] = ToImVec4(mainNoTint);                                    //3
	style.Colors[ImGuiCol_ButtonActive] = ToImVec4(mainLessVibrantNoTint);                            //6
	style.Colors[ImGuiCol_Header] = ToImVec4(mainTint4);                                            //7
	style.Colors[ImGuiCol_HeaderHovered] = ToImVec4(mainTint2);                                        //8
	style.Colors[ImGuiCol_HeaderActive] = ToImVec4(mainNoTint);                                        //3
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ToImVec4(mainTint3);                                    //2
	style.Colors[ImGuiCol_ResizeGripActive] = ToImVec4(mainTint1);                                    //9
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ToImVec4(coMain);                                        //10
	style.Colors[ImGuiCol_PlotHistogram] = ToImVec4(coMainDark);                                    //11
	style.Colors[ImGuiCol_PlotHistogramHovered] = ToImVec4(coMainRed);                                //12
	style.Colors[ImGuiCol_TextSelectedBg] = ToImVec4(mainTint5);                                    //13
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	style.Colors[ImGuiCol_Tab] = ToImVec4(mainTint4);                                                //1
	style.Colors[ImGuiCol_TabHovered] = ToImVec4(mainNoTint);                                        //3
	style.Colors[ImGuiCol_TabActive] = ToImVec4(mainLessVibrantNoTint);                                //6
	style.Colors[ImGuiCol_TabUnfocused] = ToImVec4(mainTint4);                                        //1
	style.Colors[ImGuiCol_TabUnfocusedActive] = ToImVec4(mainLessVibrantNoTint);                        //14

	SetStyle(Style::Dark);
}

void Gui::Shutdown()
{
	GuiImpl::SFML::Shutdown();
}

void Gui::OnEvent(const sf::Event &event)
{
	GuiImpl::SFML::ProcessEvent(event);
}

void Gui::Begin()
{
	GuiImpl::SFML::Update(Application::Get().GetWindow().GetNativeWindow(), Global::Clock::GetFrameTime());
}

void Gui::End()
{
	ImGuiIO &io = ImGui::GetIO();

	GuiImpl::SFML::Render(Application::Get().GetWindow().GetNativeWindow());

	if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void Gui::BeginPropertyGrid(String id, float width)
{
	PushID();
	ImGui::Columns(2, id.length() ? id.c_str() : nullptr);
	ImGui::AlignTextToFramePadding();
}

void Gui::EndPropertyGrid()
{
	ImGui::Columns(1);
	PopID();
}

bool Gui::BeginTreeNode(const String &name, bool defaultOpen)
{
	ImGuiTreeNodeFlags treeNodeFlags =
		ImGuiTreeNodeFlags_Framed |
		ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_FramePadding;
	if ( defaultOpen )
		treeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

	return ImGui::TreeNodeEx(name.c_str(), treeNodeFlags);
}

void Gui::EndTreeNode()
{
	ImGui::TreePop();
}

void Gui::Property(const String &name, const Function<void()> &onClick, bool secondColumn)
{
	if ( secondColumn )
	{
		ImGui::NextColumn();
	}

	const String id = name + "##" + name;
	if ( ImGui::Button(id.c_str(), { ImGui::GetContentRegionAvailWidth(), 0 }) )
	{
		onClick();
	}

	if ( !secondColumn )
	{
		ImGui::NextColumn();
	}
	ImGui::NextColumn();
}

void Gui::Property(const String &name, const String &value)
{
	ImGui::Text("%s", name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const String id = "##" + name;
	ImGui::InputText(id.c_str(), const_cast<char *>(value.c_str()), 256, ImGuiInputTextFlags_ReadOnly);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
}

bool Gui::Property(const String &name, String &value)
{
	ImGui::Text("%s", name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	char buffer[256];
	strcpy(buffer, value.c_str());

	const String id = "##" + name;
	bool changed = false;
	if ( ImGui::InputText(id.c_str(), buffer, 256) )
	{
		value = buffer;
		changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String &name, bool &value)
{
	ImGui::Text("%s", name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const String id = "##" + name;
	const bool result = ImGui::Checkbox(id.c_str(), &value);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return result;
}

bool Gui::Property(const String &name, const String &text, const String &buttonName,
				   const Function<void()> &onButtonPress)
{
	ImGui::Text("%s", name.c_str());
	ImGui::NextColumn();

	const auto minButtonWidth = ImGui::CalcTextSize(buttonName.c_str()).x + 8.0f;
	const auto textBoxWidth = ImGui::GetContentRegionAvailWidth() - minButtonWidth;

	if ( textBoxWidth > 0.0f )
	{
		ImGui::PushItemWidth(textBoxWidth);
		char buffer[256];
		strcpy(buffer, text.c_str());
		const String id = "##" + name;
		ImGui::InputText(id.c_str(), const_cast<char *>(text.c_str()), 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();
		ImGui::SameLine();
	}

	bool changed = false;
	const auto contentRegionAvailable = ImGui::GetContentRegionAvailWidth();
	if ( contentRegionAvailable > 0.0f )
	{
		if ( ImGui::Button(buttonName.c_str(), { ImGui::GetContentRegionAvailWidth(), 0.0f }) )
		{
			if ( onButtonPress )
				onButtonPress();
			changed = true;
		}
	}

	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String &name, int &value, int min, int max, float step, PropertyFlag flags)
{
	return Property(name, value, "%d", min, max, step, flags);
}

bool Gui::Property(const String &name, int &value, const char *format,
				   int min, int max, float step, Gui::PropertyFlag flags)
{
	ImGui::Text("%s", name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const auto imGuiFlags = GetImGuiSliderFlags(flags);

	const String id = "##" + name;
	bool changed = false;
	if ( flags & PropertyFlag_Slider )
	{
		changed = ImGui::SliderInt(id.c_str(), &value, min, max, format, imGuiFlags);
	}
	else if ( flags & PropertyFlag_Drag )
	{
		changed = ImGui::DragInt(id.c_str(), &value, step, min, max, format, imGuiFlags);
	}

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String &name, float &value, float min, float max, float step, PropertyFlag flags)
{
	return Property(name, value, "%.3f", min, max, step, flags);
}

bool Gui::Property(const String &name, float &value, const char *format,
				   float min, float max, float step, Gui::PropertyFlag flags)
{
	ImGui::Text("%s", name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const auto imGuiFlags = GetImGuiSliderFlags(flags);

	const String id = "##" + name;
	bool changed = false;
	if ( flags & PropertyFlag_Slider )
	{
		changed = ImGui::SliderFloat(id.c_str(), &value, min, max, format, imGuiFlags);
	}
	else if ( flags & PropertyFlag_Drag )
	{
		changed = ImGui::DragFloat(id.c_str(), &value, step, min, max, format, imGuiFlags);
	}

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String &name, sf::Vector2f &value, Gui::PropertyFlag flags)
{
	return Property(name, value, -1.0f, 1.0f, 1.0f, flags);
}

bool Gui::Property(const String &name, sf::Vector2f &value, float min, float max, float step, PropertyFlag flags)
{
	return Property(name, value, "%.03f", min, max, step, flags);
}

bool Gui::Property(const String &name, sf::Vector2f &value, const char *format,
				   float min, float max, float step, Gui::PropertyFlag flags)
{
	ImGui::Text("%s", name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const auto imGuiFlags = GetImGuiSliderFlags(flags);

	const String id = "##" + name;
	bool changed = false;
	if ( flags & PropertyFlag_Slider )
	{
		changed = ImGui::SliderFloat2(id.c_str(), reinterpret_cast<float *>(&value),
									  min, max, format, imGuiFlags);
	}
	else if ( flags & PropertyFlag_Drag )
	{
		changed = ImGui::DragFloat2(id.c_str(), reinterpret_cast<float *>(&value),
									step, min, max, format, imGuiFlags);
	}

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String &name, sf::Vector3f &value, PropertyFlag flags)
{
	return Property(name, value, -1.0f, 1.0f, 1.0f, flags);
}

bool Gui::Property(const String &name, sf::Vector3f &value, float min, float max, float step, Gui::PropertyFlag flags,
				   Optional<std::function<void()>> fn)
{
	return Property(name, value, "%.3f", min, max, step, flags);
}

bool Gui::Property(const String &name, sf::Vector3f &value, const char *format,
				   float min, float max, float step, Gui::PropertyFlag flags, Optional<std::function<void()>> fn)
{
	ImGui::Text("%s", name.c_str());
	ImGui::NextColumn();

	const auto imGuiFlags = GetImGuiSliderFlags(flags);

	if ( fn.has_value() )
	{
		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.75f);
	}
	else
	{
		ImGui::PushItemWidth(-1);
	}

	const String id = "##" + name;
	bool changed = false;
	if ( flags & PropertyFlag_Color )
	{
		changed = ImGui::ColorEdit3(id.c_str(), reinterpret_cast<float *>(&value), ImGuiColorEditFlags_NoInputs);
	}
	else if ( flags & PropertyFlag_Slider )
	{
		changed = ImGui::SliderFloat3(id.c_str(), reinterpret_cast<float *>(&value), min, max, format, imGuiFlags);
	}
	else if ( flags & PropertyFlag_Drag )
	{
		changed = ImGui::DragFloat3(id.c_str(), reinterpret_cast<float *>(&value), step, min, max, format, imGuiFlags);
	}

	if ( fn.has_value() )
	{
		const String buttonID = "<" + id + "##fn";
		ImGui::SameLine();
		if ( ImGui::Button(buttonID.c_str(), { ImGui::GetContentRegionAvailWidth(), 0.0f }) )
		{
			fn.value()();
			changed = false;
		}
	}

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String &name, sf::Vector4f &value, PropertyFlag flags)
{
	return Property(name, value, -1.0f, 1.0f, 1.0f, flags);
}

bool Gui::Property(const String &name, sf::Vector4f &value, float min, float max, float step, PropertyFlag flags)
{
	return Property(name, value, "%.3f", min, max, step, flags);
}

bool Gui::Property(const String &name, sf::Vector4f &value, const char *format,
				   float min, float max, float step, Gui::PropertyFlag flags)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const auto imGuiFlags = GetImGuiSliderFlags(flags);

	const String id = "##" + name;
	bool changed = false;
	if ( flags & PropertyFlag_Color )
	{
		changed = ImGui::ColorEdit4(id.c_str(), reinterpret_cast<float *>(&value), ImGuiColorEditFlags_NoInputs);
	}
	else if ( flags & PropertyFlag_Slider )
	{
		changed = ImGui::SliderFloat4(id.c_str(), reinterpret_cast<float *>(&value), min, max, "%.3f", imGuiFlags);
	}
	else if ( flags & PropertyFlag_Drag )
	{
		changed = ImGui::DragFloat4(id.c_str(), reinterpret_cast<float *>(&value), step, min, max, "%.3f", imGuiFlags);
	}

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

void Gui::Image(const sf::Texture &texture, const sf::Color &tintColor, const sf::Color &borderColor)
{
	GuiImpl::Image(texture, tintColor, borderColor);
}

void Gui::Image(const sf::Texture &texture, const sf::Vector2f &size, const sf::Color &tintColor,
				const sf::Color &borderColor)
{
	GuiImpl::Image(texture, size, tintColor, borderColor);
}

void Gui::Image(const sf::Texture &texture, const sf::FloatRect &uvRect, const sf::Color &tintColor,
				const sf::Color &borderColor)
{
	GuiImpl::Image(texture, uvRect, tintColor, borderColor);
}

void Gui::Image(const sf::Texture &texture, const sf::Vector2f &size, const sf::FloatRect &uvRect,
				const sf::Color &tintColor, const sf::Color &borderColor)
{
	GuiImpl::Image(texture, size, uvRect, tintColor, borderColor);
}

void Gui::Image(const sf::Texture &texture, const sf::Vector2f &size, const sf::Vector2f &uv0, const sf::Vector2f &uv1,
				const sf::Color &tintColor, const sf::Color &borderColor)
{
	GuiImpl::Image(texture, size, uv0, uv1, tintColor, borderColor);
}

void Gui::Image(const sf::Sprite &sprite, const sf::Color &tintColor, const sf::Color &borderColor)
{
	GuiImpl::Image(sprite, tintColor, borderColor);
}

void Gui::Image(const sf::Sprite &sprite, const sf::Vector2f &size, const sf::Color &tintColor,
				const sf::Color &borderColor)
{
	GuiImpl::Image(sprite, size, tintColor, borderColor);
}

bool Gui::ImageButton(const sf::Texture &texture, int framePadding, const sf::Color &bgColor,
					  const sf::Color &tintColor)
{
	return GuiImpl::ImageButton(texture, framePadding, bgColor, tintColor);
}

bool
Gui::ImageButton(const sf::Texture &texture, const sf::Vector2f &size, int framePadding, const sf::Color &bgColor,
				 const sf::Color &tintColor)
{
	return GuiImpl::ImageButton(texture, size, framePadding, bgColor, tintColor);
}

bool
Gui::ImageButton(const sf::Sprite &sprite, int framePadding, const sf::Color &bgColor, const sf::Color &tintColor)
{
	return GuiImpl::ImageButton(sprite, framePadding, bgColor, tintColor);
}

bool
Gui::ImageButton(const sf::Sprite &sprite, const sf::Vector2f &size, int framePadding, const sf::Color &bgColor,
				 const sf::Color &tintColor)
{
	return GuiImpl::ImageButton(sprite, size, framePadding, bgColor, tintColor);
}

void Gui::HelpMarker(const String &desc)
{
	ImGui::TextDisabled("(?)");
	if ( ImGui::IsItemHovered() )
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc.c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void Gui::InfoModal(const char *title, const char *text, bool &open)
{
	const auto prevFontSize = GetFontSize();
	SetFontSize(20);
	if ( open && !ImGui::IsPopupOpen(title) )
	{
		ImGui::OpenPopup(title);
	}

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if ( ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize) )
	{
		ImGui::Text(text);
		if ( ImGui::Button("Dismiss") )
		{
			ImGui::CloseCurrentPopup();
		}
		open = false;
		ImGui::EndPopup();
	}
	SetFontSize(prevFontSize);
}

int Gui::GetFontSize()
{
	return _currentFont.first;
}

void Gui::SetStyle(Style style)
{
	ImGuiStyle &imguiStyle = ImGui::GetStyle();

	if ( _currentStyle != style )
	{
		for ( int i = 0; i <= ImGuiCol_COUNT; i++ )
		{
			ImVec4 &col = imguiStyle.Colors[i];
			float H, S, V;
			ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

			if ( S < 0.1f )
			{
				V = 1.0f - V;
			}
			ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
		}
		_currentStyle = style;
	}
}

void Gui::SetFontSize(int size)
{
	ImFont *candidate = GetAppropriateFont(size);
	SE_CORE_ASSERT(candidate, "Failed to fetch appropriate font and could be caused by an empty font container");
	ImGui::SetCurrentFont(candidate);
}

Font *Gui::AddFont(const Filepath &path, int size)
{
	auto *newFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.string().c_str(), static_cast<float>(size));
	_fonts.emplace(size, newFont);
	return newFont;
}

void Gui::ForceHideBarTab()
{
	if ( ImGui::IsWindowDocked() )
	{
		auto *node = ImGui::GetWindowDockNode();
		if ( node )
		{
			if ( node && !node->IsHiddenTabBar() )
			{
				node->WantHiddenTabBarToggle = true;
			}
		}
	}
}

sf::Vector4f Gui::GetSaffronOrange(float opacity)
{
	return sf::Vector4f(0.89f, 0.46f, 0.16f, opacity);
}

sf::Vector4f Gui::GetSaffronPurple(float opacity)
{
	return sf::Vector4f(0.29f, 0.13f, 0.42f, opacity);
}

void Gui::PushID()
{
	ImGui::PushID(s_UIContextID++);
}

void Gui::PopID()
{
	ImGui::PopID();
	s_UIContextID--;
}

Font *Gui::GetAppropriateFont(int size)
{
	ImFont *candidate = nullptr;
	int bestDiff = std::numeric_limits<int>::max();
	for ( auto &[fontSize, font] : _fonts )
	{
		if ( std::abs(fontSize - size) > bestDiff )
		{
			break;
		}
		bestDiff = std::abs(fontSize - size);
		candidate = font;
	}
	return candidate;
}

ImGuiSliderFlags Gui::GetImGuiSliderFlags(Gui::PropertyFlag flags)
{
	ImGuiSliderFlags imGuiSliderFlags = 0;
	if ( flags & PropertyFlag_Logarithmic )
	{
		imGuiSliderFlags |= ImGuiSliderFlags_Logarithmic;
	}
	return imGuiSliderFlags;
}
}

