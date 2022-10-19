#pragma once

#include "Fwd.h"

#include <LaggyDx/LaggyDxFwd.h>


class GuiController
{
public:
  GuiController(Game& i_game);

  void update(double i_dt);

  void createInGameGui();

private:
  Game& d_game;
  
  std::shared_ptr<Dx::Label> d_fpsLabel;
  
  void createFpsLabel();
};
