/**
* @file Action.h
*/
#ifndef TUTORIAL_SRC_ACTION_H_
#define TUTORIAL_SRC_ACTION_H_
#include <DirectXMath.h>
#include <vector>

class ScriptEntity;

enum ActionType
{
	ActionType_Velocity = 0x00,
	ActionType_Thrust = 0x01,
	ActionType_Rotation = 0x02,
	ActionType_Animation = 0x03,
	ActionType_Shot = 0x04,
	ActionType_Interval = 0x05,
	ActionType_Deletion = 0x06,

	RotationType_Front = 0x00,
	RotationType_Fixed = 0x10,
};

namespace ActionTypeTag {
struct Velocity {};
struct Thrust {};
struct Rotation {};
struct Animation {};
struct Shot {};
struct Interval {};
struct Deletion {};
} // namespace ActionTypeTag

struct Action
{
	constexpr Action(ActionTypeTag::Velocity tag, float t, DirectX::XMFLOAT2 v) : type(ActionType_Velocity), time(t), u(tag, v) {}
	constexpr Action(ActionTypeTag::Thrust tag, float t, DirectX::XMFLOAT2 v) : type(ActionType_Thrust), time(t), u(tag, v) {}
	constexpr Action(ActionTypeTag::Rotation tag, float t, float r) : type(ActionType_Rotation), time(t), u(tag, r) {}
	constexpr Action(ActionTypeTag::Animation tag, float t, int seq) : type(ActionType_Animation), time(t), u(tag, seq) {}
	constexpr Action(ActionTypeTag::Shot tag, float t, int type, float dir, float speed) : type(ActionType_Shot), time(t), u(tag, type, dir, speed) {}
	constexpr Action(ActionTypeTag::Interval tag, float t) : type(ActionType_Interval), time(t), u(tag) {}
	constexpr Action(ActionTypeTag::Deletion tag, float t) : type(ActionType_Deletion), time(t), u(tag) {}

	ActionType type;
	float time;
	union Value {
		constexpr Value(ActionTypeTag::Velocity tag, DirectX::XMFLOAT2 v) : value(v) {}
		constexpr Value(ActionTypeTag::Thrust tag,  DirectX::XMFLOAT2 v) : value(v) {}
		constexpr Value(ActionTypeTag::Rotation tag, float r) : rot(r) {}
		constexpr Value(ActionTypeTag::Animation tag, int seq) : animationSeq(seq) {}
		constexpr Value(ActionTypeTag::Shot tag, int type, float dir, float speed) : shot{ type, dir, speed } {}
		constexpr Value(ActionTypeTag::Interval tag) : rot(0) {}
		constexpr Value(ActionTypeTag::Deletion tag) : rot(0) {}

		float rot;
		int animationSeq;
		struct {
			int type;
			float dir;
			float speed;
		} shot;
		DirectX::XMFLOAT2 value;
	} u;
};

struct ActionSequence
{
	std::vector<Action> seq;
};

struct ActionList
{
	std::vector<ActionSequence> list;
};

class ActionController
{
public:
	explicit ActionController(ScriptEntity*);
	void SetList(const ActionList* p);
	void SetSequence(int seq);
	void Update(float delta);
private:
	enum Flag {
		Finish = 0x01,
	};
	ScriptEntity* entity;
	const ActionList* list;
	int flags;
	int index;
	int sequence;
	float time;
};

ActionList* GetActionList();

#endif // TUTORIAL_SRC_ACTION_H_
