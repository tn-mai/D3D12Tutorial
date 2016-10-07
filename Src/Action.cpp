/**
* @file Action.h
*/
#include "Action.h"
#include "Entity.h"

const Action actEmpty[] = {
	Action(ActionTypeTag::Interval(), 0.0f)
};

const Action actPlayerBullet[] = {
	Action(ActionTypeTag::Velocity(), 0.0f, DirectX::XMFLOAT2(0.0f, -0.01f)),
};

const Action actEnemy00[] = {
	Action(ActionTypeTag::Velocity(), 1.0f, DirectX::XMFLOAT2(0.0f, 0.005f)),
	Action(ActionTypeTag::Thrust(), 0.0f, DirectX::XMFLOAT2(0.0001f, 0.0f))
};

const Action actEnemyExplosion[] = {
	Action(ActionTypeTag::Velocity(), 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f)),
	Action(ActionTypeTag::Thrust(), 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f)),
	Action(ActionTypeTag::Animation(), 0.0f, 1),
	Action(ActionTypeTag::Interval(), 1.0f),
	Action(ActionTypeTag::Deletion(), 0.0f)
};

ActionList* GetActionList()
{
	static ActionList actionList;
	if (actionList.list.empty()) {
		actionList.list.resize(4);
		for (auto& e : actEmpty) { actionList.list[0].seq.push_back(e); }
		for (auto& e : actPlayerBullet) { actionList.list[1].seq.push_back(e); }
		for (auto& e : actEnemy00) { actionList.list[2].seq.push_back(e); }
		for (auto& e : actEnemyExplosion) { actionList.list[3].seq.push_back(e); }
	}
	return &actionList;
}

ActionController::ActionController(ScriptEntity* e) :
	entity(e),
	list(nullptr),
	flags(0)
{
}

void ActionController::SetList(const ActionList* p)
{
	list = p;
	SetSequence(0);
}

void ActionController::SetSequence(int seq)
{
	flags &= ~Finish;
	index = 0;
	sequence = seq;
	time = 0.0f;
}

void ActionController::Update(float delta)
{
	if (!entity || !list || list->list.empty() || list->list[sequence].seq.empty() || (flags & Finish)) {
		return;
	}
	if (entity->GetState() == Entity::State::AbortRequested) {
		return;
	}
	time += delta;
	for (;;) {
		const Action& action = list->list[sequence].seq[index];
		switch (action.type & 0x0f) {
		case ActionType_Velocity:
			entity->velocity.x = action.u.value.x;
			entity->velocity.y = action.u.value.y;
			break;
		case ActionType_Thrust:
			entity->thrust.x = action.u.value.x;
			entity->thrust.y = action.u.value.y;
			break;
		case ActionType_Rotation:
			entity->rotation = action.u.rot;
			break;
		case ActionType_Animation: 
			entity->animation.SetSequence(action.u.animationSeq);
			break;
		case ActionType_Shot:
			entity->Shot(action.u.shot.type, action.u.shot.dir, action.u.shot.speed);
			break;
		case ActionType_Deletion:
			entity->Abort();
			break;
		}
		if (time >= action.time) {
			time -= action.time;
			++index;
			if (index >= list->list[sequence].seq.size()) {
				flags |= Finish;
				break;
			}
		} else {
			break;
		}
	}
}
