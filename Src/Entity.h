/**
* @file Entity.h
*/
#ifndef TUTORIAL_SRC_ENTITY_H_
#define TUTORIAL_SRC_ENTITY_H_
#include "Animation.h"
#include "Action.h"
#include <list>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <d3d12.h>
#include <DirectXMath.h>

class SpriteRenderer;
class Entity;

enum class CollisionShapeType
{
	Circle,
	Rect,
};

struct Collision
{
	Collision() : type(CollisionShapeType::Circle), groupId(0), circle{ 0.1f } {}

	CollisionShapeType type;
	int groupId;
	union {
		struct Circle
		{
			float radius;
		} circle;
		struct Rect
		{
			DirectX::XMFLOAT2 leftTop;
			DirectX::XMFLOAT2 rightBottom;
		} rect;
	};
};

typedef std::shared_ptr<Entity> EntityPtr;
typedef std::list<EntityPtr> EntityLinkedList;
typedef std::function<void(Entity&, Entity&)> CollisionSolution;

class CollisionDetector
{
public:
	void AddSolution(uint32_t g0, uint32_t g1, CollisionSolution& func);
	void Detect(EntityLinkedList& lhs, EntityLinkedList& rhs);
private:
	std::map<uint64_t, CollisionSolution> solutionMap;
};

class Entity
{
	friend class EntityList;

public:
	enum class State
	{
		New,
		Running,
		AbortRequested,
		Terminated,
	};
public:
	Entity(DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex);
	virtual void Update(float);
	void Draw(SpriteRenderer&) const;
	State GetState() const;

	DirectX::XMFLOAT3 GetPosition() const { return position; }
	
public:
	typedef std::shared_ptr<Entity> Ptr;
	typedef std::list<Ptr>::iterator ListItr;

	State state;

	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;
	DirectX::XMFLOAT3 position;
	float rotation;
	DirectX::XMFLOAT2 scale;
	DirectX::XMFLOAT2 velocity;
	DirectX::XMFLOAT2 thrust;
	DirectX::XMFLOAT4 color;
	AnimationController animation;

	Collision collision;
	ListItr collisionItr;
};

/**
* ユーザー入力によって操作されるプレイヤーのエンティティクラス.
*/
class PlayerEntity : public Entity
{
public:
	PlayerEntity(DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex);
	virtual void Update(float);
private:
	//PlayerController controller;
};

/**
* 動作制御スクリプトによって操作されるエンティティクラス.
*/
class ScriptEntity : public Entity
{
public:
	ScriptEntity(DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex, const ActionList* act);
	virtual void Update(float);
	void SetAction(int n) { action.SetSequence(n); }
	void Shot(int type, float dir, float speed);
	void Abort() { state = State::AbortRequested; }
private:
	ActionController action;
};

class EntityList
{
public:
	EntityList();
	PlayerEntity* CreatePlayerEntity(int groupId, DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex);
	ScriptEntity* CreateScriptEntity(int groupId, DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex, const ActionList* act);
	void AddCollisionSolution(int g0, int g1, CollisionSolution func);
	void Update(float);
	void Draw(SpriteRenderer&) const;

private:
	typedef std::list<Entity::Ptr> EntityLinkedList;

	EntityLinkedList* FindCollisionGroup(int groupId);
	void AddToCollisionGroup(Entity::Ptr);

private:
	EntityLinkedList entityList;
	std::vector<EntityLinkedList> entityListForCollision;
	CollisionDetector collisionDetector;
	bool sorted;
};

#endif // TUTORIAL_SRC_ENTITY_H_
