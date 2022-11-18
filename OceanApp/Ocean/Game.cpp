#include "stdafx.h"
#include "Game.h"

#include <LaggyDx/FreeCameraController.h>
#include <LaggyDx/GameSettings.h>
#include <LaggyDx/IFbxResource.h>
#include <LaggyDx/IShape3d.h>
#include <LaggyDx/ModelUtils.h>
#include <LaggyDx/Model.h>
#include <LaggyDx/Object3.h>

#include <LaggySdk/Math.h>


namespace
{
  constexpr float GridSize = 100;
  constexpr float GridResolutionHi = 0.5f;
  constexpr int GridResolutionMultiplier = 2;
  constexpr float GridResolutionLow = GridResolutionHi * GridResolutionMultiplier;
  constexpr int GridPointsNumberHi = (int)(GridSize / GridResolutionHi) + 1;
  constexpr int GridPointsNumberLow = (int)(GridSize / GridResolutionLow) + 1;
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
  createBoat();

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
  const auto planeShapeHi = Dx::IShape3d::plane(
    { GridPointsNumberHi, GridPointsNumberHi },
    GridResolutionHi, TextureMultiplier);
  const auto planeShapeLow = Dx::IShape3d::planeTesselatedBorder(
    { GridPointsNumberLow, GridPointsNumberLow },
    GridResolutionLow, TextureMultiplier);

  d_oceanObject1 = Dx::createObjectFromShape(*planeShapeHi, getRenderDevice(), true);
  d_oceanObject1->setPosition({ 0, 0, 0 });

  d_oceanObject2 = Dx::createObjectFromShape(*planeShapeLow, getRenderDevice(), true);
  d_oceanObject2->setPosition({ GridSize, 0, 0 });

  d_oceanObject3 = Dx::createObjectFromShape(*planeShapeLow, getRenderDevice(), true);
  d_oceanObject3->setPosition({ GridSize, 0, GridSize });

  d_oceanObject4 = Dx::createObjectFromShape(*planeShapeLow, getRenderDevice(), true);
  d_oceanObject4->setPosition({ 0, 0, GridSize });
}

void Game::createTestMesh()
{
  const auto testShape = Dx::IShape3d::sphere(1.0f, 50, 50);
  auto mesh = Dx::createMeshFromShape(*testShape, getRenderDevice(), true);

  d_testObject = Dx::createObjectFromMesh(std::move(mesh));
  d_testObject->setPosition({ 30, 5, 30 });

  Dx::setColorOfAllMaterials(d_testObject->getModel(), { 0.16f, 0.5f, 0.33f, 1.0f });
}

void Game::createSkydomeMesh()
{
  constexpr int SkydomeStacksNSlices = 200;
  const auto domeShape = Dx::IShape3d::sphereInverted(1, SkydomeStacksNSlices, SkydomeStacksNSlices);
  d_skydomeObject = Dx::createObjectFromShape(*domeShape, getRenderDevice(), true);
}

void Game::createBoat()
{
  d_boat = std::make_unique<Dx::Object3>();
  d_boat->setModel(getResourceController().getFbx("row_boat.fbx").getModel());

  Dx::setColorOfAllMaterials(d_boat->getModel(), Sdk::Vector4F::identity());

  constexpr float BoatScale = 0.01f;
  d_boat->setScale({ BoatScale, BoatScale, BoatScale });
  d_boat->setRotation({ -(float)Sdk::PiHalf, 0, 0 });
  d_boat->setPosition({ GridSize / 2.0f, 0, GridSize / 2.0f });
}


void Game::createCamera()
{
  d_camera = Dx::ICamera::createFirstPersonCamera(
    { getGameSettings().screenWidth, getGameSettings().screenHeight });
  d_camera->setPosition({ 41.74f, 6.48f, 41.32f });
  d_camera->setLookAt({ 42.32f, 6.06f, 42.01f });
}


const Dx::ICamera& Game::getCamera() const
{
  CONTRACT_EXPECT(d_camera);
  return *d_camera;
}

const GuiController& Game::getGuiController() const
{
  return d_guiController;
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
  d_oceanShader->setWaterColor({ 0.16, 0.33, 0.5, 1.0 });
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
  getOceanShader().draw(*d_oceanObject1);
  getOceanShader().draw(*d_oceanObject2);
  getOceanShader().draw(*d_oceanObject3);
  getOceanShader().draw(*d_oceanObject4);
  getSimpleShader().draw(*d_testObject);
  getSimpleShader().draw(*d_boat);

  Dx::Game::render();
}
