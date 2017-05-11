#include <string.h>
#include <unordered_map>
#include <vector>

using namespace std;

/**
 * @brief The RuleSystem class
 * It describes a set of rules in an L-system.
 */
class RuleSystem{
private:
    char axiom;
    unordered_map<char,string>rules;

public:
    void Init(char axiom){
        this->axiom = axiom;
    }

    void addRule(char b, string trans){
        if(rules.count(b) == 0)  rules.emplace(make_pair(b, trans));
    }

    string getRule(char k){
        if(rules.count(k) !=0){
            return rules.at(k);
        }else{
            return "";
        }
    }

};
