/**
* @file Action.h
*/
#include "Action.h"
#include "Entity.h"

ActionList* GetActionList()
{
	static ActionList actionList;
	if (actionList.list.empty()) {
		actionList.list.resize(1);
		actionList.list[0].seq.push_back(Action(ActionTypeTag::Velocity(), 1.0f, DirectX::XMFLOAT2(0.001f, 0.0f)));
		actionList.list[0].seq.push_back(Action(ActionTypeTag::Interval(), 2.0f));
		actionList.list[0].seq.push_back(Action(ActionTypeTag::Deletion(), 5.0f));
	}
	return &actionList;
}

ActionController::ActionController(ScriptEntity* e) :
	entity(e),
	list(nullptr)
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
	if (!entity || !list | (flags & Finish)) {
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
		}
		break;
	}
}
