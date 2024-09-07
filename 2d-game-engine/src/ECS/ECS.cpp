#include "ECS.h"


int IComponent::nextID = 0;

int Entity::GetID() const {
	return this->ID;
}

void System::AddEntityToSystem(Entity entity) {
	this->entities.push_back(entity);
};
void System::RemoveEntityToSystem(Entity entity) {
	this->entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity Other) {return entity == Other; }), entities.end());
};
vector<Entity> System::GetSystemEntities() const {
	return this->entities;
};
const Signature& System::GetComponentSignature() const {
	return this->ComponentSignature;
};

Entity WorldCordinator::CreateEntity() {
	int entityID;
	if (freeIDs.empty()) {
		entityID = this->numberOfEntities++;
		if (entityID >= EntityComponentSignatures.size()) {
			EntityComponentSignatures.resize(entityID + 1);
		} 
	}
	else {
		entityID = freeIDs.front();
		freeIDs.pop_front();
	}
	Entity newEntity(entityID);
	newEntity.worldCordinator = this;
	this->entitiesToBeCreated.insert(newEntity);
	return newEntity;
};
void WorldCordinator::KillEntity(Entity entity) { 
	this->entitiesToBekilled.insert(entity);

}

void WorldCordinator::AddEntityToSystems(Entity entity) {
	const auto entityID = entity.GetID();
	const auto entityComponentSignature = this->EntityComponentSignatures[entityID];
	for (auto& system : this->systems) {
		const auto& systemComponentSignature = system.second->GetComponentSignature();
		if ((entityComponentSignature & systemComponentSignature) == systemComponentSignature)
			system.second->AddEntityToSystem(entity);
	}
}
void WorldCordinator::RemoveEntityFromSystems(Entity entity) {

	for (auto& system : this->systems) {
		system.second->RemoveEntityToSystem(entity);
	}
}

void WorldCordinator::Update() {

	for (auto entity : entitiesToBeCreated) {
		AddEntityToSystems(entity);
	}
	entitiesToBeCreated.clear();

	for (auto entity : entitiesToBekilled) {
		RemoveEntityFromSystems(entity);
		EntityComponentSignatures[entity.GetID()].reset();
		freeIDs.push_back(entity.GetID());
	}
	entitiesToBekilled.clear();
};

void Entity::Kill() {
	worldCordinator->RemoveEntityFromSystems(*this);
};