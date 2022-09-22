#include "stdafx.h"
#include "Game.h"

#include <LaggyDx/GameSettings.h>


namespace
{
  const Dx::GameSettings& getGameSettings()
  {
    static Dx::GameSettings settings;
    settings.applicationName = "Ocean Sim";
    return settings;
  }

} // anonym NS


Game::Game()
  : Dx::Game(getGameSettings())
{
  getInputDevice().showCursor();
}
