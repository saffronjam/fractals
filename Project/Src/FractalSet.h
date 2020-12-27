#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <complex>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <Saffron.h>

namespace Se
{
class FractalSet
{
public:
	enum Palette
	{
		Fiery,
		UV,
		GreyScale,
		Rainbow
	};

	enum class Type
	{
		Mandelbrot,
		Julia
	};

protected:
	struct Worker;

public:
	FractalSet(String name, Type type, sf::Vector2f renderSize);
	virtual ~FractalSet();

	virtual void OnUpdate(Scene &scene);
	virtual void OnRender(Scene &scene);

	void MarkForImageComputation() noexcept;
	void MarkForImageRendering() noexcept;
	void AddWorker(Worker *worker);

	const String &GetName() const noexcept { return _name; }
	Type GetType() const { return _type; }

	void ResizeVertexArray(const sf::Vector2f &size);

	void SetSimBox(const Pair<sf::Vector2f, sf::Vector2f> &box);
	void SetComputeIterationCount(size_t iterations) noexcept;
	void SetPalette(Palette palette) noexcept;

private:
	void ComputeImage();
	void RenderImage();

protected:
	String _name;
	Type _type;

	size_t _computeIterations;
	std::vector<Worker *> _workers;
	std::atomic<size_t> _nWorkerComplete;

	int _simWidth;
	int _simHeight;
	std::pair<sf::Vector2f, sf::Vector2f> _simBox;

private:
	struct TransitionColor
	{
		float r;
		float g;
		float b;
		float a;
	};

	sf::VertexArray _vertexArray;
	int *_fractalArray;
	sf::Vector2f _vertexArrayDimensions;

	// Marks with true if the image should be recomputed/reconstructed this frame
	bool _recomputeImage;
	bool _reconstructImage;
	bool _shouldResize;

	// Animate palette change
	Palette _desiredPalette;
	sf::Image _currentPalette;
	std::array<TransitionColor, 256> _colorsStart;
	std::array<TransitionColor, 256> _colorsCurrent;
	float _colorTransitionTimer;
	float _colorTransitionDuration;
	std::vector<sf::Image> _palettes;

	static constexpr int PaletteWidth = 256;

protected:
	struct Worker
	{
		virtual ~Worker() = default;
		virtual void Compute() = 0;

		std::atomic<size_t> *nWorkerComplete = nullptr;

		int *fractalArray;
		int simWidth = 0;

		sf::Vector2<double> imageTL = { 0.0f, 0.0f };
		sf::Vector2<double> imageBR = { 0.0f, 0.0f };
		sf::Vector2<double> fractalTL = { 0.0f, 0.0f };
		sf::Vector2<double> fractalBR = { 0.0f, 0.0f };

		size_t iterations = 0;
		bool alive = true;

		std::thread thread;
		std::condition_variable cvStart;
		std::mutex mutex;

	};
};
}