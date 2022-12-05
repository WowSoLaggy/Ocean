#include "stdafx.h"
#include "ActionsController.h"

#include "Game.h"


ActionsController::ActionsController(Game& i_game)
  : d_game(i_game)
{
}


void ActionsController::createActions()
{
  auto set = [&](auto&&... args) {
    d_game.getActionsMap().setAction(std::forward<decltype(args)>(args)...);
  };

  set(
    Dx::KeyboardKey::Space,
    Dx::Action(std::bind(&ActionsController::changeControlType, this)),
    Dx::ActionType::OnPress);

  set(
    Dx::KeyboardKey::G,
    Dx::Action([&]() {
      d_game.getGuiController().switchGuiVisibility();
      }),
    Dx::ActionType::OnPress);

  set(
    Dx::KeyboardKey::F,
    Dx::Action([&]() {
      static bool fillSolid = true;
      fillSolid = !fillSolid;
      d_game.getOceanShader().setFillMode(fillSolid);
      d_game.getSimpleShader().setFillMode(fillSolid);
      }),
    Dx::ActionType::OnPress);

  set(
    Dx::KeyboardKey::Tab,
    Dx::Action([&]() {
      if (auto* notebook = d_game.getNotebook())
        notebook->setVisible(!notebook->getVisible());
      }),
    Dx::ActionType::OnPress);

  set(
    Dx::KeyboardKey::Escape,
    Dx::Action([&]() {
      d_game.stop();
      }),
    Dx::ActionType::OnPress);
}


void ActionsController::changeControlType()
{
  if (d_game.hasInputControllerAttached())
    d_game.removeInputController();
  else
    d_game.createInputController();
}
