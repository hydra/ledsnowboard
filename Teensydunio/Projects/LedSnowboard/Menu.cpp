/*
 * Menu.cpp
 *
 *  Created on: 27 Feb 2014
 *      Author: hydra
 */

#include "WProgram.h"

#include "Config.h"

#include "Input.h"
#include "ArduinoDigitalInput.h"
#include "DebouncedInput.h"

#include "Animator.h"

#include "Scheduling/ScheduledAction.h"
#include "AnimationScheduler.h"

#include "Menu.h"

void stopAnimation(void);
void openNextAnimation(void);

void waitForButtonRelease(DebouncedInput button);

extern ChooseAnimationMenu chooseAnimationMenu;
extern FrequencyMenu frequencyMenu;

ChooseAnimationMenu::ChooseAnimationMenu() {
}

const char *ChooseAnimationMenu::getName() {
    return "chooseAnimation";
}

void ChooseAnimationMenu::onUpButton() {
    stopAnimation();
    openNextAnimation();
}

void ChooseAnimationMenu::onDownButton() {
    stopAnimation();
    openNextAnimation();
}

#define MIN_FREQUENCY 0
#define MAX_FREQUENCY 1000
#define FREQUENCY_STEP 10

FrequencyMenu::FrequencyMenu(AnimationScheduler *animationScheduler, Animator *animator) : animationScheduler(animationScheduler), animator(animator) {
}

const char *FrequencyMenu::getName() {
    return "frequency";
}

void FrequencyMenu::showFrequency() {
    Serial.print("frequency: ");
    Serial.println(animator->timeAxisFrequencyMillis, DEC);
}

void FrequencyMenu::decreaseFrequency(void) {
    if (animator->timeAxisFrequencyMillis >= MIN_FREQUENCY + FREQUENCY_STEP) {
        animator->timeAxisFrequencyMillis -= FREQUENCY_STEP;
    } else {
        animator->timeAxisFrequencyMillis = MIN_FREQUENCY;
    }
}

void FrequencyMenu::increaseFrequency(void) {
    if (animator->timeAxisFrequencyMillis < (MAX_FREQUENCY - FREQUENCY_STEP)) {
        animator->timeAxisFrequencyMillis += FREQUENCY_STEP;
    } else {
        animator->timeAxisFrequencyMillis = MAX_FREQUENCY;
    }
}
void FrequencyMenu::onUpButton() {
    decreaseFrequency();
    animationScheduler->update();
    showFrequency();
}

void FrequencyMenu::onDownButton() {
    increaseFrequency();
    animationScheduler->update();
    showFrequency();
}

MainMenu::MainMenu(Menu **subMenus, uint8_t subMenuCount) :
    subMenus(subMenus),
    subMenuCount(subMenuCount),
    candidateMenuIndex(0) {
}

const char *MainMenu::getName() {
    return "main";
}


void MainMenu::onUpButton() {
    if (candidateMenuIndex > 0) {
        candidateMenuIndex--;
    } else {
        candidateMenuIndex = subMenuCount - 1;
    }
    showCandidateMenu();
}

void MainMenu::onDownButton() {
    if (candidateMenuIndex < (subMenuCount - 1)) {
        candidateMenuIndex++;
    } else {
        candidateMenuIndex = 0;
    }
    showCandidateMenu();
}

void MainMenu::showCandidateMenu(void) {
    Menu* candidate = subMenus[candidateMenuIndex];
    Serial.print("candidate: ");
    Serial.println(candidate->getName());
}

void MainMenu::onSelectButton() {
    selectedMenu = subMenus[candidateMenuIndex];
    shouldChangeMenuNow = true;
}

void MainMenu::onActivate(void) {
    showCandidateMenu();
}

void MainMenu::onRestore(void) {
    showCandidateMenu();
}


MenuStack::MenuStack(
        Menu *initialMenu,
        DebouncedInput &backButton,
        DebouncedInput &selectButton,
        DebouncedInput &upButton,
        DebouncedInput &downButton
) :
    currentMenu(initialMenu),
    backButton(backButton),
    selectButton(selectButton),
    upButton(upButton),
    downButton(downButton) {
}

void MenuStack::initalize(void) {
    currentMenu->activate(NULL);
    currentMenu->onActivate();
}

void MenuStack::process() {
#ifdef DEBUG_MENU_STACK_VERBOSE
    Serial.println("menu stack process - begin");
#endif
    processButtons();
    performUpdateMenu();
#ifdef DEBUG_MENU_STACK_VERBOSE
    Serial.println("menu stack process - end");
#endif
}

void MenuStack::processButtons(void) {
#ifdef DEBUG_MENU_STACK_VERBOSE
    Serial.println("menu stack process buttons - begin");
#endif
    if (selectButton.getValue()) {
        Serial.println("select pressed");
        waitForButtonRelease(selectButton);

        currentMenu->onSelectButton();

        goto processButtonsEnd;
    }

    if (backButton.getValue()) {
        Serial.println("back pressed");
        waitForButtonRelease(backButton);

        if (!currentMenu->hasParent()) {
            return;
        }

        currentMenu->onExit();

        currentMenu = currentMenu->getParent();
        currentMenu->onRestore();

        goto processButtonsEnd;
    }

    if (upButton.getValue()) {
        Serial.println("up pressed");
        waitForButtonRelease(upButton);

        currentMenu->onUpButton();

        goto processButtonsEnd;
    }

    if (downButton.getValue()) {
        Serial.println("down pressed");
        waitForButtonRelease(downButton);

        currentMenu->onDownButton();

        goto processButtonsEnd;
    }

    processButtonsEnd:
    {
#ifdef DEBUG_MENU_STACK_VERBOSE
        Serial.println("menu stack process buttons - end");
#endif
    }
}

void MenuStack::performUpdateMenu() {
    if (!currentMenu->shouldChangeMenu()) {
        return;
    }

    Menu *newMenu = currentMenu->getSelectedMenu();
    currentMenu->resetMenuSelectionStatus();

    currentMenu->onSuspend();

    newMenu->activate(currentMenu);

    currentMenu = newMenu;
    currentMenu->onActivate();
}




