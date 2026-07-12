#include "AMEEBuiltinMaterials.hpp"
#include "AMEEStandardMaterial.hpp"
#include "../Texture/AMEETexture2D.hpp"
#include "../Texture/AMEEImage.hpp"
#include "../../Core/Asset/AMEEAssetManager.hpp"
#include "../../Core/Log/AMEELog.hpp"

namespace AMEE {

MaterialHandle BuiltinMaterials::gs_DefaultHandle;
MaterialHandle BuiltinMaterials::gs_ErrorHandle;

void BuiltinMaterials::Initialize(RHI* rhi)
{
    auto& Assets = AssetManager::GetSingleton();

    // Default Gray — solid color, no texture needed
    {
        auto Mat = std::make_unique<StandardMaterial>();
        Mat->SetName("Default-Gray");
        Mat->SetAlbedoColor({0.5f, 0.5f, 0.5f, 1.0f});
        Mat->SetMetallic(0.0f);
        Mat->SetSmoothness(0.5f);
        gs_DefaultHandle = Assets.RegisterMaterial(std::move(Mat));
        AMEE_LOG_INFO("BuiltinMaterials", "Default-Gray registered [%u]", gs_DefaultHandle.Index);
    }

    // Error Pink — magenta with 1x1 fallback texture
    {
        unsigned char Magenta[4] = { 255, 0, 255, 255 };
        ImageData Img;
        Img.Width = Img.Height = 1;
        Img.Channels = 4;
        Img.Pixels.assign(Magenta, Magenta + 4);

        auto Tex = std::make_unique<Texture2D>();
        Tex->Create(rhi, Img);
        TextureHandle PinkTex = Assets.RegisterTexture(std::move(Tex), "_BuiltinPink");

        auto Mat = std::make_unique<StandardMaterial>();
        Mat->SetName("Error-Pink");
        Mat->SetAlbedoMap(PinkTex);
        Mat->SetAlbedoColor({1.0f, 0.0f, 1.0f, 1.0f});
        Mat->SetMetallic(0.0f);
        Mat->SetSmoothness(0.5f);
        gs_ErrorHandle = Assets.RegisterMaterial(std::move(Mat));
        AMEE_LOG_INFO("BuiltinMaterials", "Error-Pink registered [%u]", gs_ErrorHandle.Index);
    }
}

} // namespace AMEE
