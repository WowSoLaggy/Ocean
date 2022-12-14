#pragma once

#include "ActionsController.h"
#include "GuiController.h"
#include "OceanLodController.h"

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

  const Dx::ICamera& getCamera() const;
  const GuiController& getGuiController() const;

  Dx::IOceanShader& getOceanShader() const;
  Dx::ISimpleShader& getSimpleShader() const;
  Dx::ISkydomeShader& getSkydomeShader() const;

  bool hasInputControllerAttached() const;
  void createInputController();
  void removeInputController();

  Dx::IObject3* getNotebook() const;

private:
  std::unique_ptr<Dx::ICamera> d_camera;

  std::unique_ptr<Dx::IOceanShader> d_oceanShader;
  std::unique_ptr<Dx::ISimpleShader> d_simpleShader;
  std::unique_ptr<Dx::ISkydomeShader> d_skydomeShader;

  std::unique_ptr<Dx::IObject3> d_skydomeObject;
  std::unique_ptr<Dx::IObject3> d_surfaceObject;
  std::unique_ptr<Dx::IObject3> d_oceanObject;
  std::unique_ptr<Dx::IObject3> d_notebook;

  std::vector<std::shared_ptr<Dx::IObject3>> d_objects;

  std::unique_ptr<Dx::IInputController> d_inputController;

  ActionsController d_actionsController;
  GuiController d_guiController;

  void createSurfaceMesh();
  void createOceanMesh();
  void createTestObjects();
  void createSkydomeMesh();
  void createBoat();
  void createNotebook();

  void createOceanShader();
  void createSimpleShader();
  void createSkydomeShader();

  void createCamera();

  void updateSkydomePosition() const;
  void updateNotebookPosition() const;
};
