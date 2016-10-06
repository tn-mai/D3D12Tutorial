/**
* @file Animation.h
*/
#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED
#include <DirectXMath.h>
#include <vector>

enum class AnimationFlag
{
	None = 0x00,
	HFlip = 0x01,
	VFlip = 0x02,
	Loop = 0x04,
};

struct AnimationChip
{
	float time;
	int flags;
	DirectX::XMFLOAT2 leftTop;
	DirectX::XMFLOAT2 size;
};

struct AnimationSequence
{
	std::vector<AnimationChip> seq;
};

struct AnimationList
{
	std::vector<AnimationSequence> list;
};

class AnimationController
{
public:
	AnimationController();
	void SetList(const AnimationList* p);
	void SetSequence(int seq);
	void Update(float delta);
	const AnimationChip* GetChip() const;
private:
	enum Flag {
		Finish = 0x01,
	};
	const AnimationList* list;
	int flags;
	int index;
	int sequence;
	float time;
};

#endif // ANIMATION_H_INCLUDED
