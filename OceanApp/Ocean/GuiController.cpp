#include "stdafx.h"
#include "GuiController.h"

#include "Game.h"

#include <LaggyDx/Geometry.h>
#include <LaggyDx/Label.h>
#include <LaggyDx/Layout.h>
#include <LaggyDx/Panel.h>
#include <LaggyDx/RadioButton.h>
#include <LaggyDx/RadioGroup.h>
#include <LaggyDx/Slider.h>

#include <LaggySdk/Math.h>


namespace
{
  const std::string FontName = "play.spritefont";

  const std::unordered_map<int, double> WavesDirections{
    { 1, 20 },
    { 2, 0 },
    { 3, 40 },
  };
  const std::unordered_map<int, double> WavesSteepness{
    { 1, 0.25 },
    { 2, 0.25 },
    { 3, 0.25 },
  };
  const std::unordered_map<int, double> WavesLengths{
    { 1, 30 },
    { 2, 15 },
    { 3, 9 },
  };

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

  std::shared_ptr<Dx::Layout> createSettingsLayout(Dx::IControl& i_parent)
  {
    auto layoutPtr = createLayout(i_parent);
    layoutPtr->setSize(i_parent.getSize());
    layoutPtr->setAlign(Dx::LayoutAlign::TopToBottom_LeftSide);
    layoutPtr->setOffsetFromBorder(8);
    layoutPtr->setOffsetBetweenElements(8);
    layoutPtr->setVisible(false);

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

  std::shared_ptr<Dx::RadioButton> createRadioButton(Dx::IControl& i_parent)
  {
    auto rbPtr = std::make_shared<Dx::RadioButton>();
    rbPtr->setFont(FontName);

    i_parent.addChild(rbPtr);
    return rbPtr;
  }

  std::shared_ptr<Dx::RadioButton> createTabRadioButton(Dx::IControl& i_parent)
  {
    auto rbTabPtr = createRadioButton(i_parent);
    rbTabPtr->setTextScale(0.6f);
    rbTabPtr->setTextureName(Dx::RadioButtonState::Checked, "tab_selected.png");
    rbTabPtr->setTextureName(Dx::RadioButtonState::Unchecked, "tab_unselected.png");

    return rbTabPtr;
  }

  std::shared_ptr<Dx::RadioGroup> createRadioGroup(Dx::IControl& i_parent)
  {
    auto radioGroupPtr = std::make_shared<Dx::RadioGroup>();

    i_parent.addChild(radioGroupPtr);
    return radioGroupPtr;
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
  auto sidePanel = createPanel(d_game.getForm());
  sidePanel->setColor({ 0, 0, 0, 0.4f });
  sidePanel->setTexture("white.png");
  sidePanel->setSize({ 300, (float)d_game.getRenderDevice().getResolution().y });
  sidePanel->setPosition({ (float)d_game.getRenderDevice().getResolution().x - sidePanel->getSize().x, 0 });

  auto sidePanelLayout = createLayout(*sidePanel);
  sidePanelLayout->setSize(sidePanel->getSize());
  sidePanelLayout->setOffsetBetweenElements(-4);
  sidePanelLayout->setAlign(Dx::LayoutAlign::TopToBottom_LeftSide);

  createTabs(*sidePanelLayout);

  auto delimeter = createDelimiter(*sidePanelLayout);
  delimeter->setSize({
    sidePanelLayout->getSize().x - sidePanelLayout->getOffsetFromBorder() * 2,
    5 });

  createWavesSettings(*sidePanelLayout);
  createLightSettings(*sidePanelLayout);

  showWavesSettings();
}


void GuiController::createTabs(Dx::IControl& i_parent)
{
  auto tabsRadioGroup = createRadioGroup(i_parent);
  tabsRadioGroup->setOffsetFromBorder(8);
  tabsRadioGroup->setOffsetBetweenElements(8);
  tabsRadioGroup->setDynamicSizeY(true);
  tabsRadioGroup->setAlign(Dx::LayoutAlign::LeftToRight_TopSide);

  auto wavesTab = createTabRadioButton(*tabsRadioGroup);
  wavesTab->setText("Waves");
  wavesTab->setOnCheck([&]() {
    showWavesSettings();
    });

  auto lightTab = createTabRadioButton(*tabsRadioGroup);
  lightTab->setText("Light");
  lightTab->setOnCheck([&]() {
    showLightSettings();
    });
}

void GuiController::createWavesSettings(Dx::IControl& i_parent)
{
  d_wavesSettingsLayout = createSettingsLayout(i_parent);


  constexpr int WavesCount = 3;
  for (int waveIndex = 1; waveIndex <= WavesCount; ++waveIndex)
  {
    auto windDirectionLabel = createSidePanelLabel(*d_wavesSettingsLayout);
    windDirectionLabel->setText("Wave " + std::to_string(waveIndex) + " Direction (deg):");

    auto windDirectionSlider = createSlider(*d_wavesSettingsLayout);
    windDirectionSlider->setLength(
      (int)d_wavesSettingsLayout->getSize().x -
      d_wavesSettingsLayout->getOffsetFromBorder() * 2 -
      windDirectionSlider->getSidesSize().x);
    windDirectionSlider->setOnValueChangedHandler([&, waveIndex](const double i_value) {
      Sdk::Vector2D v{ 1, 0 };
      v.rotate(Sdk::degToRad(i_value));
      d_game.getOceanShader().setWindDirection(waveIndex, std::move(v));
      });
    windDirectionSlider->setMinValue(0);
    windDirectionSlider->setMaxValue(360);
    windDirectionSlider->setCurrentValue(WavesDirections.at(waveIndex));


    auto wavesAmplitudeLabel = createSidePanelLabel(*d_wavesSettingsLayout);
    wavesAmplitudeLabel->setText("Wave " + std::to_string(waveIndex) + " Steepness:");

    auto wavesAmplitudeSlider = createSlider(*d_wavesSettingsLayout);
    wavesAmplitudeSlider->setLength(
      (int)d_wavesSettingsLayout->getSize().x -
      d_wavesSettingsLayout->getOffsetFromBorder() * 2 -
      wavesAmplitudeSlider->getSidesSize().x);
    wavesAmplitudeSlider->setOnValueChangedHandler([&, waveIndex](const double i_value) {
      d_game.getOceanShader().setWavesSteepness(waveIndex, i_value);
      });
    wavesAmplitudeSlider->setMinValue(0);
    wavesAmplitudeSlider->setMaxValue(1);
    wavesAmplitudeSlider->setCurrentValue(WavesSteepness.at(waveIndex));
    wavesAmplitudeSlider->setLabelsPrecision(2);


    auto wavesLengthLabel = createSidePanelLabel(*d_wavesSettingsLayout);
    wavesLengthLabel->setText("Wave " + std::to_string(waveIndex) + " Length (m):");

    auto wavesLengthSlider = createSlider(*d_wavesSettingsLayout);
    wavesLengthSlider->setLength(
      (int)d_wavesSettingsLayout->getSize().x -
      d_wavesSettingsLayout->getOffsetFromBorder() * 2 -
      wavesLengthSlider->getSidesSize().x);
    wavesLengthSlider->setOnValueChangedHandler([&, waveIndex](const double i_value) {
      d_game.getOceanShader().setWavesLength(waveIndex, i_value);
      });
    wavesLengthSlider->setMinValue(0);
    wavesLengthSlider->setMaxValue(50);
    wavesLengthSlider->setCurrentValue(WavesLengths.at(waveIndex));
    wavesLengthSlider->setLabelsPrecision(2);

    if (waveIndex == WavesCount)
      return;

    auto delimeter = createDelimiter(*d_wavesSettingsLayout);
    delimeter->setSize({
      d_wavesSettingsLayout->getSize().x - d_wavesSettingsLayout->getOffsetFromBorder() * 2,
      5 });
  } // for
}

void GuiController::createLightSettings(Dx::IControl& i_parent)
{
  d_lightSettingsLayout = createSettingsLayout(i_parent);


  auto altitudeLabel = createSidePanelLabel(*d_lightSettingsLayout);
  altitudeLabel->setText("Sun Altitude (deg):");

  auto altitudeSlider = createSlider(*d_lightSettingsLayout);
  altitudeSlider->setLength(
    (int)d_lightSettingsLayout->getSize().x -
    d_lightSettingsLayout->getOffsetFromBorder() * 2 -
    altitudeSlider->getSidesSize().x);
  altitudeSlider->setOnValueChangedHandler(
    std::bind(&GuiController::setSunAltitude, this, std::placeholders::_1));
  altitudeSlider->setMinValue(-90);
  altitudeSlider->setMaxValue(90);
  altitudeSlider->setCurrentValue(70);


  auto longitudeLabel = createSidePanelLabel(*d_lightSettingsLayout);
  longitudeLabel->setText("Sun Longitude (deg):");

  auto longitudeSlider = createSlider(*d_lightSettingsLayout);
  longitudeSlider->setLength(
    (int)d_lightSettingsLayout->getSize().x -
    d_lightSettingsLayout->getOffsetFromBorder() * 2 -
    longitudeSlider->getSidesSize().x);
  longitudeSlider->setOnValueChangedHandler(
    std::bind(&GuiController::setSunLongitude, this, std::placeholders::_1));
  longitudeSlider->setMinValue(0);
  longitudeSlider->setMaxValue(360);
  longitudeSlider->setCurrentValue(45);
  longitudeSlider->setLabelsPrecision(0);


  auto radiusLabel = createSidePanelLabel(*d_lightSettingsLayout);
  radiusLabel->setText("Sun Radius:");

  auto radiusSlider = createSlider(*d_lightSettingsLayout);
  radiusSlider->setLength(
    (int)d_lightSettingsLayout->getSize().x -
    d_lightSettingsLayout->getOffsetFromBorder() * 2 -
    radiusSlider->getSidesSize().x);
  radiusSlider->setOnValueChangedHandler([&](const double i_value) {
    d_game.getSkydomeShader().setSunRadius((float)i_value);
    });
  radiusSlider->setMinValue(0.001);
  radiusSlider->setMaxValue(0.5);
  radiusSlider->setCurrentValue(0.05);
  radiusSlider->setLabelsPrecision(3);
}


void GuiController::showWavesSettings()
{
  d_lightSettingsLayout->setVisible(false);
  d_wavesSettingsLayout->setVisible(true);
}

void GuiController::showLightSettings()
{
  d_wavesSettingsLayout->setVisible(false);
  d_lightSettingsLayout->setVisible(true);
}


void GuiController::setSunAltitude(const double i_value)
{
  d_sunAltitude = i_value;
  updateLightDirection();
}

void GuiController::setSunLongitude(const double i_value)
{
  d_sunLongitude = i_value;
  updateLightDirection();
}

void GuiController::updateLightDirection() const
{
  const auto lightDirection = -Dx::getVectorByYawAndPitch(
    Sdk::degToRad(d_sunLongitude), Sdk::degToRad(d_sunAltitude));
  const auto sunDirection = -lightDirection;

  d_game.getOceanShader().setLightDirection(lightDirection);
  d_game.getSimpleShader().setLightDirection(lightDirection);

  d_game.getSkydomeShader().setSunDirection(sunDirection);
}
