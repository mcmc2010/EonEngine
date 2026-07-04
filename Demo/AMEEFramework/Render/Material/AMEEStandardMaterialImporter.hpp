#ifndef __AMEE_STANDARDMATERIALIMPORTER_H__
#define __AMEE_STANDARDMATERIALIMPORTER_H__
#pragma once
#include "AMEEStandardMaterial.hpp"
#include <string>
#include <vector>
#include <memory>

namespace AMEE {

class RHI;

class AMEEStandardMaterialImporter {
public:
    struct ImportedMaterial {
        std::string Name;
        std::unique_ptr<StandardMaterial> Material;
        std::string DiffuseTexturePath;
        std::string NormalTexturePath;
    };

    static std::vector<ImportedMaterial> Parse(const std::string& Source);
};

} // namespace AMEE

#endif // __AMEE_STANDARDMATERIALIMPORTER_H__
