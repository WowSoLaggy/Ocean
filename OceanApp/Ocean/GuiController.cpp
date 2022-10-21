#include "stdafx.h"
#include "GuiController.h"

#include "Game.h"

#include <LaggyDx/Label.h>
#include <LaggyDx/Layout.h>
#include <LaggyDx/Panel.h>
#include <LaggyDx/Slider.h>


namespace
{
  std::shared_ptr<Dx::Label> createLabel(Dx::IControl& i_parent)
  {
    auto labelPtr = std::make_shared<Dx::Label>();
    labelPtr->setFont("play.spritefont");
    
    i_parent.addChild(labelPtr);
    return labelPtr;
  }

  std::shared_ptr<Dx::Panel> createPanel(Dx::IControl& i_parent)
  {
    auto panelPtr = std::make_shared<Dx::Panel>();

    i_parent.addChild(panelPtr);
    return panelPtr;
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

  d_sidePanelLayout = createLayout(*d_sidePanel);
  d_sidePanelLayout->setSize(d_sidePanel->getSize());
  d_sidePanelLayout->setOffsetFromBorder(8);
  d_sidePanelLayout->setOffsetBetweenElements(8);

  d_windDirectionLabel = createLabel(*d_sidePanelLayout);
  d_windDirectionLabel->setText("Wind direction:");
  d_windDirectionLabel->setTextScale(0.7f);

  d_windDirectionSlider = createSlider(*d_sidePanelLayout);
  d_windDirectionSlider->setLength(
    (int)d_sidePanel->getSize().x -
    d_sidePanelLayout->getOffsetFromBorder() * 2 -
    d_windDirectionSlider->getSidesSize().x);
  d_windDirectionSlider->setMinValue(0);
  d_windDirectionSlider->setMaxValue(100);
  d_windDirectionSlider->setCurrentValue(50);
}