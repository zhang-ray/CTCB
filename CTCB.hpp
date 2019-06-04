#pragma once



#include <algorithm>
#include <cstddef>
#include <queue>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <tuple>
#include <unordered_map>

namespace CTCB{

using ChineseCharList = std::vector<uint16_t>;



class PrefixTree{
public:
    class Node{
    public:
        std::map<uint16_t, std::shared_ptr<Node>> children_;
        bool isOneSentence_ = false;
    };
public:
    static constexpr uint16_t dicLen_ = 1422;
    static constexpr uint16_t blankIdx_ = dicLen_ - 1;
public:
    PrefixTree(const std::vector<ChineseCharList> &corpus){
        for (const auto &sentence : corpus){
            addOneSentence(sentence);
        }
    }
private:
    std::shared_ptr<Node> root_ = std::make_shared<Node>();

public:
    void addOneSentence(const ChineseCharList &sentence) {
        std::shared_ptr<Node> node = root_;
        for (auto i = 0; i < sentence.size(); i++){
            auto c = sentence[i];
            if (!node->children_[c]){
                node->children_[c] = std::make_shared<Node>();
            }

            node = node->children_[c];
            auto isLast = ((i + 1) == sentence.size());
            if (isLast){
                node->isOneSentence_ = true;
            }
        }
    }

    decltype(PrefixTree::root_) getNode(const ChineseCharList &sentence) const {
        auto node = root_;
        for (const auto & c : sentence){
            if (node->children_[c]){
                node = node->children_[c];
            }
            else {
                return nullptr;
            }
        }
        return node;
    }


    auto isSentence(const ChineseCharList &sentence) const {
        auto node = getNode(sentence);
        if (node){
            return node->isOneSentence_;
        }
        return false;
    }

    auto getNextChars(const ChineseCharList &sentence) const {
        ChineseCharList chars;
        auto node = getNode(sentence);
        if (node){
            auto allChildren = node->children_;
            for(auto it = allChildren.begin(); it != allChildren.end(); ++it) {
                chars.push_back(it->first);
            }
        }

        return chars;
    }

    auto getNextSentence(const ChineseCharList &sentence) const{
        std::vector<ChineseCharList> sentences;
        auto node = getNode(sentence);
        if (node){
            std::queue<decltype(node)> nodes;
            nodes.push(node);

            std::queue<ChineseCharList> prefixes;
            prefixes.push(sentence);

            for ( ;nodes.size()>0; ){
                if (nullptr == nodes.front()){
                    continue;
                }

                for (auto it = nodes.front()->children_.begin(); it != nodes.front()->children_.end(); ++it){
                    nodes.push(it->second);

                    auto p0 = prefixes.front();
                    p0.push_back(it->first);
                    prefixes.push(p0);
                }

                if (nodes.front()->isOneSentence_){
                    sentences.push_back(prefixes.front());
                }

                nodes.pop();
                prefixes.pop();
            }
        }

        return sentences;
    }
};

struct ChineseCharListHasher{
    size_t operator()(const ChineseCharList& charList) const {
        std::size_t result = charList.size();
        for (const auto& l : charList){
            result ^= l + 0x9e3779b9 + (result << 6) + (result >> 2);
        }
        return result;
    }
};




class Beam{
public:
    double probBlank_ = 1.0;
    double probNonBlank_ = 0;

    ChineseCharList charList_;
    ChineseCharList devOfCharList_;

    PrefixTree tree_;

public:
    Beam(PrefixTree &tree)
        :  tree_(tree)
    {  }


    const auto getCharList(){ return charList_; }

    const auto getProbBlank(){ return probBlank_;}

    auto getProbNonBlank(){return probNonBlank_;}

    auto getProbTotal() {return getProbBlank()+getProbNonBlank();}

    auto getNextChars(){return tree_.getNextChars(devOfCharList_);}

    auto isCharEmptyOrAllCtcBlank(){
        if (charList_.size()==0){
            return true;
        }

        bool allBlank=true;
        for (auto & c: charList_){
            if (c != PrefixTree::blankIdx_){
                allBlank=false;
                break;
            }
        }

        return allBlank;
    }


    void mergeBeam(const std::shared_ptr<Beam>& beam) {
        probBlank_ += beam->probBlank_;
        probNonBlank_ += beam->probNonBlank_;
    }


    auto createChildBeam(uint16_t newChar, double prBlank, double prNonBlank){
        auto beam = std::make_shared<Beam>(tree_);
        beam->devOfCharList_= devOfCharList_;
        beam->charList_= charList_;
        beam->charList_.push_back(newChar);

        if (newChar!=PrefixTree::blankIdx_){
            beam->devOfCharList_.push_back(newChar);
        }

        beam->probBlank_ = prBlank;
        beam->probNonBlank_ = prNonBlank;
        return beam;
    }

};


class BeamList{
public:
    void addBeam(const std::shared_ptr<Beam>& beam){
        auto iter = m_beams.find(beam->getCharList());
        if (iter == m_beams.end()) {
            m_beams[beam->getCharList()] = beam;
        }
        else {
            iter->second->mergeBeam(beam);
        }
    }

    auto getBestBeams(const size_t beamWidth) const {
        typedef std::pair<ChineseCharList, std::shared_ptr<Beam>> KeyValueType;
        std::vector<KeyValueType> beams(m_beams.begin(), m_beams.end());
        std::sort(beams.begin(), beams.end(), [](const KeyValueType& a, const KeyValueType& b) {
            return a.second->getProbTotal() > b.second->getProbTotal();
        });


        std::vector<std::shared_ptr<Beam>> result;
        result.reserve(beamWidth);
        for (size_t i = 0; (i < beams.size()) && (i < beamWidth); ++i){
            result.push_back(beams[i].second);
        }
        return result;
    }

    auto completeBeams(const PrefixTree &tree){
        for(auto kv : m_beams) {
            auto lastPrefix = kv.second->devOfCharList_;
            if (lastPrefix.empty() || tree.isSentence(lastPrefix)){
                continue;
            }

            auto sentences = tree.getNextSentence(lastPrefix);

            if (sentences.size() != 1){
                continue;
            }
            auto sentence = sentences[0];

            auto startPoint = lastPrefix.size() - sentence.size();
            for (;startPoint <0;){
                startPoint += sentence.size();
            }
            for (auto c : sentence){
                kv.second->charList_.push_back(c);
            }
        }
    }

private:
    std::unordered_map<ChineseCharList, std::shared_ptr<Beam>, ChineseCharListHasher> m_beams;
};




class CSV{
private:
    std::vector<std::vector<double>> data_;
    size_t row_;
    size_t col_;

public:
    size_t rows() const { return row_; }
    size_t cols() const { return col_; }

public:
    CSV(const std::string& filename){
        std::ifstream ifs(filename);
        std::string line;

        for (;std::getline(ifs, line);) {
            std::vector<double> row;
            std::string tmp;
            for (size_t i = 0; i < line.size(); ++i){
                char c = line[i];
                if (c == ';' || i + 1 == line.size()){
                    row.push_back(std::stod(tmp));
                    tmp = "";
                }
                else{
                    tmp.push_back(c);
                }
            }

            if (!row.empty()) {
                data_.push_back(row);
            }
        }

        row_ = data_.size();
        col_ = data_.front().size();
    }

    double get(size_t row, size_t col) const{ return data_[row][col]; }
    void set(size_t row, size_t col, double v){ data_[row][col] = v; }
};





class CommandResult{
public:
    ChineseCharList charList_;
public:
    CommandResult(const decltype(charList_) charList){
        for (const auto &t : charList){
            if (t!=PrefixTree::blankIdx_){
                charList_.push_back(t);
            }
        }
    }
};



CommandResult CTCB(const std::vector<ChineseCharList> &copus, const CSV &mat ){
    PrefixTree tree(copus);

    const size_t maxT = mat.rows();

    BeamList last;

    last.addBeam(std::make_shared<Beam>(tree));

    for (size_t t = 0; t < maxT; ++t) {
        BeamList curr;
        auto bestBeams = last.getBestBeams(25);

        for (const auto &beam : bestBeams){
            auto prNonBlank = 0.0;
            if (!beam->isCharEmptyOrAllCtcBlank()){
                auto theCharList = beam->getCharList();
                auto labelIdx = theCharList[theCharList.size()-1];
                prNonBlank = beam->getProbNonBlank() * mat.get(t, labelIdx);
            }

            auto prBlank = beam->getProbTotal() * mat.get(t, PrefixTree::blankIdx_);
            curr.addBeam(beam->createChildBeam(PrefixTree::blankIdx_, prBlank, prNonBlank));

            auto nextChars = beam->getNextChars();

            for (const auto &c : nextChars){
                auto labelIdx = c;
                auto theCharList = beam->getCharList();
                bool OK = false;
                if (theCharList.size()>0){
                    auto labelIdxOfLast = theCharList[theCharList.size()-1];
                    if ((!beam->isCharEmptyOrAllCtcBlank()) &&(labelIdxOfLast == c)){
                        prNonBlank = mat.get(t, labelIdx) * beam->getProbBlank();
                        OK=true;
                    }
                }

                if(!OK){
                    prNonBlank = mat.get(t, labelIdx) * beam->getProbTotal();
                }
                curr.addBeam(beam->createChildBeam(c, 0, prNonBlank));
            }
        }
        last=curr;
    }

    last.completeBeams(tree);

    auto bestBeam = last.getBestBeams(1)[0];

    return bestBeam->getCharList();
}

}
