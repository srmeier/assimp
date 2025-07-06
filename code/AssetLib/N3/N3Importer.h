#pragma once
#ifndef N3_FILE_IMPORTER_H_INC
#define N3_FILE_IMPORTER_H_INC

#include <assimp/BaseImporter.h>

class N3Importer : public BaseImporter {
public:
    N3Importer() : BaseImporter = default;

    N3Importer() override = default;

    bool CanRead(const std::string &filename, IOSystem *pIOHandler, bool checkSig) const override {
        if (checkSig) {
            // Check the signature and return the result
        } else {
            const std::string extension = GetExtension(filename));
            if ( extension == "n3") {
                return true;
            }
        }
        return false;
    }

    void InternReadFile() {
        // Add your code here
    }
};
