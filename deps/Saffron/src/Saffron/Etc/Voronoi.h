//#pragma once
//
//#include <memory>
//#include <optional>
//#include <vector>
//#include <set>
//#include <cstring>
//
//#include <SFML/Graphics/Rect.hpp>
//#include <SFML/Graphics/ConvexShape.hpp>
//#include <jcv/jc_voronoi.h>
//
//#include "Camera.h"
//#include "IException.h"
//#include "Random.h"
//
//class Voronoi : public sf::Drawable
//{
//public:
//    class Polygon : public sf::ConvexShape
//    {
//    public:
//        Polygon(const sf::Vector2f &voronoiPoint, size_t pointCount = 0)
//            : sf::ConvexShape(pointCount),
//              _voronoiPoint(voronoiPoint)
//        {
//        }
//        Polygon(const sf::ConvexShape &shape, const sf::Vector2f &voronoiPoint)
//            : sf::ConvexShape(shape),
//              _voronoiPoint(voronoiPoint)
//        {
//        }
//
//        void addNeighbor(Polygon *neighbor) { _neighbors.emplace(neighbor); }
//
//        const sf::Vector2f &getVoronoiPoint() const { return _voronoiPoint; }
//        const std::set<Polygon *> &getNeighbors() const { return _neighbors; }
//
//        void setVoronoiPoint(const sf::Vector2f &voronoiPoint) { _voronoiPoint = voronoiPoint; }
//
//    private:
//        std::set<Polygon *> _neighbors;
//        sf::Vector2f _voronoiPoint;
//    };
//
//public:
//    Voronoi();
//    Voronoi(const sf::FloatRect &boundingBox, const std::vector<sf::Vector2f> &points);
//    Voronoi(const sf::FloatRect &boundingBox, int nRandomPoints);
//    ~Voronoi();
//
//    void SetPoints(const std::vector<sf::Vector2f> &points);
//    void SetBoundingBox(const sf::FloatRect &boundingBox);
//    void SetFillColors(const std::vector<sf::Color> &fillColors) { _fillColors = fillColors; }
//    void SetOutlineColor(const sf::Color &color);
//    void SetOutlineThickness(float thickness);
//
//    void Relax(int iterations = 1);
//
//    const std::vector<Voronoi::Polygon> &GetPolygons() const { return _polygons; }
//    Voronoi::Polygon &GetPolygon(const sf::Vector2f &position);
//
//protected:
//    void GenerateVoronoi();
//    static jcv_rect ConvertBoundingBox(const sf::FloatRect &boundingBox);
//
//private:
//    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
//
//    std::optional<jcv_diagram> _diagram;
//    sf::FloatRect _boundingBox;
//    std::vector<sf::Vector2f> _points;
//    std::vector<Voronoi::Polygon> _polygons;
//
//    std::vector<sf::Color> _fillColors;
//
//public:
//    class Exception : public IException
//    {
//    public:
//        Exception(int line, const char *file, const char *errorString);
//        const char *what() const override;
//        virtual const char *GetType() const override;
//        const char *GetErrorString() const;
//
//    private:
//        std::string errorString;
//    };
//};