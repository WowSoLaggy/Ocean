#pragma once

#include <LaggyDx/IObject3.h>


class OceanLodController
{
public:
  void createObjects(const Dx::IRenderDevice& i_renderDevice);

  const std::vector<std::shared_ptr<Dx::IObject3>>& getObjects() const;

private:
  std::vector<std::shared_ptr<Dx::IObject3>> d_objects;
};
