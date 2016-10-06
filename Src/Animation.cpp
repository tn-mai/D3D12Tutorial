/**
* @file Animation.h
*/
#include "Animation.h"

AnimationController::AnimationController() :
	list(nullptr)
{
}


void AnimationController::SetList(const AnimationList* p)
{
	list = p;
	SetSequence(0);
}

void AnimationController::SetSequence(int seq)
{
	index = 0;
	sequence = 0;
	time = 0;
	flags &= ~Finish;
}

void AnimationController::Update(float delta)
{
	if (flags & Finish) {
		return;
	}
	const AnimationChip& chip = list->list[sequence].seq[index];
	time += delta;
	if (time >= chip.time) {
		++index;
		if (index >= list->list[sequence].seq.size()) {
			if (chip.flags & static_cast<int>(AnimationFlag::Loop)) {
				index = 0;
				time = time - chip.time;
			} else {
				--index;
				flags |= Finish;
			}
		} else {
			time = time - chip.time;
		}
	}
}

const AnimationChip* AnimationController::GetChip() const
{
	if (!list) {
		return nullptr;
	}
	return &list->list[sequence].seq[index];
}
