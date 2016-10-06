/**
* @file Entity.h
*/
#ifndef TUTORIAL_SRC_ENTITY_H_
#define TUTORIAL_SRC_ENTITY_H_
#include <DirectXMath.h>
#include "Animation.h"
#include "Action.h"
#include <list>
#include <memory>
#include <d3d12.h>

class SpriteRenderer;

enum class CollisionShapeType
{
	Circle,
	Rect,
};

union Collision
{
	CollisionShapeType type;
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

class Entity
{
	friend class EntityList;

public:
	enum class State
	{
		New,
		Running,
		Terminated,
	};
public:
	Entity(DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex);
	virtual void Update(float);
	void Draw(SpriteRenderer&) const;
	State GetState() const;

	DirectX::XMFLOAT3 GetPosition() const { return position; }
	
public:
	State state;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;
	DirectX::XMFLOAT3 position;
	float rotation;
	DirectX::XMFLOAT2 scale;
	DirectX::XMFLOAT2 velocity;
	DirectX::XMFLOAT2 thrust;
	DirectX::XMFLOAT4 color;
	Collision collision;
	AnimationController animation;
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
	ScriptEntity(DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex);
	virtual void Update(float);
	void Shot(int type, float dir, float speed);
private:
//	ActionController action;
};

class EntityList
{
public:
	EntityList();
	PlayerEntity* CreatePlayerEntity(DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex);
	ScriptEntity* CreateScriptEntity(DirectX::XMFLOAT3 pos, const AnimationList* p, D3D12_GPU_DESCRIPTOR_HANDLE tex);
	void Update(float);
	void Draw(SpriteRenderer&) const;
private:
	typedef std::shared_ptr<Entity> EntityPtr;

	std::list<EntityPtr> entityList;
	bool sorted;
};

#endif // TUTORIAL_SRC_ENTITY_H_
