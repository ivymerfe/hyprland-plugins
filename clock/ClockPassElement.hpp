#pragma once

#include <hyprland/src/render/Texture.hpp>
#include <hyprland/src/render/pass/PassElement.hpp>

class CClockPassElement : public IPassElement {
public:
    struct SRenderData {
        SP<CTexture> texture;
        CBox box;
    };

    explicit CClockPassElement(SRenderData data_);

    ~CClockPassElement() override = default;

    void draw(const CRegion &damage) override;

    bool needsLiveBlur() override;

    bool needsPrecomputeBlur() override;

    std::optional<CBox> boundingBox() override;

    const char *passName() override { return "CClockPassElement"; }

private:
    SRenderData m_data;
};
