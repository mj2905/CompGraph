#pragma once
#include <vector>
#include "icg_helper.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

/*
vec3 operator*(const vec3& a, double b) {
    return vec3(a.x * b, a.y * b, a.z * b);
}
*/

class Bezier {

private:

    const vector<vec3> points;

    vec3 apply(const vector<vec3>& points, float t) {
        assert(points.size() >= 1);
        if(points.size() == 1) {
            return points[0];
        }
        else {
            vector<vec3> newPoints(points.size()-1);
            for(size_t i = 0; i < newPoints.size(); ++i) {
                newPoints[i] = (1-t) * points[i] + t * points[i+1];
            }
            return apply(newPoints, t);
        }
    }

    const size_t segments;
    vector<vec3> samples;

    vector<vec3> computeSamples() {
        double step = 1.0/segments;
        vector<vec3> segs;
        for(size_t i = 0; i <= segments; ++i) {
            double t = i * step;
            segs.push_back(apply(points, (float)t));
        }
        return segs;
    }

    vec3 interpolationSamples(float t) {
        size_t segment = t * segments;
        if(segment >= segments) {
            return samples[samples.size() - 1];
        }

        float variable = t * segments - segment;

        return (1-variable) * samples[segment] + variable*samples[segment+1];
    }


public:

    Bezier(vector<vec3>& points, size_t segments = 30) : points(points), segments(segments) {
        assert(this->points.size() >= 2);
        assert(segments >= 1);
        samples = computeSamples();
    }

    vec3 apply(float t) {
        assert(t >= 0 and t <= 1);
        //return apply(points, t);
        return interpolationSamples(t);
    }

    float getSegmentSize(float t) {
        size_t segment = t * segments;
        if(segment >= segments) {
            return 1;
        }
        return glm::distance(samples[segment], samples[segment+1]);
    }

};
