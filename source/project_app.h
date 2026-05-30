#pragma once

#include <memory>

#include <saffron.h>

#include "layers/project_layer.h"

namespace fractals
{
using namespace saffron;
class ProjectApp : public App
{
public:
	explicit ProjectApp(const AppProperties& properties);

	void OnInit() override;

	void OnUpdate() override;

private:
	std::shared_ptr<ProjectLayer> _projectLayer;
};
}