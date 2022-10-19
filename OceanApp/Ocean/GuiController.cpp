#include "stdafx.h"
#include "GuiController.h"

#include "Game.h"

#include <LaggyDx/Label.h>
#include <LaggyDx/Layout.h>
#include <LaggyDx/Panel.h>


GuiController::GuiController(Game& i_game)
  : d_game(i_game)
{
}


void GuiController::update(double i_dt)
{
  d_fpsLabel->setText("FPS: " + std::to_string(d_game.getFpsCounter().fps()));
}


void GuiController::createInGameGui()
{
  createFpsLabel();
}


void GuiController::createFpsLabel()
{
  d_fpsLabel = std::make_shared<Dx::Label>();
  d_fpsLabel->setFont("play.spritefont");
  d_game.getForm().addChild(d_fpsLabel);
}
