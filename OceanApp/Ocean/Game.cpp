#include "stdafx.h"
#include "Game.h"

#include <LaggyDx/FirstPersonController.h>
#include <LaggyDx/GameSettings.h>


namespace
{
  const Dx::GameSettings& getGameSettings()
  {
    static Dx::GameSettings settings;
    settings.applicationName = "Ocean Sim";
    settings.screenWidth = 1024;
    settings.screenHeight = 768;
    return settings;
  }

} // anonym NS


Game::Game()
  : Dx::Game(getGameSettings())
{
  getInputDevice().showCursor();

  d_camera = Dx::ICamera::createFirstPersonCamera(
    { getGameSettings().screenWidth, getGameSettings().screenHeight });
  d_camera->setPosition({ -10.0f, 10.0f, -10.0f });
  d_camera->setLookAt({ 0.0f, 0.0f, 0.0f });

  d_inputController = std::make_unique<Dx::FirstPersonController>(*this, *d_camera);
void Game::update(double i_dt)
{
  auto toStr = [](const auto& i_vec) -> std::string
  {
    return std::to_string(i_vec.x) + ", " + std::to_string(i_vec.y) + ", " + std::to_string(i_vec.z);
  };

  Dx::Game::update(i_dt);
}


void Game::render()
{
  Dx::Game::render();
}
