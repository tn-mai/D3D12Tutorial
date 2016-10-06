/**
* @file AnimationData.cpp
*/
#include "AnimationData.h"
#include <DirectXMath.h>

using namespace DirectX;

namespace /* unnamed */ {

#define FLAG(f) static_cast<int>(AnimationFlag::f)

const AnimationChip anmPlayerStandard[] = {
	{ 0.5f, FLAG(None), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.125f, 0.125f) },
	{ 0.5f, FLAG(HFlip) | FLAG(Loop), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.125f, 0.125f) },
};
const AnimationChip anmPlayerLeft[] = {
	{ 0.1f, FLAG(None), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.125f, 0.125f) },
	{ 0.1f, FLAG(Loop), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.125f, 0.125f) },
};
const AnimationChip anmPlayerRight[] = {
	{ 0.1f, FLAG(None), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.125f, 0.125f) },
	{ 0.1f, FLAG(Loop), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.125f, 0.125f) },
};

const AnimationChip anmPlayerShot[] = {
	{ 0.1f, FLAG(None), XMFLOAT2(0.125f, 0.0f), XMFLOAT2(0.125f, 0.125f) },
	{ 0.1f, FLAG(Loop) | FLAG(HFlip), XMFLOAT2(0.125f, 0.0f), XMFLOAT2(0.125f, 0.125f) },
};

const AnimationChip anmEnemyStandard[] = {
	{ 0.1f, FLAG(None), XMFLOAT2(0.75f, 0.0f), XMFLOAT2(0.125f, 0.125f) },
	{ 0.1f, FLAG(None), XMFLOAT2(0.75f, 0.125f), XMFLOAT2(0.125f, 0.125f) },
	{ 0.1f, FLAG(None), XMFLOAT2(0.75f, 0.25f), XMFLOAT2(0.125f, 0.125f) },
	{ 0.1f, FLAG(None), XMFLOAT2(0.75f, 0.375f), XMFLOAT2(0.125f, 0.125f) },
	{ 0.1f, FLAG(None), XMFLOAT2(0.75f, 0.5f), XMFLOAT2(0.125f, 0.125f) },
	{ 0.1f, FLAG(None), XMFLOAT2(0.75f, 0.625f), XMFLOAT2(0.125f, 0.125f) },
	{ 0.1f, FLAG(None), XMFLOAT2(0.75f, 0.125f), XMFLOAT2(0.125f, 0.125f) },
	{ 0.1f, FLAG(Loop), XMFLOAT2(0.75f, 0.75f), XMFLOAT2(0.125f, 0.125f) },
};

const AnimationChip anmEnemyShot[] = {
	{ 0.1f, FLAG(None), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.125f, 0.125f) },
	{ 0.1f, FLAG(Loop), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.125f, 0.125f) },
};

} // unnamed namespace

AnimationData::AnimationData()
{
}

void AnimationData::Init()
{
	list.resize(4);
	list[0].list.resize(3);
	for (auto& e : anmPlayerStandard) { list[0].list[0].seq.push_back(e); }
	for (auto& e : anmPlayerLeft) { list[0].list[1].seq.push_back(e); }
	for (auto& e : anmPlayerRight) { list[0].list[2].seq.push_back(e); }
	list[1].list.resize(1);
	for (auto& e : anmPlayerShot) { list[1].list[0].seq.push_back(e); }
	list[2].list.resize(1);
	for (auto& e : anmEnemyStandard) { list[2].list[0].seq.push_back(e); }
	list[3].list.resize(1);
	for (auto& e : anmEnemyShot) { list[3].list[0].seq.push_back(e); }
}

const AnimationList* AnimationData::GetData(int dataNo) const
{
	return &list[dataNo];
}
