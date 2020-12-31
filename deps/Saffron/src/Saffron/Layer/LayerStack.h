#pragma once

#include "Saffron/Config.h"
#include "Layer.h"

namespace Se {

class LayerStack : public Signaller
{
public:
	struct Signals
	{
		static SignalAggregate<std::shared_ptr<Layer>> OnPushLayer;
		static SignalAggregate<std::shared_ptr<Layer>> OnPushOverlay;
		static SignalAggregate<std::shared_ptr<Layer>> OnPopLayer;
		static SignalAggregate<std::shared_ptr<Layer>> OnPopOverlay;
	};

public:
	LayerStack() = default;
	~LayerStack();

	void PushLayer(std::shared_ptr<Layer>, std::shared_ptr<BatchLoader> &batchLoader);
	void PushOverlay(std::shared_ptr<Layer>, std::shared_ptr<BatchLoader> &batchLoader);
	void PopLayer(int count);
	void PopOverlay(int count);
	void EraseLayer(std::shared_ptr<Layer> layer);
	void EraseOverlay(std::shared_ptr<Layer> overlay);

	void Clear();
	std::shared_ptr<Layer> Front();
	std::shared_ptr<Layer> Back();
	ArrayList<std::shared_ptr<Layer>>::iterator begin() { return _layers.begin(); }
	ArrayList<std::shared_ptr<Layer>>::iterator end() { return _layers.end(); }

private:
	ArrayList<std::shared_ptr<Layer>> _layers;
	unsigned int _layerInsertIndex = 0;
};

}