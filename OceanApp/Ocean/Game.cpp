#include "stdafx.h"
#include "Game.h"

#include <LaggyDx/FreeCameraController.h>
#include <LaggyDx/GameSettings.h>
#include <LaggyDx/MeshUtils.h>
#include <LaggyDx/Model.h>
#include <LaggyDx/Object3.h>
#include <LaggyDx/Shape3d.h>


namespace
{
  constexpr float GridWorldSize = 100;
  constexpr float GridResolution = 0.1f;
  constexpr int GridPointsNumber = (int)(GridWorldSize / GridResolution);
  constexpr float TextureMultiplier = 0.1f;

  const Dx::GameSettings& getGameSettings()
  {
    static Dx::GameSettings settings;
    settings.applicationName = "Ocean Sim";
    settings.screenWidth = 1280;
    settings.screenHeight = 768;
    return settings;
  }

} // anonym NS


Game::Game()
  : Dx::Game(getGameSettings())
  , d_actionsController(*this)
  , d_guiController(*this)
{
  createMesh();
  createCamera();
  createOceanShader();

  d_actionsController.createActions();

  d_guiController.createInGameGui();
  getInputDevice().showCursor();
}


void Game::createMesh()
{
  const auto planeShape = Dx::Shape3d::plane(
    { GridPointsNumber, GridPointsNumber }, GridResolution, TextureMultiplier);
  auto mesh = Dx::createMeshFromShape(planeShape, getRenderDevice());
  
  Dx::MaterialSequence matSeq;
  Dx::Material mat;
  mat.diffuseColor = { 0.16f, 0.33f, 0.5f, 1.0f };
  matSeq.add({ std::move(mat), 0, (int)planeShape.getInds().size() });
  mesh.setMaterials(std::make_unique<Dx::MaterialSequence>(std::move(matSeq)));

  Dx::Model model;
  model.addMesh(std::move(mesh));
  d_model = std::make_unique<Dx::Model>(std::move(model));

  Dx::Object3 obj;
  obj.setModel(*d_model);
  //obj.setTextureResource(getResourceController().getTexture("ocean.png"));

  d_object = std::make_unique<Dx::Object3>(std::move(obj));
}

void Game::createCamera()
{
  d_camera = Dx::ICamera::createFirstPersonCamera(
    { getGameSettings().screenWidth, getGameSettings().screenHeight });
  d_camera->setPosition({ 6.0f, 16.0f, -10.0f });
  d_camera->setLookAt({ 25.0f, 0.0f, 25.0f });
}


Dx::IOceanShader& Game::getShader() const
{
  CONTRACT_EXPECT(d_shader);
  return *d_shader;
}


bool Game::hasInputControllerAttached() const
{
  return d_inputController.get();
}

void Game::createInputController()
{
  CONTRACT_ASSERT(d_camera);

  auto controller = std::make_unique<Dx::FreeCameraController>(*this, *d_camera);
  controller->setCameraSpeed(10.0);

  d_inputController = std::move(controller);
}

void Game::removeInputController()
{
  d_inputController.reset();
}

void Game::createOceanShader()
{
  d_shader = Dx::IOceanShader::create(getRenderDevice(), *d_camera, getResourceController());
  d_shader->setTextureCoef(TextureMultiplier);
}


void Game::update(double i_dt)
{
  d_guiController.update(i_dt);

  d_shader->setGlobalTime(getGlobalTime());

  Dx::Game::update(i_dt);
}


void Game::render()
{
  d_shader->draw(*d_object);

  Dx::Game::render();
}
