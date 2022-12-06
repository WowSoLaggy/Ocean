#include "stdafx.h"
#include "Game.h"

#include <LaggyDx/Colors.h>
#include <LaggyDx/FreeCameraController.h>
#include <LaggyDx/GameSettings.h>
#include <LaggyDx/Geometry.h>
#include <LaggyDx/HeightMap.h>
#include <LaggyDx/IFbxResource.h>
#include <LaggyDx/IShape3d.h>
#include <LaggyDx/ITexture.h>
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

    settings.debugMode = true;

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
  createTestObjects();
  createSkydomeMesh();
  createBoat();
  createNotebook();

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
  heightMap.normalize(-30, 10);

  auto pred = [](const Dx::Tri& i_tri, const double i_heightDiff) {
    const int depth = i_tri.depth();
    constexpr int MinDepth = 5;
    constexpr int MaxDepth = 20;
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

void Game::createTestObjects()
{
  const auto testShape = Dx::IShape3d::sphere(1.0f, 50, 50);

  std::vector<float> Depths{ 0, -2, -5, -10, -20 };
  for (const float depth : Depths)
  {
    auto testObject = Dx::createObjectFromShape(*testShape, getRenderDevice(), true);
    testObject->setPosition({ 102, depth, 96 });

    Dx::traverseMaterials(testObject->getModel(), [](auto& i_mat) {
      i_mat.diffuseColor = { 0.16f, 0.5f, 0.33f, 1.0f };
      i_mat.specularIntensity = 1;
      });

    d_objects.push_back(std::move(testObject));
  }
}

void Game::createSkydomeMesh()
{
  const auto skydomeShape = Dx::IShape3d::skydomePlane(20, 10);
  d_skydomeObject = Dx::createObjectFromShape(*skydomeShape, getRenderDevice(), true);
}

void Game::createBoat()
{
  auto boat = std::make_unique<Dx::Object3>();
  boat->setModel(getResourceController().getFbx("row_boat.fbx").getModel());

  Dx::traverseMaterials(boat->getModel(), [](auto& i_mat) {
    i_mat.diffuseColor = Sdk::Vector4F::identity();
    });

  constexpr float BoatScale = 0.01f;
  boat->setScale({ BoatScale, BoatScale, BoatScale });
  boat->setRotation({ -(float)Sdk::PiHalf, 0, 0 });
  boat->setPosition(WorldCenter);

  d_objects.push_back(std::move(boat));
}

void Game::createNotebook()
{
  constexpr float MapSize = 0.2f;
  constexpr float MapTextureCoeff = 1.0f / 0.2f;
  auto mapShape = Dx::IShape3d::plane({ 2, 2 }, MapSize, MapTextureCoeff);
  d_notebook = Dx::createObjectFromShape(*mapShape, getRenderDevice(), true);

  Dx::traverseMaterials(d_notebook->getModel(), [](Dx::Material& i_mat) {
    i_mat.diffuseColor = Dx::Colors::White;
    i_mat.textureName = "";
    });

  d_notebook->setTexture(getResourceController().getTexture("height_map.png"));
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


Dx::IObject3* Game::getNotebook() const
{
  return d_notebook.get();
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
  d_skydomeShader->setWindDirection1({ 2.3, 1 });
  d_skydomeShader->setWindDirection2({ 2, 1 });
  d_skydomeShader->setWindSpeed1(0.005);
  d_skydomeShader->setWindSpeed2(0.005);
  d_skydomeShader->setOvercast(0.5);
}


void Game::update(double i_dt)
{
  Dx::Game::update(i_dt);
  d_guiController.update(i_dt);

  getOceanShader().setGlobalTime(getGlobalTime());
  getSkydomeShader().setGlobalTime(getGlobalTime());

  updateSkydomePosition();
  updateNotebookPosition();
}


void Game::render()
{
  getSkydomeShader().draw(*d_skydomeObject);
  getSimpleShader().draw(*d_surfaceObject);

  for (const auto& objPtr : d_objects)
    getSimpleShader().draw(*objPtr);
  
  getOceanShader().draw(*d_oceanObject);

  getSimpleShader().draw(*d_notebook);

  Dx::Game::render();
}


void Game::updateSkydomePosition() const
{
  d_skydomeObject->setPosition(d_camera->getPosition());
}

void Game::updateNotebookPosition() const
{
  const auto pos = d_camera->getPosition() +
    d_camera->getForward() * 0.5f +
    d_camera->getDown() * 0.05f +
    d_camera->getLeft() * 0.25f;
  d_notebook->setPosition(pos);

  const auto rot = Dx::getYawAndPitchFromVector(d_camera->getForward());
  d_notebook->setRotation({ Sdk::degToRad(30.0f) + rot.z, -rot.y - Sdk::degToRad(80.0f), 0 });
}
