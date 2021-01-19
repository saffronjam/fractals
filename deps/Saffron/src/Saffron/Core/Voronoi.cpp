#include "SaffronPCH.h"

#define JC_VORONOI_IMPLEMENTATION

#include "Saffron/Core/Voronoi.h"

namespace Se
{
Voronoi::Polygon::Polygon(Voronoi &parent, int lineVAIndex, int filledVAIndex, const sf::Vector2f &voronoiPoint,
						  ArrayList<sf::Vector2f> points) :
	_parent(parent),
	_lineVAIndex(lineVAIndex),
	_filledVAIndex(filledVAIndex),
	_points(Move(points)),
	_voronoiPoint(voronoiPoint)
{
}

void Voronoi::Polygon::SetFillColor(sf::Color color)
{
	_parent.SetFillColor(*this, color);
}

void Voronoi::Polygon::ClearFillColor(sf::Color color)
{
	SetFillColor(sf::Color::Transparent);
}

Pair<sf::Vector2f, sf::Vector2f> Voronoi::Polygon::GetClosestEdge(const sf::Vector2f &position) const
{
	constexpr float inf = std::numeric_limits<float>::infinity();
	Pair<float, float> distCache{ inf, inf };
	Pair<int, int> closestIndices{ -1, -1 };

	for ( int i = 0; i < _points.size(); i++ )
	{
		const float firstCandidate = VecUtils::LengthSq(position - _points[i]);
		if ( closestIndices.first == -1 || firstCandidate < distCache.first )
		{
			closestIndices.first = i;
			distCache.first = firstCandidate;
		}
	}

	for ( int i = 0; i < _points.size(); i++ )
	{
		if ( i != closestIndices.first )
		{
			const float secondCandidate = VecUtils::LengthSq(position - _points[i]);
			if ( closestIndices.second == -1 || secondCandidate < distCache.second )
			{
				closestIndices.second = i;
				distCache.second = secondCandidate;
			}
		}
	}

	return { _points[closestIndices.first], _points[closestIndices.second] };

}

Voronoi::Voronoi() :
	_boundingBox(0.0f, 0.0f, 0.0f, 0.0f)
{
	SetupUpdateCallback();
}

Voronoi::Voronoi(const sf::FloatRect &boundingBox) :
	_boundingBox(boundingBox)
{
	SetupUpdateCallback();
}

Voronoi::Voronoi(const sf::FloatRect &boundingBox, ArrayList<sf::Vector2f> points) :
	_boundingBox(boundingBox),
	_points(Move(points))
{
	SetupUpdateCallback();
	ForceGenerate();
}

Voronoi::Voronoi(const sf::FloatRect &boundingBox, int noRandomPoints) :
	_boundingBox(boundingBox)
{
	SetPoints(noRandomPoints);
	SetupUpdateCallback();
	ForceGenerate();
}

Voronoi::~Voronoi()
{
	if ( _diagram.has_value() )
	{
		jcv_diagram_free(&_diagram.value());
	}

	Run::Remove(_updateHandle);
}

void Voronoi::SetPoints(ArrayList<sf::Vector2f> points)
{
	_points = Move(points);
	MarkForGeneration();
}

void Voronoi::SetPoints(int noRandomPoints)
{
	GeneratePoints(noRandomPoints);
	MarkForGeneration();
}

void Voronoi::SetBoundingBox(const sf::FloatRect &boundingBox)
{
	_boundingBox = boundingBox;
	MarkForGeneration();
}

void Voronoi::SetOutlineColor(const sf::Color &color)
{
	for ( int i = 0; i < _polygonsVA.getVertexCount(); i++ )
	{
		_polygonsVA[i].color = color;
	}
}

void Voronoi::SetFillColor(const Polygon &polygon, sf::Color color)
{
	const auto result = std::find(_polygons.begin(), _polygons.end(), polygon);
	if ( result != _polygons.end() )
	{
		const size_t noPoints = polygon.GetPoints().size();
		const int VAIndex = polygon.GetFilledVAIndex();
		for ( int i = 0; i < noPoints * 3; i++ )
		{
			_filledPolygonsVA[VAIndex + i].color = color;
		}
	}
}

void Voronoi::Relax(int iterations)
{
	SE_CORE_ASSERT(_diagram.has_value(), "Voronoi was not created. _diagram was not instantiated");
	for ( int i = 0; i < iterations; i++ )
	{
		const jcv_site *sites = jcv_diagram_get_sites(&_diagram.value());
		for ( int i = 0; i < _diagram.value().numsites; ++i )
		{
			const jcv_site *site = &sites[i];
			jcv_point sum = site->p;
			int count = 1;
			for ( const jcv_graphedge *edge = site->edges; edge != nullptr; edge = edge->next )
			{
				sum.x += edge->pos[0].x;
				sum.y += edge->pos[0].y;
				++count;
			}
			_points[site->index].x = sum.x / count;
			_points[site->index].y = sum.y / count;
		}
		ForceGenerate();
	}
}

Voronoi::Polygon &Voronoi::GetPolygon(const sf::Vector2f &position)
{
	float minDistance = std::numeric_limits<float>::infinity();
	Polygon *closest = nullptr;
	for ( auto &polygon : _polygons )
	{
		const float distance = VecUtils::LengthSq(polygon.GetVoronoiPoint() - position);
		if ( distance < minDistance )
		{
			minDistance = distance;
			closest = &polygon;
		}
	}

	SE_CORE_ASSERT(closest != nullptr, "Failed to find any polygons closer than std::numeric_limits<float>::infinity() units from given position");
	return *closest;
}

void Voronoi::ForceGenerate()
{
	_wantNewPolygonVA = false;
	Generate();
}

jcv_rect Voronoi::ConvertBoundingBox(const sf::FloatRect &boundingBox)
{
	return { boundingBox.left, boundingBox.top, boundingBox.left + boundingBox.width,
			boundingBox.top + boundingBox.height };
}

void Voronoi::Generate()
{
	SE_CORE_ASSERT(_boundingBox.width > 0.0f && _boundingBox.height > 0.0f, "Invalid bounding box dimensions.");
	SE_CORE_ASSERT(!_points.empty(), "No points available");

	jcv_rect rect = ConvertBoundingBox(_boundingBox);

	if ( _diagram.has_value() )
	{
		jcv_diagram_free(&_diagram.value());
		std::memset(&_diagram.value(), 0, sizeof(jcv_diagram));
	}
	_diagram = jcv_diagram();
	jcv_diagram_generate(_points.size(), reinterpret_cast<const jcv_point *>(_points.data()), &rect, nullptr,
						 &_diagram.value());

	// Generate polygon list from sites
	const jcv_site *sites = jcv_diagram_get_sites(&_diagram.value());

	_polygonsVA.clear();
	_filledPolygonsVA.clear();
	_polygons.clear();
	_polygons.reserve(_diagram.value().numsites);

	const auto setupPolygon = [this](Polygon &polygon, int &currentLineVAIndex, int &currentFilledVAIndex)
	{
		const auto &points = polygon.GetPoints();
		for ( int i = 0; i < points.size() - 1; i++ )
		{
			_polygonsVA.append({ points[i], _defaultGridColor });
			_polygonsVA.append({ points[i + 1], _defaultGridColor });
		}
		_polygonsVA.append({ points.back(), _defaultGridColor });
		_polygonsVA.append({ points.front(), _defaultGridColor });

		const auto &voronoiPoint = polygon.GetVoronoiPoint();
		const sf::Color fillColor = sf::Color::Transparent; // = polygon.GetFillColor();
		for ( int i = 0; i < points.size() - 1; i++ )
		{
			_filledPolygonsVA.append({ voronoiPoint, fillColor });
			_filledPolygonsVA.append({ points[i], fillColor });
			_filledPolygonsVA.append({ points[i + 1], fillColor });
		}
		_filledPolygonsVA.append({ voronoiPoint, fillColor });
		_filledPolygonsVA.append({ points.front(), fillColor });
		_filledPolygonsVA.append({ points.back(), fillColor });

		currentLineVAIndex += points.size() * 2;
		currentFilledVAIndex += points.size() * 3;
	};


	for ( int i = 0, currentLineVAIndex = 0, currentFilledVAIndex = 0; i < _diagram.value().numsites; ++i )
	{
		const jcv_site *site = &sites[i];

		ArrayList<sf::Vector2f> polygonPoints;
		for ( jcv_graphedge *edge = site->edges; edge != nullptr; edge = edge->next )
		{
			polygonPoints.emplace_back(edge->pos[0].x, edge->pos[0].y);
		}

		// Setup Polygon
		Polygon polygon(*this, currentLineVAIndex, currentFilledVAIndex, VecUtils::ConvertTo<sf::Vector2f>(site->p), polygonPoints);
		setupPolygon(polygon, currentLineVAIndex, currentFilledVAIndex);
		_polygons.push_back(polygon);
	}

	for ( int i = 0; i < _diagram.value().numsites; ++i )
	{
		const jcv_site *site = &sites[i];

		for ( jcv_graphedge *edge = site->edges; edge != nullptr; edge = edge->next )
		{
			if ( edge->neighbor )
			{
				Polygon &neighbor = GetPolygon(VecUtils::ConvertTo<sf::Vector2f>(edge->neighbor->p));
				_polygons[i].AddNeighbor(&neighbor);
			}
		}
	}
}

void Voronoi::GeneratePoints(int noPoints)
{
	_points.clear();
	_points.reserve(noPoints);

	RandomGenerator<float> generatorX(_boundingBox.left, _boundingBox.left + _boundingBox.width);
	RandomGenerator<float> generatorY(_boundingBox.top, _boundingBox.top + _boundingBox.height);
	for ( int i = 0; i < noPoints; i++ )
	{
		_points.emplace_back(generatorX.Generate(), generatorY.Generate());
	}
}

void Voronoi::MarkForGeneration()
{
	if ( _automaticGeneration )
	{
		_wantNewPolygonVA = true;
	}
}

void Voronoi::SetupUpdateCallback()
{
	_updateHandle = Run::EveryFrame([this]
									{
										if ( _wantNewPolygonVA )
										{
											Generate();
											_wantNewPolygonVA = false;
										}
									});
}

void Voronoi::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	if ( _shouldDrawFilledPolygons )
	{
		target.draw(_filledPolygonsVA, states);
	}
	if ( _shouldDrawGrid )
	{
		target.draw(_polygonsVA, states);
	}
}
}