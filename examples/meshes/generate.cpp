#include "../../include/mesh4d.hpp"


Mesh4D makeTesseract(const float size = 1.0f) {
    Mesh4D m;
    for (int i = 0; i < 16; ++i) {
        m.addVertex(Vec4{
            (i & 1) ? size : -size,
            (i & 2) ? size : -size,
            (i & 4) ? size : -size,
            (i & 8) ? size : -size
        });
    }

    // 24 faces
    for (int dim1 = 0; dim1 < 3; ++dim1) {
        for (int dim2 = dim1 + 1; dim2 < 4; ++dim2) {
            int other1 = -1, other2 = -1;
            for (int k = 0; k < 4; ++k) {
                if (k != dim1 && k != dim2) {
                    if (other1 == -1) other1 = k;
                    else other2 = k;
                }
            }

            for (int k = 0; k < 4; ++k) {
                const int bit1 = (k & 1) ? 1 : 0;
                const int bit2 = (k & 2) ? 1 : 0;

                const int baseMask = (bit1 << other1) | (bit2 << other2);

                const int v00 = baseMask;
                const int v10 = baseMask | (1 << dim1);
                const int v01 = baseMask | (1 << dim2);
                const int v11 = baseMask | (1 << dim1) | (1 << dim2);

                m.addTriangle(v00, v10, v01);
                m.addTriangle(v10, v11, v01);
            }
        }
    }
    return m;
}

int main(int argc, char* argv[]) {
    std::cout << "Generating Tesseract" << std::endl;
    const bool tesseract = makeTesseract().writeToFile("./tesseract.m4d");
    if (!tesseract) {
        std::cout << "tesseract failed" << std::endl;
    }
    std::cout << "Finished all meshes" << std::endl;
    return tesseract;
}
