#ifndef TRIANGLES_H
#define TRIANGLES_H

#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include "triangle.h"

class Triangles {
private:
    vector<Triangle> triangles;
    double c;
    
public:
    Triangles(double c, size_t N) : c(c), triangles(N) {}

    void Init() {
        for(auto& triangle : triangles) {
            triangle.Init();
        }
    }

    void Cleanup() {
        for(auto& triangle : triangles) {
            triangle.Cleanup();
        }
        triangles.clear();
    }

    void Draw() {
        for(size_t n = 0; n < triangles.size(); ++n) {
            float theta = (n/10.0 * M_PI);
            float r = 2*(n/180.0 + c);
            float scale = 2*(n/1900.0 + 0.005);

            glm::mat4 rota = glm::mat4(
                            cos(theta),   sin(theta),   0, 0,
                           -sin(theta),   cos(theta),   0, 0,
                            0,            0,            1, 0,
                            0,            0,            0, 1
                        );
            glm::mat4 tran = glm::mat4(
                            1,            0,              0, 0,
                            0,            1,              0, 0,
                            0,            0,              1, 0,
                            r, 0,   0, 1
                        );
            glm::mat4 scal = glm::mat4(
                            scale, 0,     0, 0,
                            0,     scale, 0, 0,
                            0,     0,     1, 0,
                            0,     0,     0, 1
                        );



            triangles[n].Draw(rota*tran*scal);
        }
    }

};

#endif // TRIANGLES_H
