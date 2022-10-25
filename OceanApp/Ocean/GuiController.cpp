#include "stdafx.h"
#include "GuiController.h"

#include "Game.h"

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
  d_wavesSettingsLayout = createLayout(i_parent);
  d_wavesSettingsLayout->setSize(i_parent.getSize());
  d_wavesSettingsLayout->setAlign(Dx::LayoutAlign::TopToBottom_LeftSide);
  d_wavesSettingsLayout->setOffsetFromBorder(8);
  d_wavesSettingsLayout->setOffsetBetweenElements(6);
  d_wavesSettingsLayout->setVisible(false);


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
      d_game.getShader().setWindDirection(waveIndex, std::move(v));
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
      d_game.getShader().setWavesSteepness(waveIndex, i_value);
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
      d_game.getShader().setWavesLength(waveIndex, i_value);
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
  d_lightSettingsLayout = createLayout(i_parent);
  d_lightSettingsLayout->setSize(i_parent.getSize());
  d_lightSettingsLayout->setAlign(Dx::LayoutAlign::TopToBottom_LeftSide);
  d_lightSettingsLayout->setOffsetFromBorder(8);
  d_lightSettingsLayout->setOffsetBetweenElements(8);
  d_lightSettingsLayout->setVisible(false);
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

