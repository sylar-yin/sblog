#ifndef __BLOG_WORD_PARSER_H__
#define __BLOG_WORD_PARSER_H__

#include <cppjieba/Jieba.hpp>
#include "sylar/sylar.h"

namespace blog {

class WordParser {
public:
    WordParser();
    void cut(const std::string& sentence, std::vector<std::string>& words, bool hmm = true) const;
    void cutAll(const std::string& sentence, std::vector<std::string>& words) const;
    void cutForSearch(const std::string& sentence, std::vector<std::string>& words, bool hmm = true) const;
    void cutHMM(const std::string& sentence, std::vector<std::string>& words) const;
    void cutSmall(const std::string& sentence, std::vector<std::string>& words, size_t max_word_len) const;
    void extract(const std::string& sentence, std::vector<std::string>& keywords, size_t topN) const;
private:
    std::shared_ptr<cppjieba::Jieba> m_jieba;
};

typedef sylar::Singleton<WordParser> WordParserMgr;

}

#endif
