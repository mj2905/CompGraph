#include <vector>

using namespace std;

/**
 *A template representing a stack.
 */
template<class A> class Stack{
private:
    vector<A> s;
public:
    void push_back(A e){
        s.push_back(e);
    }

    A at(int index){
        return s.at(index);
    }

    void clear(){
        s.clear();
    }

    A back(){
        return s.back();
    }

    int size(){
        return s.size();
    }

    int capacity(){
        return s.capacity();
    }


    void popStack(){
        s.pop_back();
    }

    A popStackAndReturnNextBack(){
        s.pop_back();
        return s.back();
    }

};
