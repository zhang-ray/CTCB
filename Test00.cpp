#include "CTCB.hpp"

#include <cstdio>

int test00(void){
    std::vector<CTCB::ChineseCharList>  copus;

    ::CSV mat("/tmp/mat_test00.csv");
    auto finalMat = mat.toMergedMatrix();


    copus.push_back({958, 769});
    copus.push_back({472, 769});
    copus.push_back({798, 406, 1088, 374, 1162});
    copus.push_back({190, 472, 25, 29});
    copus.push_back({805, 143, 25, 29});
    copus.push_back({190, 472, 460, 344, 210});
    copus.push_back({1194, 426, 560, 239});
    copus.push_back({436, 213, 560, 239});


    printf("result:\n");
    auto result = CTCB::CTCB(copus, finalMat);
    for (auto &v: result.charList_){
        printf("%d ", v);
    }
    printf("\n");

    return 0;
}


int test01(void){
    std::vector<CTCB::ChineseCharList>  copus;

    ::CSV mat("/tmp/mat_test00.csv");
    auto mergedMat = mat.toMergedMatrix();


    copus.push_back({958, 769});
    copus.push_back({472, 769});
    copus.push_back({798, 406, 1088, 374, 1162});
    copus.push_back({190, 472, 25, 29});
    copus.push_back({805, 143, 25, 29});
    copus.push_back({190, 472, 460, 344, 210});
    copus.push_back({1194, 426, 560, 239});
    copus.push_back({436, 213, 560, 239});


    std::vector<uint16_t> mergedCopus;
    for (auto &oneCopus : copus){
        for (auto &v : oneCopus){
            mergedCopus.push_back(v);
        }
        mergedCopus.push_back(0);
    }


    printf("result:\n");
    auto result = runCtc(mergedCopus, mergedMat);
    for (auto &v: result.charList_){
        printf("%d ", v);
    }

    printf("\n");

    return 0;
}


int main(void){
    test00();
    return test01();
}
