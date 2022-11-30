#include "stdafx.h"
#include "Game.h"

#include <LaggyDx/FreeCameraController.h>
#include <LaggyDx/GameSettings.h>
#include <LaggyDx/HeightMap.h>
#include <LaggyDx/IFbxResource.h>
#include <LaggyDx/IShape3d.h>
#include <LaggyDx/ITextureResource.h>
#include <LaggyDx/ModelUtils.h>
#include <LaggyDx/Model.h>
#include <LaggyDx/Object3.h>
#include <LaggyDx/Roam.h>
#include <LaggyDx/Tri.h>

#include <LaggySdk/Math.h>


namespace
{
  const Sdk::Vector3F WorldCenter = { 100, 0, 100 };

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
  createSurfaceMesh();
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


void Game::createSurfaceMesh()
{
  const auto& heightMapTexture = getResourceController().getTexture("height_map.png");
  auto heightMap = Dx::HeightMap::fromBitmap(*heightMapTexture.getBitmap(getRenderDevice()));
  heightMap.normalize(-10, 10);

  auto pred = [](const Dx::Tri& i_tri, const double i_heightDiff) {
    const int depth = i_tri.depth();
    constexpr int MinDepth = 10;
    constexpr int MaxDepth = 30;
    constexpr double Precision = 0.1;

    if (depth < MinDepth)
      return true;
    if (depth >= MaxDepth)
      return false;
    return i_heightDiff > Precision;
  };

  const Dx::Roam surf(heightMap, pred);
  const auto shape = Dx::IShape3d::fromRoam(surf);
  d_surfaceObject = Dx::createObjectFromShape(*shape, getRenderDevice(), true);

  Dx::traverseMaterials(d_surfaceObject->getModel(), [](auto& i_mat) {
    i_mat.diffuseColor = { 0.2f, 0.5f, 0.2f, 1.0f };
    });
}

void Game::createOceanMesh()
{
  auto pred = [&](const Dx::Tri& i_tri, const std::vector<Dx::VertexPosNormText>& i_points) {
    const auto center = (
      i_points.at(i_tri.ind1).position +
      i_points.at(i_tri.ind2).position +
      i_points.at(i_tri.ind3).position) / 3;
    const auto dist = std::max((center - WorldCenter).length(), 1.0f);

    constexpr int MinLevel = 15;
    constexpr int MaxLevel = 20;
    constexpr float MaxQualityRadius = 10.0f;
    constexpr float MinQualityRadius = 80.0f;

    const float ratio = Sdk::saturate((dist - MaxQualityRadius) / (MinQualityRadius - MaxQualityRadius));
    const auto score = MinLevel + (MaxLevel - MinLevel) * (1 - ratio);

    return i_tri.depth() < score;
  };
  const Dx::Roam surf(200, pred);

  const auto shape = Dx::IShape3d::fromRoam(surf);
  d_oceanObject = Dx::createObjectFromShape(*shape, getRenderDevice(), true);

  Dx::traverseMaterials(d_oceanObject->getModel(), [](auto& i_mat) {
    i_mat.diffuseColor = { 0.16f, 0.33f, 0.5f, 0.9f };
    i_mat.specularIntensity = 1;
    i_mat.specularPower = 16;
    });
}

void Game::createTestMesh()
{
  const auto testShape = Dx::IShape3d::sphere(1.0f, 50, 50);
  auto mesh = Dx::createMeshFromShape(*testShape, getRenderDevice(), true);

  d_testObject = Dx::createObjectFromMesh(std::move(mesh));
  d_testObject->setPosition({ 30, 5, 30 });

  Dx::traverseMaterials(d_testObject->getModel(), [](auto& i_mat) {
    i_mat.diffuseColor = { 0.16f, 0.5f, 0.33f, 1.0f };
    });
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

  Dx::traverseMaterials(d_boat->getModel(), [](auto& i_mat) {
    i_mat.diffuseColor = Sdk::Vector4F::identity();
    });

  constexpr float BoatScale = 0.01f;
  d_boat->setScale({ BoatScale, BoatScale, BoatScale });
  d_boat->setRotation({ -(float)Sdk::PiHalf, 0, 0 });
  d_boat->setPosition(WorldCenter);
}


void Game::createCamera()
{
  d_camera = Dx::ICamera::createFirstPersonCamera(
    { getGameSettings().screenWidth, getGameSettings().screenHeight });
  d_camera->setPosition({ 91.74f, 6.48f, 91.32f });
  d_camera->setLookAt({ 92.32f, 6.06f, 92.01f });
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
  d_oceanShader->setLightColor({ 1, 1, 1, 1 });
  d_oceanShader->setAmbientStrength(0.3);
  d_oceanShader->setTexturesDisplacementSettings(1.0f, 2.0f, { 2, 1 }, { 1, 2 });
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

  getSimpleShader().draw(*d_surfaceObject);
  getSimpleShader().draw(*d_testObject);
  getSimpleShader().draw(*d_boat);

  getOceanShader().draw(*d_oceanObject);

  Dx::Game::render();
}
