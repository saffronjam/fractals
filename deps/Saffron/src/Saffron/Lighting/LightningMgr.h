#pragma once

#include <vector>

#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include "Saffron/Lighting/PointLight.h"
#include "Saffron/Libraries/VecUtils.h"
#include "Saffron/Core/Camera.h"

namespace Se
{
class LightningMgr
{
private:
	struct RenderLight
	{
		RenderLight(const Light::Ptr &light)
			: light(light),
			occluders(new sf::RenderTexture()),
			shadowMap(new sf::RenderTexture()),
			lightMap(new sf::RenderTexture())
		{
			const auto box = light->GetBoundingBox();
			occluders->create(box.width, box.height);
			shadowMap->create(box.width, 1);
			lightMap->create(box.width, box.height);
		}
		Light::Ptr light;
		sf::RenderTexture *occluders;
		sf::RenderTexture *shadowMap;
		sf::RenderTexture *lightMap;
	};

public:
	LightningMgr();
	~LightningMgr();
	LightningMgr(LightningMgr &) = delete;
	LightningMgr &operator()(LightningMgr &) = delete;

	static void Draw();

	static void AddOccluder(const sf::Drawable &drawable, sf::FloatRect boundingBox, sf::RenderStates renderStates = sf::RenderStates::Default);
	static void ClearOccluders();
	static void DisplayOccluders();

	static void Render();

	static void AddLight(const Light::Ptr &light);
	static void RemoveLight(const Light::Ptr &light);
	static void SetAmbientLight(const sf::Color &ambient) { _ambient = ambient; }

	static void ChangeResolution(float factor) { _resolution *= factor; }

	static float GetResolution() { return _resolution; }
	static void SetResolution(float resolution) { _resolution = resolution; }

private:
	static void RenderShadowMaps();
	static void RenderLightMaps();
	static void RenderCollectiveLightMap();

private:
	static std::vector<RenderLight> _renderLights;
	static sf::Shader _shadowMapShader;
	static sf::Shader _renderLightsShader;
	static sf::RenderTexture _collectiveLightMap;
	static sf::Color _ambient;

	static float _resolution;
};
}