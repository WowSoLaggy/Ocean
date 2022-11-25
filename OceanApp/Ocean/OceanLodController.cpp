#include "stdafx.h"
#include "OceanLodController.h"

#include <LaggyDx/IShape3d.h>
#include <LaggyDx/ModelUtils.h>

#include <LaggySdk/Size.h>


namespace
{
  constexpr int Size = 5;
  constexpr int CenterCoord = Size / 2;

  std::vector<int> getLods()
  {
    std::vector<int> lods(Size * Size, 2);

    for (int y = 0; y < Size; ++y)
    {
      for (int x = 0; x < Size; ++x)
      {
        auto set = [&](const int i_value) {
          lods.at(x + y * Size) = i_value;
        };

        const int diffX = std::abs(x - CenterCoord);
        const int diffY = std::abs(y - CenterCoord);
        const int maxDist = std::max(diffX, diffY);

        if (maxDist == 0)
          set(0);
        else if (maxDist <= 1)
          set(1);
      }
    }

    return lods;
  }

} // anonym NS


const std::vector<std::shared_ptr<Dx::IObject3>>& OceanLodController::getObjects() const
{
  return d_objects;
}


void OceanLodController::createObjects(const Dx::IRenderDevice& i_renderDevice)
{
  constexpr float GridSize = 40;
  constexpr float GridSizeHalf = GridSize / 2;

  constexpr int GridResolutionMultiplier = 2;
  constexpr float GridResolution1 = 0.1f;
  constexpr float GridResolution2 = GridResolution1 * GridResolutionMultiplier;
  constexpr float GridResolution4 = GridResolution2 * GridResolutionMultiplier;

  constexpr int GridPointsNumber1 = (int)(GridSize / GridResolution1) + 1;
  constexpr int GridPointsNumber2 = (int)(GridSize / GridResolution2) + 1;
  constexpr int GridPointsNumber4 = (int)(GridSize / GridResolution4) + 1;

  const Sdk::Vector3F WorldCenter = { 100, 0, 100 };


  std::vector<std::shared_ptr<Dx::IShape3d>> shapes{
    Dx::IShape3d::plane({ GridPointsNumber1, GridPointsNumber1 }, GridResolution1),
    Dx::IShape3d::planeTesselatedBorder({ GridPointsNumber2, GridPointsNumber2 }, GridResolution2),
    Dx::IShape3d::planeTesselatedBorder({ GridPointsNumber4, GridPointsNumber4 }, GridResolution4) };


  const auto lods = getLods();

  for (int y = 0; y < Size; ++y)
  {
    for (int x = 0; x < Size; ++x)
    {
      const auto& shape = *shapes.at(lods.at(x + y * Size));
      auto obj = Dx::createObjectFromShape(shape, i_renderDevice, true);
      obj->setPosition(WorldCenter - Sdk::Vector3F{ GridSizeHalf, 0, GridSizeHalf } +
        Sdk::Vector3F{ GridSize * (x - CenterCoord), 0, GridSize * (y - CenterCoord) });
      
      d_objects.push_back(std::move(obj));
    }
  }
}
