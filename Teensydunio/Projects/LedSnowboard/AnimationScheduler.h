/*
 * AnimationScheduler.h
 *
 *  Created on: 27 Feb 2014
 *      Author: hydra
 */

#ifndef ANIMATIONSCHEDULER_H_
#define ANIMATIONSCHEDULER_H_

class AnimationScheduler {
public:
    AnimationScheduler(
        ScheduledAction *scheduledAction,
        Animator *animator
    );

    void update(void);

private:
    ScheduledAction *scheduledAction;
    Animator *animator;
};

#endif /* ANIMATIONSCHEDULER_H_ */
