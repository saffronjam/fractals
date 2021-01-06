#include "SaffronPCH.h"
//
//#define JC_VORONOI_IMPLEMENTATION
//
//#include "Saffron/Etc/Voronoi.h"
//
//Voronoi::Voronoi()
//	: Voronoi(sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f), std::vector<sf::Vector2f>())
//{
//}
//
//Voronoi::Voronoi(const sf::FloatRect& boundingBox, const std::vector<sf::Vector2f>& points)
//	: _boundingBox(boundingBox),
//	_points(_points),
//	_fillColors({ sf::Color::Transparent })
//{
//	GenerateVoronoi();
//}
//
//Voronoi::Voronoi(const sf::FloatRect& boundingBox, int nRandomPoints)
//	: _boundingBox(boundingBox),
//	_fillColors({ sf::Color::Transparent })
//{
//	for (int i = 0; i < nRandomPoints; i++)
//		_points.push_back(Random::Vec2(boundingBox.left, boundingBox.top, boundingBox.left + boundingBox.width, boundingBox.top + boundingBox.height));
//	GenerateVoronoi();
//}
//
//Voronoi::~Voronoi()
//{
//	if (_diagram.has_value())
//		jcv_diagra_free(&_diagram.value());
//}
//
//void Voronoi::SetPoints(const std::vector<sf::Vector2f>& points)
//{
//	_points = points;
//	GenerateVoronoi();
//}
//
//void Voronoi::SetBoundingBox(const sf::FloatRect& boundingBox)
//{
//	_boundingBox = boundingBox;
//	GenerateVoronoi();
//}
//
//void Voronoi::SetOutlineColor(const sf::Color& color)
//{
//	for (auto& polygon : _polygons)
//	{
//		polygon.setOutlineColor(color);
//	}
//}
//
//void Voronoi::SetOutlineThickness(float thickness)
//{
//	for (auto& polygon : _polygons)
//	{
//		polygon.setOutlineThickness(thickness);
//	}
//}
//
//void Voronoi::Relax(int iterations)
//{
//	if (_diagram.has_value())
//	{
//		for (int i = 0; i < iterations; i++)
//		{
//			const jcv_site* sites = jcv_diagra_get_sites(&_diagram.value());
//			for (int i = 0; i < _diagram.value().numsites; ++i)
//			{
//				const jcv_site* site = &sites[i];
//				jcv_point sum = site->p;
//				int count = 1;
//				for (const jcv_graphedge* edge = site->edges; edge != nullptr; edge = edge->next)
//				{
//					sum.x += edge->pos[0].x;
//					sum.y += edge->pos[0].y;
//					++count;
//				}
//				_points[site->index].x = sum.x / count;
//				_points[site->index].y = sum.y / count;
//			}
//		}
//		GenerateVoronoi();
//	}
//	else
//	{
//		throw Voronoi::Exception(__LINE__, __FILE__, "Voronoi was not created. _diagram was not instantiated");
//	}
//}
//
//Voronoi::Polygon& Voronoi::GetPolygon(const sf::Vector2f& position)
//{
//	float minDistance = std::numeric_limits<float>::infinity();
//	Voronoi::Polygon* closest = nullptr;
//	for (auto& polygon : _polygons)
//	{
//		float distance = VecUtils::LengthSq(polygon.getVoronoiPoint() - position);
//		if (distance < minDistance)
//		{
//			minDistance = distance;
//			closest = &polygon;
//		}
//	}
//
//	if (closest != nullptr)
//	{
//		return *closest;
//	}
//	else
//	{
//		throw Voronoi::Exception(__LINE__, __FILE__, "Failed to find any polygons closer than std::numeric_limits<float>::infinity() units from given position");
//	}
//}
//
//void Voronoi::GenerateVoronoi()
//{
//	if (_boundingBox.width > 0.0f &&
//		_boundingBox.height > 0.0f &&
//		_points.size() > 0)
//	{
//		jcv_rect rect = ConvertBoundingBox(_boundingBox);
//
//		if (_diagram.has_value())
//		{
//			jcv_diagra_free(&_diagram.value());
//			std::memset(&_diagram.value(), 0, sizeof(jcv_diagram));
//		}
//		_diagram = jcv_diagram();
//		jcv_diagra_generate(_points.size(), reinterpret_cast<const jcv_point*>(_points.data()), &rect, nullptr, &_diagram.value());
//
//		// Generate polygon list from sites
//		const jcv_site* sites = jcv_diagra_get_sites(&_diagram.value());
//		_polygons.clear();
//		_polygons.reserve(_diagram.value().numsites);
//		for (int i = 0; i < _diagram.value().numsites; ++i)
//		{
//			const jcv_site* site = &sites[i];
//
//			std::vector<sf::Vector2f> polygonPoints;
//			for (jcv_graphedge* edge = site->edges; edge != nullptr; edge = edge->next)
//			{
//				polygonPoints.emplace_back(edge->pos[0].x, edge->pos[0].y);
//			}
//			sf::ConvexShape convexShape = Lib::CreateConvexShape(polygonPoints);
//			Voronoi::Polygon polygon(convexShape, VecUtils::ConvertTo<sf::Vector2f>(site->p));
//			polygon.setFillColor(_fillColors[i % _fillColors.size()]);
//			polygon.setOutlineColor(sf::Color::White);
//			polygon.setOutlineThickness(1);
//
//			_polygons.emplace_back(polygon);
//		}
//
//		for (int i = 0; i < _diagram.value().numsites; ++i)
//		{
//			const jcv_site* site = &sites[i];
//
//			for (jcv_graphedge* edge = site->edges; edge != nullptr; edge = edge->next)
//			{
//				if (edge->neighbor)
//				{
//					Voronoi::Polygon& neighbor = GetPolygon(VecUtils::ConvertTo<sf::Vector2f>(edge->neighbor->p));
//					_polygons[i].addNeighbor(&neighbor);
//				}
//			}
//		}
//	}
//}
//
//jcv_rect Voronoi::ConvertBoundingBox(const sf::FloatRect& boundingBox)
//{
//	return { boundingBox.left, boundingBox.top, boundingBox.left + boundingBox.width, boundingBox.top + boundingBox.height };
//}
//
//void Voronoi::draw(sf::RenderTarget& target, sf::RenderStates states) const
//{
//	if (_diagram.has_value())
//	{
//		for (auto& polygon : _polygons)
//		{
//			target.draw(polygon, states);
//		}
//	}
//	else
//	{
//		throw Voronoi::Exception(__LINE__, __FILE__, "Voronoi was not created. _diagram was not instantiated");
//	}
//}
//
//Voronoi::Exception::Exception(int line, const char* file, const char* errorString)
//	: IException(line, file),
//	errorString(errorString)
//{
//}
//
//const char* Voronoi::Exception::what() const
//{
//	std::ostringstream oss;
//	oss << "[Type] " << GetType() << std::endl
//		<< "[Description] " << GetErrorString() << std::endl
//		<< GetOriginString();
//	whatBuffer = oss.str();
//	return whatBuffer.c_str();
//}
//
//const char* Voronoi::Exception::GetType() const
//{
//	return "V-2DFramework Voronoi Exception";
//}
//
//const char* Voronoi::Exception::GetErrorString() const
//{
//	return errorString.c_str();
//}