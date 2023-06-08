#include "domain.h"

using namespace std;


string utils::GetToken(const string& str, int& pos, char to_find){
    if (pos >= str.length()){
        return "";
    }
    int first_pos = pos;
    auto ret_pos = str.find(to_find, first_pos);
    if (ret_pos == string::npos) {
        ret_pos = str.length();
    }
    pos = ret_pos + 2;
    return str.substr(first_pos, ret_pos - first_pos);
}