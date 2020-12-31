#include "SaffronPCH.h"

#include "Saffron/Layer/LayerStack.h"

namespace Se
{

SignalAggregate<std::shared_ptr<Layer>> LayerStack::Signals::OnPushLayer;
SignalAggregate<std::shared_ptr<Layer>> LayerStack::Signals::OnPushOverlay;
SignalAggregate<std::shared_ptr<Layer>> LayerStack::Signals::OnPopLayer;
SignalAggregate<std::shared_ptr<Layer>> LayerStack::Signals::OnPopOverlay;

LayerStack::~LayerStack()
{
	for ( std::shared_ptr<Layer> layer : _layers )
	{
		layer->OnDetach();
	}
	_layers.clear();
}

void LayerStack::PushLayer(std::shared_ptr<Layer> layer, std::shared_ptr<BatchLoader> &batchLoader)
{
	auto newLayer = *_layers.emplace(_layers.begin() + _layerInsertIndex, layer);
	_layerInsertIndex++;
	GetSignals().Emit(Signals::OnPushOverlay, newLayer);
	newLayer->OnAttach(batchLoader);
}

void LayerStack::PushOverlay(std::shared_ptr<Layer> overlay, std::shared_ptr<BatchLoader> &batchLoader)
{
	// TODO: Implement
	SE_CORE_ASSERT("NOT IMPLEMETED");
	auto newOverlay = _layers.emplace_back(overlay);
	GetSignals().Emit(Signals::OnPushOverlay, newOverlay);
}

void LayerStack::PopLayer(int count)
{
	for ( int i = 0; i < count; i++ )
	{
		GetSignals().Emit(Signals::OnPopLayer, _layers.back());
		_layers.back()->OnDetach();
		_layers.pop_back();
		_layerInsertIndex--;
	}
}

void LayerStack::PopOverlay(int count)
{
	// TODO: Implement
	SE_CORE_ASSERT("NOT IMPLEMETED");
	for ( int i = 0; i < count; i++ )
	{
		GetSignals().Emit(Signals::OnPopOverlay, _layers.back());
		_layers.back()->OnDetach();
		_layers.pop_back();
	}
}

void LayerStack::EraseLayer(std::shared_ptr<Layer> layer)
{
	const auto it = std::find(_layers.begin(), _layers.end(), layer);
	if ( it != _layers.end() )
	{
		GetSignals().Emit(Signals::OnPopOverlay, *it);
		(*it)->OnDetach();
		_layers.erase(it);
		_layerInsertIndex--;
	}
}

void LayerStack::EraseOverlay(std::shared_ptr<Layer> overlay)
{
	// TODO: Implement
	SE_CORE_ASSERT("NOT IMPLEMETED");
	const auto it = std::find(_layers.begin(), _layers.end(), overlay);
	if ( it != _layers.end() )
	{
		_layers.erase(it);
	}
}

void LayerStack::Clear()
{
	for ( auto &layer : _layers )
	{
		layer->OnDetach();
	}
	_layers.clear();
}

std::shared_ptr<Layer> LayerStack::Front()
{
	return _layers.front();
}

std::shared_ptr<Layer> LayerStack::Back()
{
	return _layers.back();
}
}
