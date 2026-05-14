#include "ClockPassElement.hpp"

#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <utility>

using namespace Render::GL;

CClockPassElement::CClockPassElement(const SRenderData &data_)
    : m_data(std::move(data_)) {}

std::vector<UP<IPassElement>> CClockPassElement::draw() {
  g_pHyprOpenGL->renderTexture(m_data.texture, m_data.box, {.round = 10});
  return {};
}

bool CClockPassElement::needsLiveBlur() { return false; }

std::optional<CBox> CClockPassElement::boundingBox() {
  return m_data.box.copy()
      .scale(1.F / g_pHyprRenderer->m_renderData.pMonitor->m_scale)
      .round();
}

bool CClockPassElement::needsPrecomputeBlur() { return false; }
