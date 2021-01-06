#include "SaffronPCH.h"

#include "Saffron/Core/Application.h"
#include "Saffron/Lighting/LightningMgr.h"

namespace Se
{
std::vector<LightningMgr::RenderLight> LightningMgr::_renderLights;

sf::Shader LightningMgr::_shadowMapShader;
sf::Shader LightningMgr::_renderLightsShader;
sf::Color LightningMgr::_ambient(0, 0, 0);

sf::RenderTexture LightningMgr::_collectiveLightMap;

float LightningMgr::_resolution = 4096;

LightningMgr::LightningMgr()
{
    _shadowMapShader.loadFromFile("Resources/Shaders/vertexShader.vert", "res/shaders/shadowMap.frag");
    _renderLightsShader.loadFromFile("Resources/Shaders/vertexShader.vert", "res/shaders/renderLights.frag");
    const auto &size = Application::Get().GetWindow().GetSize();
    _collectiveLightMap.create(size.x, size.y);
}

LightningMgr::~LightningMgr()
{
    for (auto &renderLight : _renderLights)
    {
        delete renderLight.occluders;
        delete renderLight.shadowMap;
        delete renderLight.lightMap;
        renderLight.occluders = nullptr;
        renderLight.shadowMap = nullptr;
        renderLight.lightMap = nullptr;
    }
}

void LightningMgr::Draw()
{
    //Window::Draw(sf::Sprite(_collectiveLightMap.getTexture()), sf::RenderStates(sf::BlendMultiply));
}

void
LightningMgr::AddOccluder(const sf::Drawable &drawable, sf::FloatRect boundingBox, sf::RenderStates renderStates)
{
    sf::RenderStates copy = renderStates;
    for (auto &renderLight : _renderLights)
    {
        if (boundingBox.intersects(renderLight.light->GetBoundingBox()))
        {
            const float radius = renderLight.light->GetRadius();
            sf::Vector2f position = renderLight.light->GetPosition();
            copy.transform.combine(sf::Transform().translate(sf::Vector2f(radius, radius) - position));
            renderLight.occluders->draw(drawable, copy);
            copy = renderStates;
        }
    }
}

void LightningMgr::ClearOccluders()
{
    for (auto &renderLight : _renderLights)
        renderLight.occluders->clear(sf::Color(0, 0, 0, 0));
}

void LightningMgr::DisplayOccluders()
{
    for (auto &renderLight : _renderLights)
    {
        renderLight.occluders->display();
    }
}

void LightningMgr::Render()
{
    RenderShadowMaps();
    RenderLightMaps();
    RenderCollectiveLightMap();
}

void LightningMgr::AddLight(const Light::Ptr &light)
{
    _renderLights.push_back(RenderLight(light));
}

void LightningMgr::RemoveLight(const Light::Ptr &light)
{
    const auto findRes = std::find_if(_renderLights.begin(), _renderLights.end(),
                                      [&light](const RenderLight &renderLight)
                                      {
                                          return renderLight.light == light;
                                      });
    if (findRes != _renderLights.end())
        _renderLights.erase(findRes);
}

void LightningMgr::RenderShadowMaps()
{
    /*const sf::FloatRect winRectAdjusted = Camera::ScreenToWorld(GenUtils::ConvertTo<float>(Application::Get().GetWindow().GetScreenRect()));
    for ( auto &renderLight : _renderLights )
    {
        if ( renderLight.light->GetBoundingBox().intersects(winRectAdjusted) )
        {
            _shadowMapShader.setUniform("resolution", _resolution);
            renderLight.shadowMap->clear(sf::Color(0, 0, 0, 0));
            renderLight.occluders->draw(sf::Sprite(renderLight.light->GetStaticOccluderMap().getTexture()));
            renderLight.shadowMap->draw(sf::Sprite(renderLight.occluders->getTexture()), sf::RenderStates(&_shadowMapShader));
            renderLight.shadowMap->display();
        }
    }*/
}

void LightningMgr::RenderLightMaps()
{
    /*const sf::FloatRect winRectAdjusted = Camera::ScreenToWorld(GenUtils::ConvertTo<float>(Application::Get().GetWindow().GetScreenRect()));
    for ( auto &renderLight : _renderLights )
    {
        if ( renderLight.light->GetBoundingBox().intersects(winRectAdjusted) )
        {
            _renderLightsShader.setUniform("resolution", _resolution);
            renderLight.lightMap->clear(sf::Color(0, 0, 0, 0));
            sf::Sprite shadowMapSprite(renderLight.shadowMap->getTexture());
            for ( int i = 0; i < renderLight.light->GetBoundingBox().height; i++ )
            {
                shadowMapSprite.setPosition(0.0, static_cast<float>(i));
                renderLight.lightMap->draw(shadowMapSprite, sf::RenderStates(&_renderLightsShader));
            }
            renderLight.lightMap->display();
        }
    }*/
}

void LightningMgr::RenderCollectiveLightMap()
{
    /*sf::FloatRect winRectAdjusted = Camera::ScreenToWorld(GenUtils::ConvertTo<float>(Application::Get().GetWindow().GetScreenRect()));
    _collectiveLightMap.clear(sf::Color(_ambient));
    for ( auto &renderLight : _renderLights )
    {
        if ( renderLight.light->GetBoundingBox().intersects(winRectAdjusted) )
        {
            sf::Sprite lightSprite(renderLight.lightMap->getTexture());
            sf::FloatRect box = renderLight.light->GetBoundingBox();
            lightSprite.setScale(box.width / lightSprite.getLocalBounds().width,
                                 box.height / lightSprite.getLocalBounds().height);
            lightSprite.setPosition(box.left, box.top);
            lightSprite.setColor(renderLight.light->GetColor());
            Camera::Draw(lightSprite, _collectiveLightMap, sf::RenderStates(sf::BlendAdd));
        }
    }
    _collectiveLightMap.display();*/
}
}