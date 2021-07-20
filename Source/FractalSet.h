#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include <SFML/Graphics/Image.hpp>

#include <Saffron.h>

namespace Se
{
enum class FractalSetPalette
{
	Fiery,
	FieryAlt,
	UV,
	GreyScale,
	Rainbow
};

enum class FractalSetType
{
	Mandelbrot,
	Julia,
	Buddhabrot
};

enum class FractalSetComputeHost
{
	CPU,
	GPUComputeShader,
	GPUPixelShader
};

enum class FractalSetGenerationType
{
	AutomaticGeneration,
	DelayedGeneration,
	ManualGeneration
};


class FractalSet
{
public:
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

		auto operator==(const SimBox& other) const -> bool
		{
			return TopLeft == other.TopLeft && BottomRight == other.BottomRight;
		}

		auto operator!=(const SimBox& other) const -> bool
		{
			return !(*this == other);
		}
	};

protected:
	struct Worker;

public:
	FractalSet(String name, FractalSetType type, sf::Vector2f renderSize);
	virtual ~FractalSet();

	virtual void OnUpdate(Scene& scene);
	virtual void OnRender(Scene& scene);
	virtual void OnViewportResize(const sf::Vector2f& size);

	void MarkForImageComputation() noexcept;
	void MarkForImageRendering() noexcept;
	void AddWorker(Worker* worker);

	auto Name() const noexcept -> const String&;
	auto Type() const -> FractalSetType;

	auto ComputeHost() const -> FractalSetComputeHost;
	void SetComputeHost(FractalSetComputeHost computeHost);

	void Resize(const sf::Vector2f& size);

	auto PaletteTexture() const -> const sf::Texture&;
	void SetSimBox(const SimBox& box);
	void SetComputeIterationCount(size_t iterations) noexcept;
	void SetPalette(FractalSetPalette palette) noexcept;

	auto GenerationType() const -> FractalSetGenerationType;
	void SetGenerationType(FractalSetGenerationType type);

	void ActivateAxis();
	void DeactivateAxis();

	template <class FractalSetType>
	auto As() -> FractalSetType&
	{
		return dynamic_cast<FractalSetType &>(*this);
	}

	template <class FractalSetType>
	auto As() const -> const FractalSetType&
	{
		return const_cast<FractalSet&>(*this).As<FractalSetType>();
	}

protected:
	virtual auto ComputeShader() -> Shared<ComputeShader> = 0;
	virtual void UpdateComputeShaderUniforms() = 0;

	virtual sf::Vector2u ComputeShaderWorkerDim()
	{
		return sf::Vector2u(_simWidth, _simHeight);
	}

	virtual auto PixelShader() -> Shared<sf::Shader> = 0;
	virtual void UpdatePixelShaderUniforms() = 0;

	// Fix for problem with using OpenGL freely alongside SFML
	static void SetUniform(uint id, const String& name, const sf::Vector2<double>& value);
	static void SetUniform(uint id, const String& name, float value);
	static void SetUniform(uint id, const String& name, double value);
	static void SetUniform(uint id, const String& name, int value);

private:
	void UpdatePaletteTexture();
	void ComputeImage();
	void RenderImage();

protected:
	String _name;
	FractalSetType _type;

	size_t _computeIterations;
	List<Worker*> _workers;
	Atomic<size_t> _nWorkerComplete;

	int _simWidth;
	int _simHeight;
	SimBox _simBox;

	static constexpr int PaletteWidth = 2048;

protected:
	FractalSetComputeHost _computeHost = FractalSetComputeHost::CPU;
	FractalSetGenerationType _generationType = FractalSetGenerationType::AutomaticGeneration;

private:
	struct TransitionColor
	{
		float r;
		float g;
		float b;
		float a;
	};

	sf::Vector2f _desiredSimulationDimensions;

	// CPU Host
	sf::VertexArray _vertexArray;
	int* _fractalArray;

	// GPU Compute Shader Host
	sf::Texture _outputCS;
	List<sf::Color> _blackColorCache;

	// GPU Pixel Shader Host
	sf::RenderTexture _outputPS;

	// Shader Host common
	Shared<sf::Shader> _painterPS;
	sf::RenderTexture _shaderBasedHostTarget;
	sf::Texture _paletteTexture;

	// Image generation
	sf::Time _lastGenerationRequest;

	// Marks with true if the image should be recomputed/reconstructed this frame
	bool _recomputeImage;
	bool _reconstructImage;
	bool _shouldResize;

	// Animate palette change
	FractalSetPalette _desiredPalette;
	sf::Image _currentPalette;
	Array<TransitionColor, PaletteWidth> _colorsStart;
	Array<TransitionColor, PaletteWidth> _colorsCurrent;
	float _colorTransitionTimer;
	float _colorTransitionDuration;
	List<Shared<sf::Image>> _palettes;

	// Axis
	bool _drawAxis = false;
	sf::VertexArray _axisVA;

protected:
	struct Worker
	{
		virtual ~Worker() = default;
		virtual void Compute() = 0;

		Atomic<size_t>* WorkerComplete = nullptr;

		int* FractalArray;
		int SimWidth = 0;

		Position ImageTL = {0.0, 0.0};
		Position ImageBR = {0.0, 0.0};
		Position FractalTL = {0.0, 0.0};
		Position FractalBR = {0.0, 0.0};

		size_t Iterations = 0;
		bool Alive = true;

		Thread Thread;
		ConditionVariable CvStart;
		Mutex Mutex;
	};
};
}
