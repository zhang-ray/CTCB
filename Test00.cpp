#include "CTCB.hpp"

#include <cstdio>

int test00(void){
    std::vector<CTCB::ChineseCharList>  copus;

    CTCB::CSV mat("/tmp/mat_test00.csv");


    copus.push_back({958, 769});
    copus.push_back({472, 769});
    copus.push_back({798, 406, 1088, 374, 1162});
    copus.push_back({190, 472, 25, 29});
    copus.push_back({805, 143, 25, 29});
    copus.push_back({190, 472, 460, 344, 210});
    copus.push_back({1194, 426, 560, 239});
    copus.push_back({436, 213, 560, 239});


    printf("result:\n");
    auto result = CTCB::CTCB(copus, mat);
    for (auto &v: result.charList_){
        printf("%d ", v);
    }
    printf("\n");

    return 0;
}


int main(void){
    return test00();
}
