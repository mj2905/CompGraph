#include <vector>

using namespace std;
class LightSource{
private:
    vector<float> positions;
public:
    void Init(float x, float y, float z){
        positions.clear();
        positions.push_back(x);
        positions.push_back(y);
        positions.push_back(z);
    }

    vector<float> getPosition(){
        return positions;
    }

    void modifyLight(float x, float y, float z){
        positions.at(0) = x;
        positions.at(1) = y;
        positions.at(2) = z;
    }
};
