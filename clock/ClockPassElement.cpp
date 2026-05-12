#include "ClockPassElement.hpp"

#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <utility>

CClockPassElement::CClockPassElement(SRenderData data_)
    : m_data(std::move(data_)) {}

void CClockPassElement::draw(const CRegion& damage) {
  g_pHyprOpenGL->renderTexture(m_data.texture, m_data.box,
                               {.damage = &damage,
                                .a = 1.0,
                                .blur = true,
                                .blurA = 0.5,
                                .round = true,
                                .roundingPower = 20});
}

bool CClockPassElement::needsLiveBlur() { return false; }

std::optional<CBox> CClockPassElement::boundingBox() {
  return m_data.box.copy()
      .scale(1.F / g_pHyprOpenGL->m_renderData.pMonitor->m_scale)
      .round();
}

bool CClockPassElement::needsPrecomputeBlur() { return false; }
