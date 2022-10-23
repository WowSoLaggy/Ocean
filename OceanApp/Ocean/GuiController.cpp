#include "stdafx.h"
#include "GuiController.h"

#include "Game.h"

#include <LaggyDx/Label.h>
#include <LaggyDx/Layout.h>
#include <LaggyDx/Panel.h>
#include <LaggyDx/Slider.h>

#include <LaggySdk/Math.h>


namespace
{
  const std::string FontName = "play.spritefont";

  std::shared_ptr<Dx::Label> createLabel(Dx::IControl& i_parent)
  {
    auto labelPtr = std::make_shared<Dx::Label>();
    labelPtr->setFont(FontName);
    
    i_parent.addChild(labelPtr);
    return labelPtr;
  }

  std::shared_ptr<Dx::Label> createSidePanelLabel(Dx::IControl& i_parent)
  {
    auto labelPtr = createLabel(i_parent);
    labelPtr->setTextScale(0.7f);

    return labelPtr;
  }

  std::shared_ptr<Dx::Panel> createPanel(Dx::IControl& i_parent)
  {
    auto panelPtr = std::make_shared<Dx::Panel>();

    i_parent.addChild(panelPtr);
    return panelPtr;
  }

  std::shared_ptr<Dx::Panel> createDelimiter(Dx::IControl& i_parent)
  {
    auto deimeterPtr = createPanel(i_parent);
    deimeterPtr->setTexture("white.png");
    deimeterPtr->setColor({ 0, 0, 0, 0.4f });

    return deimeterPtr;
  }

  std::shared_ptr<Dx::Layout> createLayout(Dx::IControl& i_parent)
  {
    auto layoutPtr = std::make_shared<Dx::Layout>();

    i_parent.addChild(layoutPtr);
    return layoutPtr;
  }

  std::shared_ptr<Dx::Slider> createSlider(Dx::IControl& i_parent)
  {
    auto sliderPtr = std::make_shared<Dx::Slider>();
    sliderPtr->setTextureBack("slider_back.png");
    sliderPtr->setTextureSlider("slider_slider.png");
    sliderPtr->setTextureLeftSide("slider_side_l.png");
    sliderPtr->setTextureRightSide("slider_side_r.png");
    sliderPtr->setFont(FontName);
    sliderPtr->setLabelsScale(0.6f);

    i_parent.addChild(sliderPtr);
    return sliderPtr;
  }
}


GuiController::GuiController(Game& i_game)
  : d_game(i_game)
{
}


void GuiController::update(double i_dt)
{
  d_fpsLabel->setText("FPS: " + std::to_string(d_game.getFpsCounter().fps()));
}


void GuiController::createInGameGui()
{
  createFpsLabel();
  createSidePanel();
}


void GuiController::createFpsLabel()
{
  d_fpsLabel = createLabel(d_game.getForm());
}

void GuiController::createSidePanel()
{
  d_sidePanel = createPanel(d_game.getForm());
  d_sidePanel->setColor({ 0, 0, 0, 0.4f });
  d_sidePanel->setTexture("white.png");
  d_sidePanel->setSize({ 300, (float)d_game.getRenderDevice().getResolution().y });
  d_sidePanel->setPosition({ (float)d_game.getRenderDevice().getResolution().x - d_sidePanel->getSize().x, 0 });

  auto sidePanelLayout = createLayout(*d_sidePanel);
  sidePanelLayout->setSize(d_sidePanel->getSize());
  sidePanelLayout->setOffsetFromBorder(8);
  sidePanelLayout->setOffsetBetweenElements(8);


  constexpr int WavesCount = 3;
  for (int waveIndex = 1; waveIndex <= WavesCount; ++waveIndex)
  {
    auto windDirectionLabel = createSidePanelLabel(*sidePanelLayout);
    windDirectionLabel->setText("Wave " + std::to_string(waveIndex) + " Direction (deg):");

    auto windDirectionSlider = createSlider(*sidePanelLayout);
    windDirectionSlider->setLength(
      (int)d_sidePanel->getSize().x -
      sidePanelLayout->getOffsetFromBorder() * 2 -
      windDirectionSlider->getSidesSize().x);
    windDirectionSlider->setOnValueChangedHandler([&, waveIndex](const double i_value) {
      Sdk::Vector2D v{ 1, 0 };
      v.rotate(Sdk::degToRad(i_value));
      d_game.getShader().setWindDirection(waveIndex, std::move(v));
      });
    windDirectionSlider->setMinValue(0);
    windDirectionSlider->setMaxValue(360);
    windDirectionSlider->setCurrentValue(25);


    auto wavesAmplitudeLabel = createSidePanelLabel(*sidePanelLayout);
    wavesAmplitudeLabel->setText("Wave " + std::to_string(waveIndex) + " Steepness:");

    auto wavesAmplitudeSlider = createSlider(*sidePanelLayout);
    wavesAmplitudeSlider->setLength(
      (int)d_sidePanel->getSize().x -
      sidePanelLayout->getOffsetFromBorder() * 2 -
      wavesAmplitudeSlider->getSidesSize().x);
    wavesAmplitudeSlider->setOnValueChangedHandler([&, waveIndex](const double i_value) {
      d_game.getShader().setWavesSteepness(waveIndex, i_value);
      });
    wavesAmplitudeSlider->setMinValue(0);
    wavesAmplitudeSlider->setMaxValue(1);
    wavesAmplitudeSlider->setCurrentValue(0.5);
    wavesAmplitudeSlider->setLabelsPrecision(2);


    auto wavesLengthLabel = createSidePanelLabel(*sidePanelLayout);
    wavesLengthLabel->setText("Wave " + std::to_string(waveIndex) + " Length (m):");

    auto wavesLengthSlider = createSlider(*sidePanelLayout);
    wavesLengthSlider->setLength(
      (int)d_sidePanel->getSize().x -
      sidePanelLayout->getOffsetFromBorder() * 2 -
      wavesLengthSlider->getSidesSize().x);
    wavesLengthSlider->setOnValueChangedHandler([&, waveIndex](const double i_value) {
      d_game.getShader().setWavesLength(waveIndex, i_value);
      });
    wavesLengthSlider->setMinValue(0);
    wavesLengthSlider->setMaxValue(50);
    wavesLengthSlider->setCurrentValue(10);
    wavesLengthSlider->setLabelsPrecision(2);

    if (waveIndex == WavesCount)
      return;

    auto delimeter = createDelimiter(*sidePanelLayout);
    delimeter->setSize({
      sidePanelLayout->getSize().x - sidePanelLayout->getOffsetFromBorder() * 2,
      5 });
  }
}
