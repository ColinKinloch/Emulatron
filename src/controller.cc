#include "controller.hh"
#include <iostream>

std::map<unsigned, Controller*> Controller::controllers;

Controller::Controller(unsigned id)
{
  controller = SDL_GameControllerOpen(id);
  poll();
}

const char* Controller::getName()
{
  return SDL_GameControllerName(controller);
}

uint8_t Controller::getButton(unsigned button)
{
  return SDL_GameControllerGetButton(controller, retroToSDLButton(button));
}
uint8_t Controller::getHat(unsigned hat)
{
  return getButton(hat);
}
int16_t Controller::getAxis(unsigned axis, unsigned stick)
{
  return SDL_GameControllerGetAxis(controller, retroToSDLAxis(axis, stick));
}
SDL_GameController* Controller::getSDLGameController()
{
  return controller;
}
SDL_GameControllerAxis Controller::retroToSDLAxis(unsigned id, unsigned index)
{
  switch(index)
  {
    case RETRO_DEVICE_INDEX_ANALOG_LEFT: switch(id)
    {
      case RETRO_DEVICE_ID_ANALOG_X: return SDL_CONTROLLER_AXIS_LEFTX;
      case RETRO_DEVICE_ID_ANALOG_Y: return SDL_CONTROLLER_AXIS_LEFTY;
      default: return SDL_CONTROLLER_AXIS_INVALID;
    }
    case RETRO_DEVICE_INDEX_ANALOG_RIGHT: switch(id)
    {
      case RETRO_DEVICE_ID_ANALOG_X: return SDL_CONTROLLER_AXIS_RIGHTX;
      case RETRO_DEVICE_ID_ANALOG_Y: return SDL_CONTROLLER_AXIS_RIGHTY;
      default: return SDL_CONTROLLER_AXIS_INVALID;
    }
    default: switch(id)
    {
      case RETRO_DEVICE_ID_JOYPAD_L2: return SDL_CONTROLLER_AXIS_TRIGGERLEFT;
      case RETRO_DEVICE_ID_JOYPAD_R2: return SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
      default: return SDL_CONTROLLER_AXIS_INVALID;
    }
  }
  return SDL_CONTROLLER_AXIS_INVALID;
}
SDL_GameControllerButton Controller::retroToSDLButton(unsigned id)
{
  switch(id)
  {
    case RETRO_DEVICE_ID_JOYPAD_A: return SDL_CONTROLLER_BUTTON_B;
    case RETRO_DEVICE_ID_JOYPAD_B: return SDL_CONTROLLER_BUTTON_A;
    case RETRO_DEVICE_ID_JOYPAD_X: return SDL_CONTROLLER_BUTTON_Y;
    case RETRO_DEVICE_ID_JOYPAD_Y: return SDL_CONTROLLER_BUTTON_X;
    case RETRO_DEVICE_ID_JOYPAD_SELECT: return SDL_CONTROLLER_BUTTON_BACK;
    //case RETRO_DEVICE_ID_JOYPAD_RGUI: return SDL_CONTROLLER_BUTTON_GUIDE;
    case RETRO_DEVICE_ID_JOYPAD_START: return SDL_CONTROLLER_BUTTON_START;
    case RETRO_DEVICE_ID_JOYPAD_L3: return SDL_CONTROLLER_BUTTON_LEFTSTICK;
    case RETRO_DEVICE_ID_JOYPAD_R3: return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
    case RETRO_DEVICE_ID_JOYPAD_L: return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
    case RETRO_DEVICE_ID_JOYPAD_R: return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
    case RETRO_DEVICE_ID_JOYPAD_UP: return SDL_CONTROLLER_BUTTON_DPAD_UP;
    case RETRO_DEVICE_ID_JOYPAD_DOWN: return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    case RETRO_DEVICE_ID_JOYPAD_LEFT: return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    case RETRO_DEVICE_ID_JOYPAD_RIGHT: return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
    //case RETRO_DEVICE_ID_JOYPAD_L2: return SDL_CONTROLLER_AXIS_TRIGGERLEFT;
    //case RETRO_DEVICE_ID_JOYPAD_R2: return SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
    default: return SDL_CONTROLLER_BUTTON_INVALID; //SDL_CONTROLLER_AXIS_INVALID
  }
}
void Controller::poll()
{
  SDL_Event e;
  SDL_PumpEvents();

  while(SDL_PeepEvents(&e, 1, SDL_GETEVENT, SDL_JOYDEVICEADDED, SDL_JOYDEVICEREMOVED) > 0)
  {
    switch(e.type)
    {
      case SDL_JOYDEVICEADDED:
      {
        Controller::controllers.insert(
        std::pair<unsigned, Controller*>(e.jdevice.which, new Controller(e.jdevice.which)
        ));
        break;
      }
      case SDL_JOYDEVICEREMOVED:
      {
        std::cout<<"Controller removed"<<std::endl;
        Controller* controller;
        Controller::controllers.erase(e.jdevice.which);
        //SDL_GameControllerClose(controller->controller);
        delete controller;
        break;
      }
    }
  }
  SDL_GameControllerUpdate();
}
