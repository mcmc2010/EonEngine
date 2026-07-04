#ifndef __AMEE_OBJLOADER_H__
#define __AMEE_OBJLOADER_H__
#pragma once
#include "AMEEModelData.hpp"
#include <string>

namespace AMEE {

class ObjLoader {
public:
    static ModelData Load(const std::string& Source);
    static std::string ExtractMtlLib(const std::string& Source);
};

} // namespace AMEE

#endif // __AMEE_OBJLOADER_H__
