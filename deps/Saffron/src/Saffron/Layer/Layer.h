#pragma once

#include <SFML/Window/Event.hpp>

#include "Saffron/Core/BatchLoader.h"

namespace Se {

class Layer
{
public:
	Layer(const String &name = "Layer");
	virtual ~Layer() = default;

	virtual void OnAttach(std::shared_ptr<BatchLoader> &loader) {}
	virtual void OnDetach() {}

	virtual void OnPreFrame() {}
	virtual void OnPostFrame() {}

	virtual void OnUpdate() {}
	virtual void OnGuiRender() {}

	virtual void OnEvent(const sf::Event &event) {}

	const String &GetName() const { return _debugName; }
protected:
	String _debugName;
};

}