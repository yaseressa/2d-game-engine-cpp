#pragma once
#include <map>
#include <list>
#include <typeindex>
#include <iostream>
#include "Event.h"
using namespace std;

class IEventCallback {
private:
	virtual void Call (Event& e) = 0;
public:
	virtual ~IEventCallback() = default;
	void Execute(Event& e) {
		Call(e);
	}
};
template <typename T, typename TOwner>
class EventCallback: public IEventCallback  {
private:
	typedef void (TOwner::*CallbackFunction)(T&)
	CallbackFunction callBackFunction;
	TOwner* ownerInstance;
	virtual void Call(Event& e) override {
		invoke(callbackFunction, ownerInstance, static_cast<TEvent&>(e));
	}
public:
	EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction) {
		this->ownerInstance = ownerInstance;
		this->callbackFunction = callbackFunction ;
	}
	virtual ~EventCallback() override = default;
};

typedef  list<unique_ptr<IEventCallback>> HandlerList;

class EventBus {
private:
	map<type_index, unique_ptr<HandlerList>> subscribers;
public:
	EventBus() = default;

	template <typename T, typename TOwner>
	void SubscribeToEvent(TOwner* ownerInstance, void (TOwner::*callbackFunction)(T&)) {
		auto subscriber = make_unique<EventCallback<TOwner, T>>(ownerInstance, callbackFunction);
		if (!subscribers[typeid(T)].get()) {
			subscribers[typeid(T)] = make_unique<HandlerList>();
		}
		subscribers[typeid(T)]->push_back(move(subscriber));
	}
	template <typename T, typename ...TArgs>
	void EmitEvent(TArgs&& ...args) {
		auto handlers = subscribers[typeid(T)].get();
		if (handlers) {
			for (auto it = handlers->begin(); it != handlers->end(); it++) {
				auto handler = it->get();
				TEvent event(forward<TArgs>(args)...);
				handler->Execute(event);
			}
		}
	}
};