/*
 * Menu.h
 *
 *  Created on: 27 Feb 2014
 *      Author: hydra
 */

#ifndef MENU_H_
#define MENU_H_

class Menu {
public:
    Menu() : parent(NULL), shouldChangeMenuNow(false), selectedMenu(NULL) {
    }
    virtual ~Menu() {};

    virtual const char *getName() = 0;

    virtual bool hasParent() { return parent != NULL; };
    virtual Menu *getParent() { return parent; };

    virtual bool shouldChangeMenu() { return shouldChangeMenuNow; };
    virtual Menu *getSelectedMenu() { return selectedMenu; };
    virtual void resetMenuSelectionStatus() {
        selectedMenu = NULL;
        shouldChangeMenuNow = false;
    }

    virtual void activate(Menu *currentMenu) {
        parent = currentMenu;
    }

    virtual void onSelectButton() { showName(); Serial.println("onSelectButton"); };
    virtual void onUpButton() { showName(); Serial.println("onUpButton"); };
    virtual void onDownButton() { showName(); Serial.println("onDownButton"); };

    virtual void onActivate() { showName(); Serial.println("onActivate"); };
    virtual void onExit() { showName(); Serial.println("onExit"); };

    virtual void onRestore() { showName(); Serial.println("onRestore"); };
    virtual void onSuspend() { showName(); Serial.println("onSuspend"); };

private:
    void showName() {
        Serial.print(getName());
    }

    Menu *parent;

protected:
    bool shouldChangeMenuNow;
    Menu *selectedMenu;
};

class ChooseAnimationMenu : public Menu {
public:
    ChooseAnimationMenu();

    const char *getName();

    void onUpButton();
    void onDownButton();
};

class FrequencyMenu : public Menu {
public:
    FrequencyMenu(AnimationScheduler *animationScheduler, Animator *animator);

    const char *getName();

    void onUpButton();
    void onDownButton();

private:
    void showFrequency(void);

    void decreaseFrequency(void);
    void increaseFrequency(void);

    AnimationScheduler *animationScheduler;
    Animator *animator;
};


class MainMenu : public Menu {
public:
    MainMenu(Menu **subMenus, uint8_t subMenuCount);

    const char *getName();

    void onActivate();
    void onRestore();

    void onUpButton();
    void onDownButton();
    void onSelectButton();

private:
    void showCandidateMenu();

    Menu **subMenus;
    uint8_t subMenuCount;

    uint8_t candidateMenuIndex;
};

class MenuStack {
public:
    MenuStack(
            Menu *initialMenu,
            DebouncedInput &backButton,
            DebouncedInput &selectButton,
            DebouncedInput &upButton,
            DebouncedInput &downButton
    );

    void initalize();
    void process();

private:
    void processButtons(void);
    void performUpdateMenu(void);

    class Menu *currentMenu;
    DebouncedInput &backButton;
    DebouncedInput &selectButton;
    DebouncedInput &upButton;
    DebouncedInput &downButton;
};



#endif /* MENU_H_ */
