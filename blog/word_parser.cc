#include "word_parser.h"

namespace blog {

WordParser::WordParser() {
    m_jieba.reset(new cppjieba::Jieba(
        sylar::EnvMgr::GetInstance()->getAbsolutePath("dict/jieba.dict.utf8"),
        sylar::EnvMgr::GetInstance()->getAbsolutePath("dict/hmm_model.utf8"),
        sylar::EnvMgr::GetInstance()->getAbsolutePath("dict/user.dict.utf8"),
        sylar::EnvMgr::GetInstance()->getAbsolutePath("dict/idf.utf8"),
        sylar::EnvMgr::GetInstance()->getAbsolutePath("dict/stop_words.utf8")
    ));
}

void WordParser::cut(const std::string& sentence, std::vector<std::string>& words, bool hmm) const {
    m_jieba->Cut(sentence, words, hmm);
}

void WordParser::cutAll(const std::string& sentence, std::vector<std::string>& words) const {
    m_jieba->CutAll(sentence, words);
}
void WordParser::cutForSearch(const std::string& sentence, std::vector<std::string>& words, bool hmm) const {
    m_jieba->CutForSearch(sentence, words, hmm);
}

void WordParser::cutHMM(const std::string& sentence, std::vector<std::string>& words) const {
    m_jieba->CutHMM(sentence, words);
}

void WordParser::cutSmall(const std::string& sentence, std::vector<std::string>& words, size_t max_word_len) const {
    m_jieba->CutSmall(sentence, words, max_word_len);
}

void WordParser::extract(const std::string& sentence, std::vector<std::string>& keywords, size_t topN) const {
    return m_jieba->extractor.Extract(sentence, keywords, topN);
}

}
