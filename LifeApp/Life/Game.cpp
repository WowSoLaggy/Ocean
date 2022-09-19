#include "stdafx.h"
#include "Game.h"

#include <LaggyDx/GameSettings.h>


namespace
{
  const Dx::GameSettings& getGameSettings()
  {
    static Dx::GameSettings settings;
    return settings;
  }

} // anonym NS


Game::Game()
  : Dx::Game(getGameSettings())
{
}
