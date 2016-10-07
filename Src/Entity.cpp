/**
* @file Entity.cpp
*/
#include "Entity.h"
#include "Sprite.h"
#include <algorithm>

bool CircleCircle(const Entity& a, const Entity& b)
{
	const float dx = a.position.x - b.position.x;
	const float dy = a.position.y - b.position.y;
	const float ra = a.collision.circle.radius;
	const float rb = b.collision.circle.radius;
	return (dx * dx + dy + dy) < (ra * ra + rb * rb);
}

bool RectCircle(const Entity& a, const Entity& b)
{
	const DirectX::XMFLOAT2 aLT(a.position.x + a.collision.rect.leftTop.x, a.position.y + a.collision.rect.leftTop.y);
	const DirectX::XMFLOAT2 aRB(a.position.x + a.collision.rect.rightBottom.x, a.position.y + a.collision.rect.rightBottom.y);
	DirectX::XMFLOAT2 p;
	p.x = std::min(std::max(b.position.x, aLT.x), aRB.x);
	p.y = std::min(std::max(b.position.y, aLT.y), aRB.y);
	const float dx = p.x - b.position.x;
	const float dy = p.y - b.position.y;
	const float rb = b.collision.circle.radius;
	return (dx * dx + dy * dy) < (rb * rb);
}
bool CircleRect(const Entity& a, const Entity& b) { return RectCircle(b, a); }

bool RectRect(const Entity& a, const Entity& b)
{
	const DirectX::XMFLOAT2 aLT(a.position.x + a.collision.rect.leftTop.x, a.position.y + a.collision.rect.leftTop.y);
	const DirectX::XMFLOAT2 aRB(a.position.x + a.collision.rect.rightBottom.x, a.position.y + a.collision.rect.rightBottom.y);
	const DirectX::XMFLOAT2 bLT(b.position.x + b.collision.rect.leftTop.x, b.position.y + b.collision.rect.leftTop.y);
	const DirectX::XMFLOAT2 bRB(b.position.x + b.collision.rect.rightBottom.x, b.position.y + b.collision.rect.rightBottom.y);
	if (aRB.x < bLT.x || aLT.x > bRB.x) return false;
	if (aRB.y < bLT.y || aLT.y > bRB.y) return false;
	return true;
}

void CollisionDetector::AddSolution(uint32_t g0, uint32_t g1, CollisionSolution& func)
{
	solutionMap.insert({ static_cast<uint64_t>(g0) + (static_cast<uint64_t>(g1) << 32), func });
	solutionMap.insert({ static_cast<uint64_t>(g1) + (static_cast<uint64_t>(g0) << 32), func });
}

void CollisionDetector::Detect(EntityLinkedList& lhs, EntityLinkedList& rhs)
{
	if (lhs.empty() || rhs.empty()) {
		return;
	}
	CollisionSolution func;
	{
		const uint64_t gidLeft = lhs.front()->collision.groupId;
		const uint64_t gidRight = rhs.front()->collision.groupId;
		const uint64_t key = gidLeft + (gidRight << 32);
		auto itr = solutionMap.find(key);
		if (itr == solutionMap.end()) {
			return;
		}
		func = itr->second;
	}
	static bool(*const funcList[2][2])(const Entity&, const Entity&) = {
		{ CircleCircle, CircleRect },
		{ RectCircle, RectRect },
	};
	for (auto itrLeft = lhs.begin(); itrLeft != lhs.end(); ++itrLeft) {
		if ((*itrLeft)->state == Entity::State::AbortRequested) {
			continue;
		}
		const int typeLeft = static_cast<int>((*itrLeft)->collision.type);
		for (auto itrRight = rhs.begin(); itrRight != rhs.end(); ++itrRight) {
			if ((*itrRight)->state == Entity::State::AbortRequested) {
				continue;
			}
			const int typeRight = static_cast<int>((*itrRight)->collision.type);
			if (funcList[typeLeft][typeRight](**itrLeft, **itrRight)) {
				func(**itrLeft, **itrRight);
			}
		}
	}
}

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

PlayerEntity* EntityList::CreatePlayerEntity(int groupId, DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex)
{
	std::shared_ptr<PlayerEntity> entity(new PlayerEntity(pos, p, tex));
	entity->collision.groupId = groupId;
	entity->collision.circle.radius = 0.1f;
	entityList.push_back(entity);
	AddToCollisionGroup(entity);
	sorted = false;
	return entity.get();
}

ScriptEntity* EntityList::CreateScriptEntity(int groupId, DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex, const ActionList* act)
{
	std::shared_ptr<ScriptEntity> entity(new ScriptEntity(pos, p, tex, act));
	entity->collision.groupId = groupId;
	entity->collision.circle.radius = 0.1f;
	entityList.push_back(entity);
	AddToCollisionGroup(entity);
	sorted = false;
	return entity.get();
}

void EntityList::AddCollisionSolution(int g0, int g1, CollisionSolution func)
{
	collisionDetector.AddSolution(g0, g1, func);
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
	if (entityListForCollision.size() >= 2) {
		auto end = entityListForCollision.end() - 1;
		for (auto i = entityListForCollision.begin(); i != end; ++i) {
			for (auto j = i + 1; j != entityListForCollision.end(); ++j) {
				collisionDetector.Detect(*i, *j);
			}
		}
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
		[groupId](const EntityLinkedList& e) { return !e.empty() && (e.front()->collision.groupId == groupId); }
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