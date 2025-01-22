module;
#include <glm/glm.hpp>
export module Face;

namespace Engine {
    export class Face {
    public:
        int v0; // Index of the first vertex in the Mesh's vertex array
        int v1; // Index of the second vertex
        int v2; // Index of the third vertex

        Face(int v0, int v1, int v2)
            : v0(v0), v1(v1), v2(v2) {
        }
    };
}
