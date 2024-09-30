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
	void Tag(const string& tag) ;
	bool HasTag(const string& tag) const;
	void Group(const string& group);
	bool HasGroup(const string& group) const;

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
	virtual void RemoveEntityFromPool(int entityID) = 0;
};
template <typename T>
class Pool : public IPool {
private:
	vector<T> data;
	int size;  // Current size of valid elements
	unordered_map<int, int> entityIdToIndex;
	unordered_map<int, int> indexToEntityId;

public:
	Pool(int capacity = 100) {
		size = 0;
		data.resize(capacity);  // Ensure initial capacity
	}

	void RemoveEntityFromPool(int entityID) override {
		if (entityIdToIndex.find(entityID) != entityIdToIndex.end()) {
			Remove(entityID);
		}
	}

	virtual ~Pool() = default;

	bool IsEmpty() const {
		return size == 0;
	}

	int GetSize() const {
		return size;
	}

	void Resize(int newSize) {
		data.resize(newSize);
	}

	void Clear() {
		data.clear();
		size = 0;
	}

	void Add(T object) {
		if (size >= data.size()) {
			Resize(data.size() * 2);  
		}
		data[size++] = object;
	}

	void Remove(int entityID) {
		int indexOfRemoved = entityIdToIndex[entityID];
		int indexOfLast = size - 1;

		data[indexOfRemoved] = data[indexOfLast];

		int entityOfLastElement = indexToEntityId[indexOfLast];
		entityIdToIndex[entityOfLastElement] = indexOfRemoved;
		indexToEntityId[indexOfRemoved] = entityOfLastElement;
		entityIdToIndex.erase(entityID);
		indexToEntityId.erase(indexOfLast);
		size--;
	}

	void Set(int entityID, T object) {
		if (entityIdToIndex.find(entityID) != entityIdToIndex.end()) {
			int index = entityIdToIndex[entityID];
			data[index] = object;
		}
		else {
			int index = size;
			entityIdToIndex[entityID] = index;
			indexToEntityId[index] = entityID;

			if (index >= data.size()) {
				Resize(data.size() * 2);
			}

			data[index] = object;
			size++;
		}
	}

	T& Get(int entityID) {
		return data[entityIdToIndex[entityID]];
	}

	T& operator[](int entityID) {
		return data[entityIdToIndex[entityID]];
	}
};


class WorldCordinator {
private:
	int numberOfEntities = 0;
	vector<shared_ptr<IPool>> ComponentPools;
	vector<Signature> EntityComponentSignatures;
	unordered_map<type_index, shared_ptr<System>> systems;
	unordered_map<int, string> tagPerEntity;
	unordered_map<string, Entity> entityPerTag;
	unordered_map<string, set<Entity>> entitiesPerGroup;
	unordered_map<int, string> groupPerEntity;
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
	// Tags
	void TagEntity(Entity entity, const string& tag);
	bool EntityHasTag(Entity entity, const string& tag) const;
	Entity GetEntityByTag(const string& tag) const;
	void RemoveEntityTag(Entity entity);
	// Group
	void GroupEntity(Entity entity, const string& group);
	bool EntityHasGroup(Entity entity, const string& group) const;
	vector<Entity> GetEntitiesByGroup(const string& group) const;
	void RemoveEntityGroup(Entity entity);

};

template <typename T, typename ...TArgs>
void WorldCordinator::AddComponent(Entity entity, TArgs&& ...args) {
	auto componentID = Component<T>::GetID();
	auto entityID = entity.GetID();


	if (componentID >= this->ComponentPools.size()) {
		this->ComponentPools.resize(componentID + 1);
	}

	if (!this->ComponentPools[componentID]) {
		shared_ptr<Pool<T>> newPool = make_shared<Pool<T>>();
		this->ComponentPools[componentID] = newPool;
	}

	shared_ptr<Pool<T>> componentPool = static_pointer_cast<Pool<T>>(this->ComponentPools[componentID]);

	T newComponent(forward<TArgs>(args)...);

	componentPool->Set(entityID, newComponent);
	this->EntityComponentSignatures[entityID].set(componentID);
}

template <typename T>
void WorldCordinator::RemoveComponent(Entity entity) {
	auto componentID = Component<T>::GetID();
	auto entityID = entity.GetID();

	shared_ptr<Pool<T>> componentPool = static_pointer_cast<Pool<T>>(this->ComponentPools[componentID]);
	componentPool->Remove(entityID);
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

