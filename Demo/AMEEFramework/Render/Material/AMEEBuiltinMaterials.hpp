#ifndef __AMEE_BUILTINMATERIALS_H__
#define __AMEE_BUILTINMATERIALS_H__
#pragma once
#include "../../Core/Asset/AMEEAssetHandle.hpp"

namespace AMEE {

class RHI;

class BuiltinMaterials {
public:
    static void Initialize(RHI* rhi);

    static MaterialHandle GetDefault() { return gs_DefaultHandle; }
    static MaterialHandle GetError()   { return gs_ErrorHandle; }

private:
    static MaterialHandle gs_DefaultHandle;
    static MaterialHandle gs_ErrorHandle;
};

} // namespace AMEE

#endif // __AMEE_BUILTINMATERIALS_H__
