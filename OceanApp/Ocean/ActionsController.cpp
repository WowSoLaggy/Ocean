#include "stdafx.h"
#include "ActionsController.h"

#include "Game.h"


ActionsController::ActionsController(Game& i_game)
  : d_game(i_game)
{
}


void ActionsController::createActions()
{
  d_game.getActionsMap().setAction(
    Dx::KeyboardKey::Space,
    Dx::Action(std::bind(&ActionsController::changeControlType, this)),
    Dx::ActionType::OnPress);
}


void ActionsController::changeControlType()
{
  if (d_game.hasInputControllerAttached())
    d_game.removeInputController();
  else
    d_game.createInputController();
}
