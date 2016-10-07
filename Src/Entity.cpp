/**
* @file Entity.cpp
*/
#include "Entity.h"
#include "Sprite.h"
#include <algorithm>

PlayerEntity::PlayerEntity(DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex) : Entity(pos, p, tex) {}

void PlayerEntity::Update(float delta)
{
	Entity::Update(delta);
}

ScriptEntity::ScriptEntity(DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex, const ActionList*  act) :
	Entity(pos, p, tex),
	action(this)
{
	action.SetList(act);
}

void ScriptEntity::Update(float delta)
{
	action.Update(delta);
	Entity::Update(delta);
}

void ScriptEntity::Shot(int type, float dir, float speed) {}

Entity::Entity(DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex) :
	state(State::Running),
	srvHandle(tex),
	position(pos),
	rotation(0.0f),
	scale(1.0f, 1.0f),
	velocity(0.0f, 0.0f),
	thrust(0.0f, 0.0f),
	color(1.0f, 1.0f, 1.0f, 1.0f)
{
	animation.SetList(p);
}

void Entity::Update(float delta)
{
	position.x += velocity.x;
	position.y += velocity.y;
	velocity.x += thrust.x;
	velocity.y += thrust.y;

	animation.Update(delta);
}

void Entity::Draw(SpriteRenderer&) const
{
}

Entity::State Entity::GetState() const
{
	return state;
}

EntityList::EntityList() :
	sorted(false)
{
}

PlayerEntity* EntityList::CreatePlayerEntity(DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex)
{
	std::shared_ptr<PlayerEntity> entity(new PlayerEntity(pos, p, tex));
	entityList.push_back(entity);
	AddToCollisionGroup(entity);
	sorted = false;
	return entity.get();
}

ScriptEntity* EntityList::CreateScriptEntity(DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex, const ActionList* act)
{
	std::shared_ptr<ScriptEntity> entity(new ScriptEntity(pos, p, tex, act));
	entityList.push_back(entity);
	AddToCollisionGroup(entity);
	sorted = false;
	return entity.get();
}

void EntityList::Update(float delta)
{
	if (!sorted) {
		entityList.sort([](const Entity::Ptr& lhs, const Entity::Ptr& rhs) { return lhs->GetPosition().z < rhs->GetPosition().z; });
		sorted = true;
	}
	for (auto& e : entityList) {
		e->Update(delta);
	}
	for (auto itr = entityList.begin(); itr != entityList.end();) {
		if ((*itr)->state == Entity::State::AbortRequested) {
			EntityLinkedList* group = FindCollisionGroup((*itr)->collision.groupId);
			if (group) {
				group->erase((*itr)->collisionItr);
			}
			itr = entityList.erase(itr);
		} else {
			++itr;
		}
	}
}

void EntityList::Draw(SpriteRenderer& renderer) const
{
	for (auto& e : entityList) {
		const AnimationChip* chip = e->animation.GetChip();
		if (chip) {
			const SpriteCell cell = {
				DirectX::XMFLOAT2(chip->leftTop),
				DirectX::XMFLOAT2(chip->size),
				DirectX::XMFLOAT2(chip->size),
			};
			DirectX::XMFLOAT2 scale = e->scale;
			if (chip->flags & static_cast<int>(AnimationFlag::HFlip)) {
				scale.x *= -1.0f;
			}
			if (chip->flags & static_cast<int>(AnimationFlag::VFlip)) {
				scale.y *= -1.0f;
			}
			renderer.AddRenderingInfo(cell, e->srvHandle, e->position, scale, e->rotation, e->color);
		}
	}
}

EntityList::EntityLinkedList* EntityList::FindCollisionGroup(int groupId)
{
	auto group = std::find_if(
		entityListForCollision.begin(),
		entityListForCollision.end(),
		[groupId](const EntityLinkedList& e) { return !e.empty() || (e.front()->collision.groupId == groupId); }
	);
	if (group != entityListForCollision.end()) {
		return &*group;
	}
	return nullptr;
}

void EntityList::AddToCollisionGroup(Entity::Ptr entity)
{
	EntityLinkedList* group = FindCollisionGroup(entity->collision.groupId);
	if (!group) {
		entityListForCollision.emplace_back();
		group = &entityListForCollision.back();
	}
	group->push_back(entity);
	entity->collisionItr = group->end();
	--entity->collisionItr;
}