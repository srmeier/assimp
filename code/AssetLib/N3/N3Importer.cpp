#ifndef AI_BUILD_NO_N3_IMPORTER

#include <assimp/scene.h>
#include <assimp/IOSystem.hpp>

#include "N3Importer.h"

using namespace Assimp;

static constexpr aiImporterDesc desc = {
    "N3 Importer",
    "BitShift",
    "BitShift",
    "",
    aiImporterFlags_SupportBinaryFlavour,
    0,
    0,
    0,
    0,
    "n3chr"
};

N3Importer::N3Importer() {

}

N3Importer::~N3Importer() {

}

bool N3Importer::CanRead(const std::string &filename, IOSystem *pIOHandler, bool checkSig) const {

    pIOHandler->Exists(filename);

    if (checkSig) {
        // Check the signature and return the result
    } else {
        const std::string extension = GetExtension(filename);
        if (extension == "n3chr") {
            return true;
        }
    }
    return false;
}

const aiImporterDesc *N3Importer::GetInfo() const {
    return &desc;
}

void N3Importer::InternReadFile(const std::string &filename, aiScene *pScene, IOSystem *pIOHandler) {
    pIOHandler->Exists(filename);
    pScene->HasAnimations();
}

#endif
