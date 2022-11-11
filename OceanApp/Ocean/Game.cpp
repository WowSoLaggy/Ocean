#include "stdafx.h"
#include "Game.h"

#include <LaggyDx/FreeCameraController.h>
#include <LaggyDx/GameSettings.h>
#include <LaggyDx/IShape3d.h>
#include <LaggyDx/MeshUtils.h>
#include <LaggyDx/Model.h>
#include <LaggyDx/Object3.h>


namespace
{
  constexpr float GridWorldSize = 500;
  constexpr float GridResolution = 0.5f;
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
  createOceanMesh();
  createTestMesh();
  createSkydomeMesh();

  createCamera();

  createOceanShader();
  createSimpleShader();
  createSkydomeShader();

  d_actionsController.createActions();

  d_guiController.createInGameGui();
  getInputDevice().showCursor();
}


void Game::createOceanMesh()
{
  const auto planeShape = Dx::IShape3d::plane(
    { GridPointsNumber, GridPointsNumber }, GridResolution, TextureMultiplier);
  auto mesh = Dx::createMeshFromShape(*planeShape, getRenderDevice());
  
  Dx::MaterialSequence matSeq;
  Dx::Material mat;
  mat.diffuseColor = { 0.16f, 0.33f, 0.5f, 1.0f };
  matSeq.add({ std::move(mat), 0, (int)planeShape->getInds().size() });
  mesh.setMaterials(std::make_unique<Dx::MaterialSequence>(std::move(matSeq)));

  Dx::Model model;
  model.addMesh(std::move(mesh));
  d_oceanModel = std::make_unique<Dx::Model>(std::move(model));

  Dx::Object3 obj;
  obj.setModel(*d_oceanModel);
  //obj.setTextureResource(getResourceController().getTexture("ocean.png"));

  d_oceanObject = std::make_unique<Dx::Object3>(std::move(obj));
}

void Game::createTestMesh()
{
  const auto testShape = Dx::IShape3d::sphere(10.0f, 50, 50);
  auto mesh = Dx::createMeshFromShape(*testShape, getRenderDevice());

  Dx::MaterialSequence matSeq;
  Dx::Material mat;
  mat.diffuseColor = { 0.16f, 0.5f, 0.33f, 1.0f };
  matSeq.add({ std::move(mat), 0, (int)testShape->getInds().size() });
  mesh.setMaterials(std::make_unique<Dx::MaterialSequence>(std::move(matSeq)));

  Dx::Model model;
  model.addMesh(std::move(mesh));
  d_testModel = std::make_unique<Dx::Model>(std::move(model));

  Dx::Object3 obj;
  obj.setModel(*d_testModel);
  obj.setPosition({ 30, 10, 30 });

  d_testObject = std::make_unique<Dx::Object3>(std::move(obj));
}

void Game::createSkydomeMesh()
{
  constexpr int SkydomeStacksNSlices = 200;
  const auto domeShape = Dx::IShape3d::sphereInverted(1, SkydomeStacksNSlices, SkydomeStacksNSlices);
  auto mesh = Dx::createMeshFromShape(*domeShape, getRenderDevice());

  Dx::MaterialSequence matSeq;
  Dx::Material mat;
  matSeq.add({ std::move(mat), 0, (int)domeShape->getInds().size() });
  mesh.setMaterials(std::make_unique<Dx::MaterialSequence>(std::move(matSeq)));

  Dx::Model model;
  model.addMesh(std::move(mesh));
  d_skydomeModel = std::make_unique<Dx::Model>(std::move(model));

  Dx::Object3 obj;
  obj.setModel(*d_skydomeModel);

  d_skydomeObject = std::make_unique<Dx::Object3>(std::move(obj));
}


void Game::createCamera()
{
  d_camera = Dx::ICamera::createFirstPersonCamera(
    { getGameSettings().screenWidth, getGameSettings().screenHeight });
  d_camera->setPosition({ 6.0f, 16.0f, -10.0f });
  d_camera->setLookAt({ 25.0f, 0.0f, 25.0f });
}


Dx::IOceanShader& Game::getOceanShader() const
{
  CONTRACT_EXPECT(d_oceanShader);
  return *d_oceanShader;
}

Dx::ISimpleShader& Game::getSimpleShader() const
{
  CONTRACT_EXPECT(d_simpleShader);
  return *d_simpleShader;
}

Dx::ISkydomeShader& Game::getSkydomeShader() const
{
  CONTRACT_EXPECT(d_skydomeShader);
  return *d_skydomeShader;
}


bool Game::hasInputControllerAttached() const
{
  return d_inputController.get();
}

void Game::createInputController()
{
  CONTRACT_ASSERT(d_camera);

  auto controller = std::make_unique<Dx::FreeCameraController>(*this, *d_camera);
  controller->setCameraSpeed(50.0);

  d_inputController = std::move(controller);
}

void Game::removeInputController()
{
  d_inputController.reset();
}


void Game::createOceanShader()
{
  d_oceanShader = Dx::IOceanShader::create(getRenderDevice(), *d_camera, getResourceController());
  d_oceanShader->setTextureCoef(TextureMultiplier);
  d_oceanShader->setLightColor({ 1, 1, 1, 1 });
  d_oceanShader->setAmbientStrength(0.3);
}

void Game::createSimpleShader()
{
  d_simpleShader = Dx::ISimpleShader::create(getRenderDevice(), *d_camera, getResourceController());
  d_simpleShader->setLightColor({ 1, 1, 1, 1 });
  d_simpleShader->setAmbientStrength(0.3);
}

void Game::createSkydomeShader()
{
  d_skydomeShader = Dx::ISkydomeShader::create(getRenderDevice(), *d_camera, getResourceController());
}


void Game::update(double i_dt)
{
  Dx::Game::update(i_dt);
  
  d_guiController.update(i_dt);
  getOceanShader().setGlobalTime(getGlobalTime());
  d_skydomeObject->setPosition(d_camera->getPosition());
}


void Game::render()
{
  getSkydomeShader().draw(*d_skydomeObject);
  getOceanShader().draw(*d_oceanObject);
  getSimpleShader().draw(*d_testObject);

  Dx::Game::render();
}
