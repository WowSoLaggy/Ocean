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
  std::shared_ptr<Dx::Panel> d_sidePanel;
  std::shared_ptr<Dx::Layout> d_sidePanelLayout;
  std::shared_ptr<Dx::Label> d_windDirectionLabel;
  std::shared_ptr<Dx::Slider> d_windDirectionSlider;
  
  void createFpsLabel();
  void createSidePanel();
};
