#pragma once
#ifndef N3_FILE_IMPORTER_H_INC
#define N3_FILE_IMPORTER_H_INC

#include <assimp/BaseImporter.h>

namespace Assimp {

class N3Importer : public BaseImporter {
public:
    N3Importer();
    ~N3Importer() override;
    bool CanRead(const std::string &filename, IOSystem *pIOHandler, bool checkSig) const override;

protected:
    const aiImporterDesc *GetInfo() const override;
    void InternReadFile(const std::string &filename, aiScene *pScene, IOSystem *pIOHandler) override;
};

} // namespace Assimp

#endif
