#include "ECS.h"
#include <string>
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
		for (auto componentPool : ComponentPools) {
			if(componentPool) componentPool->RemoveEntityFromPool(entity.GetID());
		}
		freeIDs.push_back(entity.GetID());
		RemoveEntityTag(entity);
		RemoveEntityGroup(entity);
	}
	entitiesToBekilled.clear();
};

void WorldCordinator::TagEntity(Entity entity, const string& tag) {
	entityPerTag.emplace(tag, entity);
	tagPerEntity.emplace(entity.GetID(), tag);
}
bool WorldCordinator::EntityHasTag(Entity entity, const string& tag) const {
	if (tagPerEntity.find(entity.GetID()) == tagPerEntity.end()) {
		return false;
	}
	Logger::Info(to_string(entityPerTag.find(tag)->second == entity));
}
Entity WorldCordinator::GetEntityByTag(const string& tag) const {
	return entityPerTag.at(tag);
}
void WorldCordinator::RemoveEntityTag(Entity entity) {
	auto taggedEntity = tagPerEntity.find(entity.GetID());
	if (taggedEntity != tagPerEntity.end()) {
		auto tag = taggedEntity->second;
		entityPerTag.erase(tag);
		tagPerEntity.erase(taggedEntity);
	}
}
void WorldCordinator::GroupEntity(Entity entity, const string& group) {
	entitiesPerGroup.emplace(group, set<Entity> ());
	entitiesPerGroup[group].emplace(entity);
	groupPerEntity.emplace(entity.GetID(), group);
}
bool WorldCordinator::EntityHasGroup(Entity entity, const string& group) const {
	auto it = entitiesPerGroup.find(group);
	if (it != entitiesPerGroup.end()) {
		const auto& groupEntities = it->second;
		return groupEntities.find(entity.GetID()) != groupEntities.end();
	}
	return false;
}
vector<Entity> WorldCordinator::GetEntitiesByGroup(const string& group) const {
	auto& setOfEntities = entitiesPerGroup.at(group);
	return vector<Entity>(setOfEntities.begin(), setOfEntities.end());
}
void WorldCordinator::RemoveEntityGroup(Entity entity) {
	auto groupedEntity = groupPerEntity.find(entity.GetID());
	if (groupedEntity != groupPerEntity.end()) {
		auto group = entitiesPerGroup.find(groupedEntity->second);
		if (group != entitiesPerGroup.end()) {
			auto entityInGroup = group->second.find(entity);
			if (entityInGroup != group->second.end()) {
				group->second.erase(entityInGroup);
			}
		}
		groupPerEntity.erase(groupedEntity);
	}
}

void Entity::Kill() {
	worldCordinator->RemoveEntityFromSystems(*this);
};

void Entity::Tag(const string& tag) {
	worldCordinator->TagEntity(*this, tag);
}
bool Entity::HasTag(const string& tag) const {
	return worldCordinator->EntityHasTag(*this, tag);
}
void Entity::Group(const string& group) {
	worldCordinator->GroupEntity(*this, group);
}
bool Entity::HasGroup(const string& group) const {
	return worldCordinator->EntityHasGroup(*this, group);
}
