/**
* @file AnimationData.h
*/
#ifndef ANIMATIONDATA_H_INCLUDED
#define ANIMATIONDATA_H_INCLUDED
#include "Animation.h"
#include <vector>

enum class PlayerAnimationId
{
	Standard,
	Left,
	Right,
	Explosion,
};

enum class PlayerShotAnimationId
{
	Standard,
};

enum class EnemyAnimationId
{
	Standard00,
	Explosion,
};

enum class EnemyShotAnimationId
{
	Standard,
};

enum class AnimationDataNo
{
	Player,
	PlayerShot,
	Enemy,
	EnemyShot,
};

class AnimationData
{
public:
	AnimationData();
	void Init();
	const AnimationList* GetData(int dataNo) const;
private:
	std::vector<AnimationList> list;
};

#endif // ANIMATIONDATA_H_INCLUDED
