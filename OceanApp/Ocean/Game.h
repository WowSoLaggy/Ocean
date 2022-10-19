#pragma once

#include "ActionsController.h"
#include "GuiController.h"

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

  std::unique_ptr<Dx::IInputController>& getInputController();

  void createInputController();
  void removeInputController();

private:
  std::unique_ptr<Dx::ICamera> d_camera;
  std::unique_ptr<Dx::OceanShader> d_shader;

  std::unique_ptr<Dx::IModel> d_model;
  std::unique_ptr<Dx::IObject3> d_object;

  std::unique_ptr<Dx::IInputController> d_inputController;

  ActionsController d_actionsController;
  GuiController d_guiController;

  void createMesh();
  void createCamera();
  void createOceanShader();
};
