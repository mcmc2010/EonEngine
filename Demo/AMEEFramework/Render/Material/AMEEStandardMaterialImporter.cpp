#include "AMEEStandardMaterialImporter.hpp"
#include "../../Core/Log/AMEELog.hpp"
#include <sstream>
#include <cstdlib>

namespace AMEE {

std::vector<AMEEStandardMaterialImporter::ImportedMaterial>
AMEEStandardMaterialImporter::Parse(const std::string& Source)
{
    std::vector<ImportedMaterial> Results;
    ImportedMaterial* Current = nullptr;

    std::istringstream Stream(Source);
    std::string Line;

    auto Finish = [&]() {
        if (Current && !Current->Name.empty()) {
            Results.push_back(std::move(*Current));
        }
        delete Current;
        Current = nullptr;
    };

    while (std::getline(Stream, Line)) {
        // Trim
        size_t Start = Line.find_first_not_of(" \t\r");
        if (Start == std::string::npos) continue;
        size_t End = Line.find_last_not_of(" \t\r");
        Line = Line.substr(Start, End - Start + 1);
        if (Line.empty() || Line[0] == '#') continue;

        std::istringstream LS(Line);
        std::string Token;
        LS >> Token;

        if (Token == "newmtl") {
            Finish();
            Current = new ImportedMaterial();
            LS >> Current->Name;
            Current->Material = std::make_unique<StandardMaterial>();
            Current->Material->SetName(Current->Name);
        } else if (Token == "Ka" && Current) {
            float R, G, B;
            LS >> R >> G >> B;
            // Ambient — ignore for now, use diffuse color instead
            (void)R; (void)G; (void)B;
        } else if (Token == "Kd" && Current) {
            float R, G, B;
            LS >> R >> G >> B;
            Current->Material->SetAlbedoColor({R, G, B, 1.0f});
        } else if (Token == "Ks" && Current) {
            float R, G, B;
            LS >> R >> G >> B;
            Current->Material->SetSpecularColor({R, G, B, 1.0f});
        } else if (Token == "Ns" && Current) {
            float Exp;
            LS >> Exp;
            Current->Material->SetSpecularExponent(Exp);
        } else if ((Token == "d" || Token == "Tr") && Current) {
            float Alpha;
            LS >> Alpha;
            Vec4 C = Current->Material->GetVec4("u_Color");
            C.w = (Token == "Tr") ? (1.0f - Alpha) : Alpha;
            Current->Material->SetAlbedoColor(C);
        } else if (Token == "map_Kd" && Current) {
            LS >> Current->DiffuseTexturePath;
        } else if ((Token == "map_Bump" || Token == "bump") && Current) {
            LS >> Current->NormalTexturePath;
        } else if (Token == "Ni" || Token == "illum") {
            // Ignore
        }
    }

    Finish();

    AMEE_LOG_INFO("StandardMaterialImporter", "Imported %zu materials", Results.size());
    return Results;
}

} // namespace AMEE
