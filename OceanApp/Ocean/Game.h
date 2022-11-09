#pragma once

#include "ActionsController.h"
#include "GuiController.h"

#include <LaggyDx/Game.h>
#include <LaggyDx/ICamera.h>
#include <LaggyDx/IInputController.h>
#include <LaggyDx/IModel.h>
#include <LaggyDx/IObject3.h>
#include <LaggyDx/IOceanShader.h>
#include <LaggyDx/ISimpleShader.h>
#include <LaggyDx/ISkydomeShader.h>


class Game : public Dx::Game
{
public:
  Game();

  virtual void update(double i_dt) override;
  virtual void render() override;

  Dx::IOceanShader& getOceanShader() const;
  Dx::ISimpleShader& getSimpleShader() const;
  Dx::ISkydomeShader& getSkydomeShader() const;

  bool hasInputControllerAttached() const;
  void createInputController();
  void removeInputController();

private:
  std::unique_ptr<Dx::ICamera> d_camera;

  std::unique_ptr<Dx::IOceanShader> d_oceanShader;
  std::unique_ptr<Dx::ISimpleShader> d_simpleShader;
  std::unique_ptr<Dx::ISkydomeShader> d_skydomeShader;

  std::unique_ptr<Dx::IModel> d_oceanModel;
  std::unique_ptr<Dx::IObject3> d_oceanObject;

  std::unique_ptr<Dx::IModel> d_testModel;
  std::unique_ptr<Dx::IObject3> d_testObject;

  std::unique_ptr<Dx::IModel> d_skydomeModel;
  std::unique_ptr<Dx::IObject3> d_skydomeObject;

  std::unique_ptr<Dx::IInputController> d_inputController;

  ActionsController d_actionsController;
  GuiController d_guiController;

  void createOceanMesh();
  void createTestMesh();
  void createSkydomeMesh();

  void createOceanShader();
  void createSimpleShader();
  void createSkydomeShader();

  void createCamera();
};
