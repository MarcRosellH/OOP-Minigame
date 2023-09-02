#pragma once
#ifndef __INPUT_H__
#define __INPUT_H__

#include <glm\glm.hpp>

class App;

enum Mouse_Button
{
	LEFT,
	RIGHT,
	MOUSE_BUTTON_COUNT
};

enum Key 
{
    K_SPACE,
    K_0, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9,
    K_A, K_B, K_C, K_D, K_E, K_F, K_G, K_H, K_I, K_J, K_K, K_L, K_M,
    K_N, K_O, K_P, K_Q, K_R, K_S, K_T, K_U, K_V, K_W, K_X, K_Y, K_Z,
    K_ENTER, K_ESCAPE,
    KEY_COUNT
};

enum Button_State 
{
    BUTTON_IDLE,
    BUTTON_PRESS,
    BUTTON_PRESSED,
    BUTTON_RELEASE
};

class InputModule : public Module
{
public:
    // Public methods
    InputModule(App* _app);
    virtual ~InputModule();

    Update_State pre_update() override;
    Update_State post_update() override;

public:
    // Private methods

public:
    // Public data
    glm::vec2	    mouse_position;
    glm::vec2	    mouse_delta;
    Button_State    mouse_buttons[MOUSE_BUTTON_COUNT];
    Button_State    keys[KEY_COUNT];

private:
    // Private data
};

#endif // !__INPUT_H__