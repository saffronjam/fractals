#pragma once

#include <optional>
#include <vector>
#include <set>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <jcv/jc_voronoi.h>

#include <Saffron.h>

namespace Se
{
class Voronoi : public sf::Drawable
{
private:
	friend class Polygon;

public:
	class Polygon
	{
	public:
		Polygon(Voronoi &parent, int lineVAIndex, int filledVAIndex, const sf::Vector2f &voronoiPoint,
				ArrayList<sf::Vector2f> points);

		bool operator==(const Polygon &other) const { return _uuid == other._uuid; }

		int GetLineVAIndex() const { return _lineVAIndex; }
		int GetFilledVAIndex() const { return _filledVAIndex; }

		const ArrayList<sf::Vector2f> &GetPoints() const { return _points; }

		sf::Color GetFillColor() const { return _fillColor; }
		void SetFillColor(sf::Color color);
		void ClearFillColor(sf::Color color);

		const Set<Polygon *> &GetNeighbors() const { return _neighbors; }
		void AddNeighbor(Polygon *neighbor) { _neighbors.emplace(neighbor); }

		const sf::Vector2f &GetVoronoiPoint() const { return _voronoiPoint; }
		void SetVoronoiPoint(const sf::Vector2f &voronoiPoint) { _voronoiPoint = voronoiPoint; }

		Pair<sf::Vector2f, sf::Vector2f> GetClosestEdge(const sf::Vector2f &position) const;

	private:
		UUID _uuid;

		Voronoi &_parent;
		int _lineVAIndex;
		int _filledVAIndex;

		sf::Color _fillColor = sf::Color::Transparent;
		ArrayList<sf::Vector2f> _points;
		std::set<Polygon *> _neighbors;
		sf::Vector2f _voronoiPoint;
	};

public:
	Voronoi();
	explicit Voronoi(const sf::FloatRect &boundingBox);
	Voronoi(const sf::FloatRect &boundingBox, ArrayList<sf::Vector2f> points);
	Voronoi(const sf::FloatRect &boundingBox, int noRandomPoints);
	~Voronoi() override;

	void SetPoints(ArrayList<sf::Vector2f> points);
	void SetPoints(int noRandomPoints);
	void SetBoundingBox(const sf::FloatRect &boundingBox);
	void SetOutlineColor(const sf::Color &color);
	void SetFillColor(const Polygon &polygon, sf::Color color);

	void ShowGrid() { _shouldDrawGrid = true; }
	void HideGrid() { _shouldDrawGrid = false; }

	void ShowFilled() { _shouldDrawFilledPolygons = true; }
	void HideFilled() { _shouldDrawFilledPolygons = false; }

	void Relax(int iterations = 1);

	const ArrayList<Polygon> &GetPolygons() const { return _polygons; }
	Polygon &GetPolygon(const sf::Vector2f &position);

	// Manual controls
	void EnableAutomaticGeneration() { _automaticGeneration = true; }
	void DisableAutomaticGeneration() { _automaticGeneration = false; }
	void ForceGenerate();

private:
	static jcv_rect ConvertBoundingBox(const sf::FloatRect &boundingBox);

	void Generate();
	void GeneratePoints(int noPoints);
	void MarkForGeneration();

	void SetupUpdateCallback();

	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
	Run::Handle _updateHandle;
	bool _automaticGeneration = true;

	sf::Color _defaultGridColor = sf::Color::Blue;
	Optional<jcv_diagram> _diagram;
	sf::FloatRect _boundingBox;
	ArrayList<sf::Vector2f> _points;

	ArrayList<Polygon> _polygons;
	sf::VertexArray _polygonsVA{ sf::PrimitiveType::Lines };
	sf::VertexArray _filledPolygonsVA{ sf::PrimitiveType::Triangles };
	bool _wantNewPolygonVA = false;

	bool _shouldDrawFilledPolygons = true;
	bool _shouldDrawGrid = true;
};
}

