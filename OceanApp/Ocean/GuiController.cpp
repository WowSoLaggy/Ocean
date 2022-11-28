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
#include <LaggySdk/StringUtils.h>


namespace
{
  const std::string FontName = "play.spritefont";

  struct Wave
  {
    const double direction = 0;
    const double steepness = 0;
    const double length = 0;
  };
  const std::unordered_map<int, Wave> Waves{
    { 0, { 20, 0.15, 15 } },
    { 1, { 0, 0.15, 7 } },
    { 2, { 40, 0.15, 3 } },
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

  std::string toStr(const Sdk::Vector3F& i_pos)
  {
    constexpr int VectorPrecision = 2;
    return
      Sdk::toString(i_pos.x, VectorPrecision) + ", " +
      Sdk::toString(i_pos.y, VectorPrecision) + ", " +
      Sdk::toString(i_pos.z, VectorPrecision);
  }
}


GuiController::GuiController(Game& i_game)
  : d_game(i_game)
{
}


void GuiController::update(double i_dt)
{
  const std::string text = "FPS: " + std::to_string(d_game.getFpsCounter().fps()) + "\n" +
    "Pos: " + toStr(d_game.getCamera().getPosition()) + "\n" +
    "Look: " + toStr(d_game.getCamera().getLookAt());
  d_fpsLabel->setText(text);
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
  for (int waveIndex = 0; waveIndex < WavesCount; ++waveIndex)
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
    windDirectionSlider->setCurrentValue(Waves.at(waveIndex).direction);


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
    wavesAmplitudeSlider->setCurrentValue(Waves.at(waveIndex).steepness);
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
    wavesLengthSlider->setCurrentValue(Waves.at(waveIndex).length);
    wavesLengthSlider->setLabelsPrecision(2);

    if (waveIndex == WavesCount - 1)
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


  auto radiusInternalLabel = createSidePanelLabel(*d_lightSettingsLayout);
  radiusInternalLabel->setText("Sun Radius Internal:");

  auto radiusInternalSlider = createSlider(*d_lightSettingsLayout);
  radiusInternalSlider->setLength(
    (int)d_lightSettingsLayout->getSize().x -
    d_lightSettingsLayout->getOffsetFromBorder() * 2 -
    radiusInternalSlider->getSidesSize().x);
  radiusInternalSlider->setOnValueChangedHandler([&](const double i_value) {
    d_game.getSkydomeShader().setSunRadiusInternal((float)i_value);
    });
  radiusInternalSlider->setMinValue(0.005);
  radiusInternalSlider->setMaxValue(0.2);
  radiusInternalSlider->setCurrentValue(0.01);
  radiusInternalSlider->setLabelsPrecision(3);


  auto radiusExternalLabel = createSidePanelLabel(*d_lightSettingsLayout);
  radiusExternalLabel->setText("Sun Radius External:");

  auto radiusExternalSlider = createSlider(*d_lightSettingsLayout);
  radiusExternalSlider->setLength(
    (int)d_lightSettingsLayout->getSize().x -
    d_lightSettingsLayout->getOffsetFromBorder() * 2 -
    radiusExternalSlider->getSidesSize().x);
  radiusExternalSlider->setOnValueChangedHandler([&](const double i_value) {
    d_game.getSkydomeShader().setSunRadiusExternal((float)i_value);
    });
  radiusExternalSlider->setMinValue(0.005);
  radiusExternalSlider->setMaxValue(0.2);
  radiusExternalSlider->setCurrentValue(0.05);
  radiusExternalSlider->setLabelsPrecision(3);
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


void GuiController::switchGuiVisibility() const
{
  std::vector<Dx::IControl*> ctrls{ d_fpsLabel.get(), d_sidePanel.get() };
  for (auto ctrlPtr : ctrls)
  {
    CONTRACT_EXPECT(ctrlPtr);
    ctrlPtr->setVisible(!ctrlPtr->getVisible());
  }
}
