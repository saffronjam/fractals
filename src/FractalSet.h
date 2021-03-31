#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

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

	enum class ComputeHost
	{
		CPU,
		GPU
	};

	using Position = sf::Vector2<double>;

	struct SimBox
	{
		Position TopLeft;
		Position BottomRight;

		SimBox(const Position& topLeft, const Position& bottomRight) :
			TopLeft(topLeft),
			BottomRight(bottomRight)
		{
		}

		bool operator==(const SimBox& other) const
		{
			return TopLeft == other.TopLeft && BottomRight == other.BottomRight;
		}

		bool operator!=(const SimBox& other) const
		{
			return !(*this == other);
		}
	};

protected:
	struct Worker;

public:
	FractalSet(String name, Type type, sf::Vector2f renderSize);
	virtual ~FractalSet();

	virtual void OnUpdate(Scene& scene);
	virtual void OnRender(Scene& scene);

	void MarkForImageComputation() noexcept;
	void MarkForImageRendering() noexcept;
	void AddWorker(Worker* worker);

	const String& GetName() const noexcept;
	Type GetType() const;
	
	ComputeHost GetComputeHost() const;
	void SetComputeHost(ComputeHost computeHost);

	void ResizeVertexArray(const sf::Vector2f& size);

	void SetSimBox(const SimBox& box);
	void SetComputeIterationCount(size_t iterations) noexcept;
	void SetPalette(Palette palette) noexcept;

protected:
	virtual Shared<ComputeShader> GetComputeShader() = 0;

private:
	void UpdatePaletteTexture();
	void ComputeImage();
	void RenderImage();

protected:
	String _name;
	Type _type;

	size_t _computeIterations;
	ArrayList<Worker*> _workers;
	Atomic<size_t> _nWorkerComplete;

	int _simWidth;
	int _simHeight;
	SimBox _simBox;

private:
	struct TransitionColor
	{
		float r;
		float g;
		float b;
		float a;
	};

	ComputeHost _computeHost = ComputeHost::CPU;

	Shared<ComputeShader> _painterCS;
	sf::Texture _output;
	sf::Texture _data;
	sf::Texture _paletteTexture;

	sf::VertexArray _vertexArray;
	int* _fractalArray;
	sf::Vector2f _vertexArrayDimensions;

	// Marks with true if the image should be recomputed/reconstructed this frame
	bool _recomputeImage;
	bool _reconstructImage;
	bool _shouldResize;

	// Animate palette change
	Palette _desiredPalette;
	sf::Image _currentPalette;
	Array<TransitionColor, 256> _colorsStart;
	Array<TransitionColor, 256> _colorsCurrent;
	float _colorTransitionTimer;
	float _colorTransitionDuration;
	ArrayList<Shared<sf::Image>> _palettes;

	static constexpr int PaletteWidth = 256;

protected:
	struct Worker
	{
		virtual ~Worker() = default;
		virtual void Compute() = 0;

		Atomic<size_t>* nWorkerComplete = nullptr;

		int* fractalArray;
		int simWidth = 0;

		sf::Vector2<double> imageTL = {0.0f, 0.0f};
		sf::Vector2<double> imageBR = {0.0f, 0.0f};
		sf::Vector2<double> fractalTL = {0.0f, 0.0f};
		sf::Vector2<double> fractalBR = {0.0f, 0.0f};

		size_t iterations = 0;
		bool alive = true;

		Thread thread;
		ConditionVariable cvStart;
		Mutex mutex;
	};
};
}
