#pragma once
#include <iostream>
#include <bitset>
#include <vector>
#include <set>
#include <unordered_map>
#include <deque>
#include <typeindex>
#include "../Logger/Logger.h"
#include <string>
#include <memory>
using namespace std;
const unsigned int MAX_COMPONENTS = 32;
typedef bitset<MAX_COMPONENTS> Signature;

struct IComponent {
protected:
	static int nextID;
};


template <typename T>
class Component : public IComponent {

public:
	static int GetID() {
		static auto ID = nextID++;
		return ID;
	};
};

class Entity {
private:
	int ID;
public:
	Entity(int ID) : ID(ID) {};
	int GetID() const;
	bool operator==(const Entity& other) const {
		return this->GetID() == other.GetID();
	};
	bool operator!=(const Entity& other) const {
		return this->GetID() != other.GetID();
	};
	bool operator<(const Entity& other) const {
		return this->GetID() < other.GetID();
	}
	template <typename T, typename ...TArgs>
	void AddComponent( TArgs&& ...args);
	template <typename T>
	void RemoveComponent();
	template <typename T>
	bool HasComponent();
	template <typename T>
	T& GetComponent();
	class WorldCordinator* worldCordinator;
	void Kill();
};




class System { 
private:
	Signature ComponentSignature;
	vector<Entity> entities;
public:
	System() = default;
	~System() = default;
	void AddEntityToSystem(Entity entity);
	void RemoveEntityToSystem(Entity entity);
	vector<Entity> GetSystemEntities() const;
	const Signature& GetComponentSignature() const;
	template <typename T> void RequireComponent();

};


template <typename T> 
void System::RequireComponent() {
	const auto ComponentID = Component<T>::GetID();
	ComponentSignature.set(ComponentID);
};
struct IPool {
public: 
	virtual ~IPool() = default;
};
template <typename T>
class Pool: public IPool {
private:
	vector<T> components;
public:
	Pool(int size = 100) {
		Resize(size);
	};
	virtual ~Pool() = default;
	bool IsEmpty() const {
		return components.empty();
	}
	int GetSize() const {
		return components.size();
	}
	void Resize(int size) {
		components.resize(size);
	}
	void Clear() {
		components.clear();
	}
	void Add(T component) {
		components.push_back(component);
	}
	void Set(int idx, T component) {
		components[idx]  = component;
	}
	T& Get(int idx) {
		return static_cast<T&>(components[idx]);
	}
	T& operator [](int idx) {
		return static_cast<T&>(components[idx]);
	}

	
};

class WorldCordinator {
private:
	int numberOfEntities = 0;
	vector<shared_ptr<IPool>> ComponentPools;
	vector<Signature> EntityComponentSignatures;
	unordered_map<type_index, shared_ptr<System>> systems;
	set<Entity> entitiesToBeCreated;
	set<Entity> entitiesToBekilled;
	deque<int> freeIDs;

public:
	WorldCordinator() {
		Logger::Info("World Co-ordinator initialized. ");
	};
	~WorldCordinator() {
		Logger::Info("World Co-ordinator Destructed. ");
	};
	Entity CreateEntity();
	void KillEntity(Entity entity);
	template <typename T, typename ...TArgs>
	void AddComponent(Entity entity, TArgs&& ...args);
	template <typename T>
	void RemoveComponent(Entity entity);
	template <typename T>
	bool HasComponent(Entity entity);
	template <typename T>
	T& GetComponent(Entity entity);
	template <typename T, typename ...TArgs>
	void AddSystem(TArgs... args);
	template <typename T>
	void RemoveSystem();
	template <typename T>
	bool HasSystem();
	template <typename T>
	T& GetSystem();
	void AddEntityToSystems(Entity entity);
	void RemoveEntityFromSystems(Entity entity);
	void Update();
};

template <typename T, typename ...TArgs>
void WorldCordinator::AddComponent(Entity entity, TArgs&& ...args) {
	auto componentID = Component<T>::GetID();
	auto entityID = entity.GetID();

	if (componentID >= this->ComponentPools.size()) {
		this->ComponentPools.resize(componentID + 1);
	}
	if (!ComponentPools[componentID]) {
		shared_ptr<Pool<T>> newPool = make_shared<Pool<T>>();
		//Pool<T>* newPool = new Pool<T>();
		this->ComponentPools[componentID] = newPool;
	}
	shared_ptr<Pool<T>> componentPool = static_pointer_cast<Pool<T>>(this->ComponentPools[componentID]);

	if (entityID >= componentPool->GetSize()) {
		componentPool->Resize(this->numberOfEntities);
	}
	T newComponent(forward<TArgs>(args)...);
	componentPool->Set(entityID, newComponent);
	this->EntityComponentSignatures[entityID].set(componentID);
}
template <typename T>
void WorldCordinator::RemoveComponent(Entity entity) {
	auto componentID = Component<T>::GetID();
	auto entityID = entity.GetID();

	if (!this->ComponentPools[componentID]) {
		Logger::Error("No Component Found");
		return;
	}
	this->EntityComponentSignatures[entityID].set(componentID, false);
};

template <typename T>
bool WorldCordinator::HasComponent(Entity entity) {
	auto componentID = Component<T>::GetID();
	auto entityID = entity.GetID();

	if (!this->ComponentPools[componentID]) {
		Logger::Error("No Component Found");
		return false;
	}
	return this->EntityComponentSignatures[entityID].test(componentID);
};
template <typename T>
T& WorldCordinator::GetComponent(Entity entity) {
	auto componentID = Component<T>::GetID();
	auto entityID = entity.GetID();

	shared_ptr<Pool<T>> componentPool = static_pointer_cast<Pool<T>>(this->ComponentPools[componentID]);
	return componentPool->Get(entityID);
};


template <typename T, typename ...TArgs>
void WorldCordinator::AddSystem(TArgs... args) {
	shared_ptr<T> newSystem = make_shared<T>(forward<TArgs>(args)...);
	this->systems.insert(make_pair(type_index(typeid(T)), newSystem));
};
template <typename T>
void WorldCordinator::RemoveSystem() {
	auto system = this->systems.find(type_index(typeid(T)));
	this->systems.erase(system); 
};
template <typename T>
bool WorldCordinator::HasSystem() {
	return this->systems.find(type_index(typeid(T))) != this->systems.end();

};
template <typename T>
T& WorldCordinator::GetSystem() {
	auto system = this->systems.find(type_index(typeid(T)));
	return *(static_pointer_cast<T>(system->second));
};


template <typename T, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args) {
	this->worldCordinator->AddComponent<T>(*this, forward<TArgs>(args)...);
}
template <typename T>
void Entity::RemoveComponent() {
	this->worldCordinator->RemoveComponent<T>(*this);
};

template <typename T>
bool Entity::HasComponent() {
	return this->worldCordinator->HasComponent<T>(*this);
};
template <typename T>
T& Entity::GetComponent() {
	return this->worldCordinator->GetComponent<T>(*this);
};


