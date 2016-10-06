/**
* @file Entity.cpp
*/
#include "Entity.h"
#include "Sprite.h"

PlayerEntity::PlayerEntity(DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex) : Entity(pos, p, tex) {}

void PlayerEntity::Update(float delta)
{
	Entity::Update(delta);
}

ScriptEntity::ScriptEntity(DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex) : Entity(pos, p, tex) {}

void ScriptEntity::Update(float delta)
{
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
	sorted = false;
	return entity.get();
}

ScriptEntity* EntityList::CreateScriptEntity(DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex)
{
	std::shared_ptr<ScriptEntity> entity(new ScriptEntity(pos, p, tex));
	entityList.push_back(entity);
	sorted = false;
	return entity.get();
}

void EntityList::Update(float delta)
{
	if (!sorted) {
		entityList.sort([](const EntityPtr& lhs, const EntityPtr& rhs) { return lhs->GetPosition().z < rhs->GetPosition().z; });
		sorted = true;
	}
	for (auto& e : entityList) {
		e->Update(delta);
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

