#include "AbstractImportExportBase.h"
#include "UnitTestPCH.h"

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

using namespace Assimp;

class utN3ImporterExporter : public AbstractImportExportBase {
public:
    virtual bool importerTest() {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(ASSIMP_TEST_MODELS_DIR "/N3/chr/mob_bandicoot.n3chr", aiProcess_ValidateDataStructure);
        return nullptr != scene;
    }
};

TEST_F(utN3ImporterExporter, importTest) {
    EXPECT_TRUE(importerTest());
}
