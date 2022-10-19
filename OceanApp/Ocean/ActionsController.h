#pragma once

#include "Fwd.h"


class ActionsController
{
public:
  ActionsController(Game& i_game);

  void createActions();

private:
  Game& d_game;

  void changeControlType();
};
