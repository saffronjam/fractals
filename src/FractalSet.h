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
		GPUComputeShader,
		GPUPixelShader
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

	void Resize(const sf::Vector2f& size);

	const sf::Texture& GetPaletteTexture() const;
	void SetSimBox(const SimBox& box);
	void SetComputeIterationCount(size_t iterations) noexcept;
	void SetPalette(Palette palette) noexcept;

protected:
	virtual Shared<ComputeShader> GetComputeShader() = 0;
	virtual void UpdateComputeShaderUniforms() = 0;

	virtual Shared<sf::Shader> GetPixelShader() = 0;
	virtual void UpdatePixelShaderUniforms() = 0;
	
	// Fix for problem with using OpenGL freely alongside SFML
	static void SetUniform(Uint32 id, const String &name, const sf::Vector2<double> &value);
	static void SetUniform(Uint32 id, const String &name, float value);
	static void SetUniform(Uint32 id, const String &name, double value);
	static void SetUniform(Uint32 id, const String &name, int value);

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

	static constexpr int PaletteWidth = 256;

private:
	struct TransitionColor
	{
		float r;
		float g;
		float b;
		float a;
	};

	ComputeHost _computeHost = ComputeHost::GPUPixelShader;
	sf::Vector2f _desiredSimulationDimensions;
	
	// CPU Host
	sf::VertexArray _vertexArray;
	int* _fractalArray;
	
	// GPU Compute Shader Host
	sf::Texture _outputCS;

	// GPU Pixel Shader Host
	sf::RenderTexture _outputPS;
	
	// Shader Host common
	Shared<sf::Shader> _painterPS;
	sf::RenderTexture _shaderBasedHostTarget;
	sf::Texture _testTex;
	sf::Texture _paletteTexture;
	
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

protected:
	struct Worker
	{
		virtual ~Worker() = default;
		virtual void Compute() = 0;

		Atomic<size_t>* nWorkerComplete = nullptr;

		int* fractalArray;
		int simWidth = 0;

		Position imageTL = {0.0, 0.0};
		Position imageBR = {0.0, 0.0};
		Position fractalTL = {0.0, 0.0};
		Position fractalBR = {0.0, 0.0};

		size_t iterations = 0;
		bool alive = true;

		Thread thread;
		ConditionVariable cvStart;
		Mutex mutex;
	};
};
}
