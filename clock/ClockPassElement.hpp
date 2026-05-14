#pragma once

#include <hyprland/src/render/Texture.hpp>
#include <hyprland/src/render/pass/PassElement.hpp>

class CClockPassElement : public IPassElement {
 public:
  struct SRenderData {
    SP<Render::ITexture> texture;
    CBox box;
  };

  CClockPassElement(const SRenderData &data_);
  virtual ~CClockPassElement() = default;

  virtual std::vector<UP<IPassElement>> draw() override;
  virtual bool needsLiveBlur() override;
  virtual bool needsPrecomputeBlur() override;
  virtual std::optional<CBox> boundingBox() override;

  virtual const char *passName() override { return "CClockPassElement"; }

  virtual ePassElementType type() override { return EK_CUSTOM; }

private:
  SRenderData m_data;
};
