#pragma once

#include <LaggyDx/Game.h>
#include <LaggyDx/ICamera.h>
#include <LaggyDx/IInputController.h>
#include <LaggyDx/IModel.h>
#include <LaggyDx/IObject3.h>
#include <LaggyDx/OceanShader.h>


class Game : public Dx::Game
{
public:
  Game();

  virtual void update(double i_dt) override;
  virtual void render() override;

private:
  std::unique_ptr<Dx::ICamera> d_camera;
  std::unique_ptr<Dx::OceanShader> d_shader;

  std::unique_ptr<Dx::IModel> d_model;
  std::unique_ptr<Dx::IObject3> d_object;

  std::shared_ptr<Dx::Label> d_label;

  std::unique_ptr<Dx::IInputController> d_inputController;

  void createMesh();
  void createGui();
};
