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
  std::shared_ptr<Dx::Label> d_windForceLabel;
  std::shared_ptr<Dx::Slider> d_windForceSlider;

  void createFpsLabel();
  void createSidePanel();

  void onWindDirectionChanged(double i_value) const;
  void onWindForceChanged(double i_value) const;
};
