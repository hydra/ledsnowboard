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

#include "Menu.h"

void stopAnimation(void);
void openNextAnimation(void);
void waitForButtonRelease(DebouncedInput button);

extern ChooseAnimationMenu chooseAnimationMenu;

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

MainMenu::MainMenu() {
}

const char *MainMenu::getName() {
    return "main";
}

void MainMenu::onSelectButton() {
    shouldChangeMenuNow = true;
    selectedMenu = &chooseAnimationMenu;
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




