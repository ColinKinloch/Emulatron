#pragma once

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_gamecontroller.h>
#include "retro/libretro.h"
#include <map>

class Controller
{
public:
  Controller(unsigned id);

  uint8_t getButton(unsigned button);
  uint8_t getHat(unsigned hat);
  int16_t getAxis(unsigned axis, unsigned stick);

  const char* getName();

  static void poll();
  static std::map<unsigned, Controller*> controllers;

  SDL_GameController* getSDLGameController();

private:
  SDL_GameController *controller;

  static SDL_GameControllerButton retroToSDLButton(unsigned id);
  static SDL_GameControllerAxis retroToSDLAxis(unsigned index, unsigned id);
};
