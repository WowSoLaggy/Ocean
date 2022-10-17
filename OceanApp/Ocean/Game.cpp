#include "stdafx.h"
#include "Game.h"

#include <LaggyDx/FirstPersonController.h>
#include <LaggyDx/GameSettings.h>
#include <LaggyDx/Label.h>
#include <LaggyDx/MeshUtils.h>
#include <LaggyDx/Model.h>
#include <LaggyDx/Object3.h>
#include <LaggyDx/Shape3d.h>


namespace
{
  const Dx::GameSettings& getGameSettings()
  {
    static Dx::GameSettings settings;
    settings.applicationName = "Ocean Sim";
    settings.screenWidth = 1024;
    settings.screenHeight = 768;
    return settings;
  }

} // anonym NS


Game::Game()
  : Dx::Game(getGameSettings())
{
  getInputDevice().showCursor();

  d_camera = Dx::ICamera::createFirstPersonCamera(
    { getGameSettings().screenWidth, getGameSettings().screenHeight });
  d_camera->setPosition({ -10.0f, 8.0f, -10.0f });
  d_camera->setLookAt({ 0.0f, 6.0f, 0.0f });

  d_shader = std::make_unique<Dx::OceanShader>(getRenderDevice(), *d_camera, getResourceController());

  d_inputController = std::make_unique<Dx::FirstPersonController>(*this, *d_camera);

  createMesh();
  createGui();
}


void Game::createMesh()
{
  constexpr float GridWorldSize = 50;
  constexpr float GridResolution = 0.1f;
  constexpr int GridPointsNumber = (int)(GridWorldSize / GridResolution);

  const auto planeShape = Dx::Shape3d::plane({ GridPointsNumber, GridPointsNumber }, GridResolution);
  auto mesh = Dx::createMeshFromShape(planeShape, getRenderDevice());
  
  Dx::MaterialSequence matSeq;
  matSeq.add({ {}, 0, (int)planeShape.getInds().size() });
  mesh.setMaterials(std::make_unique<Dx::MaterialSequence>(std::move(matSeq)));

  Dx::Model model;
  model.addMesh(std::move(mesh));
  d_model = std::make_unique<Dx::Model>(std::move(model));

  Dx::Object3 obj;
  obj.setModel(*d_model);

  d_object = std::make_unique<Dx::Object3>(std::move(obj));
}

void Game::createGui()
{
  d_label = std::make_shared<Dx::Label>();
  d_label->setFont("play.spritefont");
  getForm().addChild(d_label);
}


void Game::update(double i_dt)
{
  d_label->setText("FPS: " + std::to_string(getFpsCounter().fps()));

  Dx::Game::update(i_dt);
}


void Game::render()
{
  d_shader->draw(*d_object);

  Dx::Game::render();
}
