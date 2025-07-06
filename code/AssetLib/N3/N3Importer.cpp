#ifndef AI_BUILD_NO_N3_IMPORTER

#include <assimp/scene.h>

#include "N3Importer.h"

using namespace Assimp;
using namespace std;

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

aiQuaternion get_quaternion(StreamReaderLE &reader) {
    aiQuaternion quaternion;
    quaternion.x = reader.GetF4();
    quaternion.y = reader.GetF4();
    quaternion.z = reader.GetF4();
    quaternion.w = reader.GetF4();
    return quaternion;
}

N3Importer::N3Importer() {}
N3Importer::~N3Importer() {}

bool N3Importer::CanRead(const string &filename, IOSystem *pIOHandler, bool checkSig) const {

    pIOHandler->Exists(filename);

    if (checkSig) {
        // Check the signature and return the result
    } else {
        const string extension = GetExtension(filename);
        if (extension == "n3chr") {
            return true;
        }
    }
    return false;
}

const aiImporterDesc *N3Importer::GetInfo() const {
    return &desc;
}

void N3Importer::InternReadFile(const string &filename, aiScene *pScene, IOSystem *pIOHandler) {
    IOStream *stream = pIOHandler->Open(filename, "rb");
    if (!stream) {
        throw DeadlyImportError("Failed to open file ", filename);
    }

    filesystem::path fsFile = filename;
    filesystem::path fsDir = fsFile.parent_path();
    filesystem::path fsBaseDir = fsDir;
    if (fsBaseDir.has_parent_path()) {
        fsBaseDir = fsBaseDir.parent_path();
    }

    StreamReaderLE reader(stream);

    CN3Chr tmp;
    tmp.Load(reader, fsBaseDir, pIOHandler);

    pIOHandler->Exists(filename);
    pScene->HasAnimations();
}

CN3BaseFileAccess::CN3BaseFileAccess() {}
CN3BaseFileAccess::~CN3BaseFileAccess() {}

bool CN3BaseFileAccess::Load(StreamReaderLE &reader) {
    int32_t iLen = reader.GetI4();
    if (iLen > 0) {
        m_szName.assign(iLen, '\0');
        reader.CopyAndAdvance(m_szName.data(), iLen);
    }
    return true;
}

CN3AnimKey::CN3AnimKey() {
    m_nCount = 0;
    m_eType = KEY_VECTOR3;
    m_fSamplingRate = 30.0;
    m_pDatas = nullptr;
}
CN3AnimKey::~CN3AnimKey() {}

bool CN3AnimKey::Load(StreamReaderLE &reader) {
    m_nCount = reader.GetI4();
    if (m_nCount > 0) {
        reader.CopyAndAdvance(&m_eType, 4);
        m_fSamplingRate = reader.GetF4();
        if (KEY_VECTOR3 == m_eType) {
            m_pDatas = new aiVector3D[m_nCount + 1];
            reader.CopyAndAdvance(m_pDatas, sizeof(aiVector3D) * m_nCount);
            aiVector3D *pKeys = (aiVector3D *)m_pDatas;
            pKeys[m_nCount] = pKeys[m_nCount - 1];
        } else if (KEY_QUATERNION == m_eType) {
            m_pDatas = new aiQuaternion[m_nCount + 1];
            __Quaternion *m_pDatasTmp = new __Quaternion[m_nCount];
            reader.CopyAndAdvance(m_pDatasTmp, sizeof(__Quaternion) * m_nCount);
            for (int i = 0; i < m_nCount; ++i) {
                ((aiQuaternion *)m_pDatas)[i].x = m_pDatasTmp[i].x;
                ((aiQuaternion *)m_pDatas)[i].y = m_pDatasTmp[i].y;
                ((aiQuaternion *)m_pDatas)[i].z = m_pDatasTmp[i].z;
                ((aiQuaternion *)m_pDatas)[i].w = m_pDatasTmp[i].w;
            }
            delete[] m_pDatasTmp;
            aiQuaternion *pKeys = (aiQuaternion *)m_pDatas;
            pKeys[m_nCount] = pKeys[m_nCount - 1];
        }
    }
    return true;
}

CN3Transform::CN3Transform() {}
CN3Transform::~CN3Transform() {}

bool CN3Transform::Load(StreamReaderLE &reader) {
    CN3BaseFileAccess::Load(reader);
    
    aiVector3D m_vPos = reader.Get<aiVector3D>();
    aiQuaternion m_qRot = get_quaternion(reader);
    aiVector3D m_vScale = reader.Get<aiVector3D>();

    m_KeyPos.Load(reader);
    m_KeyRot.Load(reader);
    m_KeyScale.Load(reader);
    
    return true;
}

CN3TransformCollision::CN3TransformCollision() {}
CN3TransformCollision::~CN3TransformCollision() {}

bool CN3TransformCollision::Load(StreamReaderLE &reader) {
    CN3Transform::Load(reader);

    int32_t iLen;
    string szFile;

    iLen = reader.GetI4();
    if (iLen > 0) {
        szFile.assign(iLen, '\0');
        reader.CopyAndAdvance(szFile.data(), iLen);
        throw DeadlyImportError("Not implemented");
    }

    iLen = reader.GetI4();
    if (iLen > 0) {
        szFile.assign(iLen, '\0');
        reader.CopyAndAdvance(szFile.data(), iLen);
        throw DeadlyImportError("Not implemented");
    }

    return true;
}

CN3Chr::CN3Chr() {}
CN3Chr::~CN3Chr() {}

bool CN3Chr::Load(StreamReaderLE &reader, filesystem::path fsBaseDir, IOSystem *pIOHandler) {
    CN3TransformCollision::Load(reader);

    string szFile;
    int32_t iLen = reader.GetI4();
    if (iLen > 0) {
        szFile.assign(iLen, '\0');
        reader.CopyAndAdvance(szFile.data(), iLen);

        IOStream *stream = pIOHandler->Open(fsBaseDir.string() + szFile, "rb");
        if (!stream) {
            throw DeadlyImportError("Failed to open file ", szFile);
        }

        StreamReaderLE joint_reader(stream);
        m_pRootJointRef.Load(joint_reader);
        Init();
    }

    return true;
}

void CN3Chr::Init() {
    //if (NULL == m_pRootJointRef) {
    //    return;
    //}

    //int iJC = 0;
    //__Matrix44 mtxTmp;
    //mtxTmp.Identity();
    //m_pRootJointRef->NodeCount(iJC);
    //m_JointRefs.assign(iJC, NULL);
    //m_MtxJoints.assign(iJC, mtxTmp);
    //m_MtxInverses.assign(iJC, mtxTmp);

    //int nJI = 0;
    //m_pRootJointRef->Tick(0);
    //for (int i = 0; i < iJC; i++)
    //{
    //    m_JointRefs[i] = NULL;
    //    m_pRootJointRef->FindPointerByID(i, m_JointRefs[i]);
    //    ::D3DXMatrixInverse(&(m_MtxInverses[i]), NULL, &(m_JointRefs[i]->m_Matrix));
    //    m_MtxJoints[i] = m_JointRefs[i]->m_Matrix;
    //}

    //this->RemakePlugTracePolygons();

    //this->FindMinMax();

    //if (NULL == m_pMeshCollision) {
    //    m_pMeshCollision = new CN3VMesh();
    //}
    //m_pMeshCollision->CreateCube(m_vMin, m_vMax);
}

CN3Joint::CN3Joint() {
    m_pParent = nullptr;
}
CN3Joint::~CN3Joint() {}

bool CN3Joint::Load(StreamReaderLE &reader) {
    CN3Transform::Load(reader);

    m_KeyOrient.Load(reader);

    int nCC = reader.GetI4();
    for (int i = 0; i < nCC; i++) {
        CN3Joint *pChild = new CN3Joint();
        this->ChildAdd(pChild);

        pChild->Load(reader);
    }

    return true;
}

void CN3Joint::ChildAdd(CN3Joint *pChild) {
    CN3Joint *pChild2 = nullptr;
    it_Joint it = m_Children.begin();
    size_t iSize = m_Children.size();
    for (size_t i = 0; i < iSize; i++, it++) {
        pChild2 = *it;
        if (pChild2 == pChild) {
            return;
        }
    }
    m_Children.push_back(pChild);
    pChild->ParentSet(this);
}

void CN3Joint::ParentSet(CN3Joint *pParent) {
    if (pParent == m_pParent) {
        return;
    }

    m_pParent = pParent;
    if (pParent) {
        pParent->ChildAdd(this);
    }
}

#endif
