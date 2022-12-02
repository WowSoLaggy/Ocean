#pragma once

#include "Fwd.h"

#include <LaggyDx/LaggyDxFwd.h>


class GuiController
{
public:
  GuiController(Game& i_game);

  void update(double i_dt);

  void createInGameGui();
  void switchGuiVisibility() const;

private:
  Game& d_game;

  std::shared_ptr<Dx::Label> d_fpsLabel;
  std::shared_ptr<Dx::Panel> d_sidePanel;
  std::shared_ptr<Dx::Layout> d_wavesSettingsLayout;
  std::shared_ptr<Dx::Layout> d_lightSettingsLayout;

  double d_sunAltitude = 0;
  double d_sunLongitude = 0;
  void setSunAltitude(double i_value);
  void setSunLongitude(double i_value);
  void updateLightDirection() const;

  void createFpsLabel();
  void createSidePanel();

  void createTabs(Dx::IControl& i_parent);
  void createWavesSettings(Dx::IControl& i_parent);
  void createLightSettings(Dx::IControl& i_parent);

  void showWavesSettings();
  void showLightSettings();
};
