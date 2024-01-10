#include "controller_buttons.h"

#if SDL_VERSION_ATLEAST(2, 0, 0)
namespace {
bool trigger_left_is_down;
bool trigger_right_is_down;
} // namespace

ControllerButton ControllerButtonFromSdlEvent(const SDL_Event &event)
{
    switch (event.type) {
        case SDL_CONTROLLERAXISMOTION:
            fprintf(stderr,"src/controller_buttons.cpp:%d event SDL_CONTROLLERAXISMOTION\n",__LINE__);
            switch (event.caxis.axis) {
                case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                    if (event.caxis.value < 8192) { // 25% pressed
                        trigger_left_is_down = false;
                    }
                    if (event.caxis.value > 16384
                        && !trigger_left_is_down) { // 50% pressed
                        trigger_left_is_down = true;
                        return ControllerButton::TRIGGERLEFT;
                    }
                    return ControllerButton::NONE;
                case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
                    if (event.caxis.value < 8192) { // 25% pressed
                        trigger_right_is_down = false;
                    }
                    if (event.caxis.value > 16384
                        && !trigger_right_is_down) { // 50% pressed
                        trigger_right_is_down = true;
                        return ControllerButton::TRIGGERRIGHT;
                    }
                    return ControllerButton::NONE;
            }
            break;
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            fprintf(stderr,"src/controller_buttons.cpp:%d SDL_CONTROLLER_BUTTON=%d\n",__LINE__,event.cbutton.button);
            switch (event.cbutton.button) {
                case SDL_CONTROLLER_BUTTON_A: return ControllerButton::A;
                case SDL_CONTROLLER_BUTTON_B: return ControllerButton::B;
                case SDL_CONTROLLER_BUTTON_X: return ControllerButton::X;
                case SDL_CONTROLLER_BUTTON_Y: return ControllerButton::Y;
                case SDL_CONTROLLER_BUTTON_LEFTSTICK:
                    fprintf(stderr,"src/controller_buttons.cpp:%d SDL_CONTROLLER_BUTTON_LEFTSTICK=%d\n",__LINE__,event.cbutton.button);
                    return ControllerButton::LEFTSTICK;
                case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
                    fprintf(stderr,"src/controller_buttons.cpp:%d SDL_CONTROLLER_BUTTON_RIGHTSTICK=%d\n",__LINE__,event.cbutton.button);
                    return ControllerButton::RIGHTSTICK;
                case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                    fprintf(stderr,"src/controller_buttons.cpp:%d SDL_CONTROLLER_BUTTON_LEFTSHOULDER=%d\n",__LINE__,event.cbutton.button);
                    return ControllerButton::LEFTSHOULDER;
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                    fprintf(stderr,"src/controller_buttons.cpp:%d SDL_CONTROLLER_BUTTON_RIGHTSHOULDER=%d\n",__LINE__,event.cbutton.button);
                    return ControllerButton::RIGHTSHOULDER;
                case SDL_CONTROLLER_BUTTON_START:
                    return ControllerButton::START;
                case SDL_CONTROLLER_BUTTON_BACK:
                    return ControllerButton::SELECT;
                case SDL_CONTROLLER_BUTTON_DPAD_UP: 
                    return ControllerButton::UP;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                    return ControllerButton::DOWN;
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    return ControllerButton::LEFT;
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    return ControllerButton::RIGHT;
                default: break;
            }
        default: 
            fprintf(stderr,"src/controller_buttons.cpp:%d event.type=%d\n",__LINE__,event.type);
            break;
    }
    return ControllerButton::NONE;
}

bool IsControllerButtonDown(
    SDL_GameController *controller, ControllerButton button)
{
    switch (button) {
        case ControllerButton::UP:
            return SDL_GameControllerGetButton(
                controller, SDL_CONTROLLER_BUTTON_DPAD_UP);
        case ControllerButton::DOWN:
            return SDL_GameControllerGetButton(
                controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        case ControllerButton::LEFT:
            return SDL_GameControllerGetButton(
                controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        case ControllerButton::RIGHT:
            return SDL_GameControllerGetButton(
                controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        case ControllerButton::A:
            return SDL_GameControllerGetButton(
                controller, SDL_CONTROLLER_BUTTON_A);
        case ControllerButton::B:
            return SDL_GameControllerGetButton(
                controller, SDL_CONTROLLER_BUTTON_B);
        case ControllerButton::X:
            return SDL_GameControllerGetButton(
                controller, SDL_CONTROLLER_BUTTON_X);
        case ControllerButton::Y:
            return SDL_GameControllerGetButton(
                controller, SDL_CONTROLLER_BUTTON_Y);
        case ControllerButton::LEFTSHOULDER:
            return SDL_GameControllerGetButton(
                controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
        case ControllerButton::RIGHTSHOULDER:
            return SDL_GameControllerGetButton(
                controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
        case ControllerButton::TRIGGERLEFT: return trigger_left_is_down;
        case ControllerButton::TRIGGERRIGHT: return trigger_right_is_down;
        case ControllerButton::LEFTSTICK:
            return SDL_GameControllerGetButton(
                controller, SDL_CONTROLLER_BUTTON_LEFTSTICK);
        case ControllerButton::RIGHTSTICK:
            return SDL_GameControllerGetButton(
                controller, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
        case ControllerButton::START:
            return SDL_GameControllerGetButton(
                controller, SDL_CONTROLLER_BUTTON_START);
        case ControllerButton::SELECT:
            return SDL_GameControllerGetButton(
                controller, SDL_CONTROLLER_BUTTON_BACK);
    }
    return false;
}
#elif defined(KORIKI) && defined(USE_SDL)
ControllerButton ControllerButtonFromSdlEvent(const SDL_Event &event)
{
    switch (event.type) {
        case SDL_JOYBUTTONDOWN:
            switch (event.jbutton.button) {
                case 0: return ControllerButton::B;
                case 1: return ControllerButton::A;
                case 2: return ControllerButton::Y;
                case 3: return ControllerButton::X;
                case 4: return ControllerButton::LEFTSHOULDER;
                case 5: return ControllerButton::RIGHTSHOULDER;
                case 6: return ControllerButton::TRIGGERLEFT;
                case 7: return ControllerButton::TRIGGERRIGHT;
                case 8: return ControllerButton::SELECT;
                case 9: return ControllerButton::START;
                case 10: return ControllerButton::MENU;
                case 13: return ControllerButton::UP;
                case 14: return ControllerButton::DOWN;
                case 15: return ControllerButton::LEFT;
                case 16: return ControllerButton::RIGHT;
                default: break;
            }
        break;
    }
    return ControllerButton::NONE;
}
#endif
