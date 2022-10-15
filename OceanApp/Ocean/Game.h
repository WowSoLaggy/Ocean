#pragma once

#include <LaggyDx/Game.h>
#include <LaggyDx/ICamera.h>
#include <LaggyDx/IInputController.h>


class Game : public Dx::Game
{
public:
  Game();

  virtual void update(double i_dt) override;
  virtual void render() override;

private:
  std::unique_ptr<Dx::ICamera> d_camera;

  std::unique_ptr<Dx::IInputController> d_inputController;
};
