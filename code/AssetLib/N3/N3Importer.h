#pragma once
#ifndef N3_FILE_IMPORTER_H_INC
#define N3_FILE_IMPORTER_H_INC

#include <list>
#include <filesystem>
#include <assimp/BaseImporter.h>
#include <assimp/StreamReader.h>
#include <assimp/IOSystem.hpp>

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

class CN3Joint;
typedef typename std::list<CN3Joint *>::iterator it_Joint;

enum ANIMATION_KEY_TYPE {
    KEY_VECTOR3 = 0,
    KEY_QUATERNION = 1,
    KEY_UNKNOWN = 0xffffffff
};

typedef struct D3DXQUATERNION {
    float x, y, z, w;
} __Quaternion;

class CN3BaseFileAccess {
public:
    CN3BaseFileAccess();
    ~CN3BaseFileAccess();

    std::string m_szName;

    bool Load(Assimp::StreamReaderLE &reader);
};

class CN3AnimKey {
public:
    CN3AnimKey();
    ~CN3AnimKey();

    int m_nCount;
    ANIMATION_KEY_TYPE m_eType;
    float m_fSamplingRate;
    void *m_pDatas;

    bool Load(Assimp::StreamReaderLE &reader);
};

class CN3Transform : public CN3BaseFileAccess {
public:
    CN3Transform();
    ~CN3Transform();

    CN3AnimKey m_KeyPos;
    CN3AnimKey m_KeyRot;
    CN3AnimKey m_KeyScale;

    bool Load(Assimp::StreamReaderLE &reader);
};

class CN3TransformCollision : public CN3Transform {
public:
    CN3TransformCollision();
    ~CN3TransformCollision();

    bool Load(Assimp::StreamReaderLE &reader);
};

class CN3Joint : public CN3Transform {
public:
    CN3Joint();
    ~CN3Joint();

    CN3AnimKey m_KeyOrient;
    std::list<CN3Joint *> m_Children;
    CN3Joint *m_pParent;

    bool Load(Assimp::StreamReaderLE &reader);
    void ChildAdd(CN3Joint *pChild);
    void ParentSet(CN3Joint *pParent);
};

class CN3Chr : public CN3TransformCollision {
public:
    CN3Chr();
    ~CN3Chr();

    CN3Joint m_pRootJointRef;

    bool Load(Assimp::StreamReaderLE &reader, std::filesystem::path fsBaseDir, Assimp::IOSystem *pIOHandler);
    void Init();
};

#endif
