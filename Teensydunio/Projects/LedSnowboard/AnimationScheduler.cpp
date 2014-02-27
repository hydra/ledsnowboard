/*
 * AnimationScheduler.cpp
 *
 *  Created on: 27 Feb 2014
 *      Author: hydra
 */

#include "WProgram.h"

#include "Scheduling/ScheduledAction.h"

#include "Animator.h"

#include "AnimationScheduler.h"

AnimationScheduler::AnimationScheduler(ScheduledAction *scheduledAction,
        Animator *animator) :
                scheduledAction(scheduledAction),
                animator(animator) {
}

void AnimationScheduler::update(void) {
#ifdef OVERRIDE_ANIMATION_FREQUENCY
    scheduledAction->setDelayMillis(1);
#else
    scheduledAction->setDelayMillis(animator->timeAxisFrequencyMillis);
#endif
    scheduledAction->reset();
}


