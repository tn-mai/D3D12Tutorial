/**
* @file Animation.h
*/
#include "Animation.h"

AnimationController::AnimationController() :
	list(nullptr),
	flags(0)
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
	sequence = seq;
	time = 0;
	flags &= ~Finish;
}

void AnimationController::Update(float delta)
{
	if (flags & Finish) {
		return;
	}
	time += delta;
	const AnimationChip* chip = &list->list[sequence].seq[index];
	while (time >= chip->time) {
		time -= chip->time;
		++index;
		if (index >= list->list[sequence].seq.size()) {
			if (chip->flags & static_cast<int>(AnimationFlag::Loop)) {
				index = 0;
			} else {
				--index;
				flags |= Finish;
			}
			break;
		}
		chip = &list->list[sequence].seq[index];
	}
}

const AnimationChip* AnimationController::GetChip() const
{
	if (!list) {
		return nullptr;
	}
	return &list->list[sequence].seq[index];
}
