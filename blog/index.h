#ifndef __BLOG_INDEX_H__
#define __BLOG_INDEX_H__

#include "sylar/ds/bitmap.h"
#include "sylar/mutex.h"
#include "blog/manager/article_manager.h"
#include "sylar/singleton.h"

namespace blog {

enum class IndexType {
    USER_ID = 1,
    CAT_ID = 2,
    LABEL_ID = 3,
    CAT_NAME = 4,
    LABEL_NAME = 5, 
    STATE = 6,
    YEAR_MON = 7,
    CHANNEL = 8,

    WORD = 100
};

#define INDEX_TYPE_MACRO(XX) \
    XX("user_id",     IndexType::USER_ID,      1)\
    XX("cat_id",      IndexType::CAT_ID,       1)\
    XX("label_id",    IndexType::LABEL_ID,     1)\
    XX("cat_name",    IndexType::CAT_NAME,     2)\
    XX("label_name",  IndexType::LABEL_NAME,   2)\
    XX("state",       IndexType::STATE,        1)\
    XX("yearmon",     IndexType::YEAR_MON,     2)\
    XX("channel",     IndexType::CHANNEL,      1)\
    XX("word",        IndexType::WORD,     2)\


void ParseParams(std::map<uint64_t, std::set<uint64_t> >& params,
                 const std::map<std::string, std::string>& input_params);

void ParseFields(std::map<uint64_t, std::set<uint64_t> >& params,
                 const std::string& str);

std::string GetIndexTypeName(uint64_t id);
int GetIndexTypeType(uint64_t id);


class Index {
public:
    typedef std::shared_ptr<Index> ptr;
    Index();
    bool set(uint64_t type, uint64_t key, uint32_t idx, bool v);
    sylar::ds::Bitmap::ptr get(uint64_t type, uint64_t key);
    void build();

    void buildIdx(data::ArticleInfo::ptr info, uint32_t idx);

    int32_t search(std::vector<uint64_t>& ids, const std::map<uint64_t, std::set<uint64_t> >& params
                    ,uint32_t max_size);
    int32_t property(std::map<uint64_t, std::map<uint64_t, uint64_t> >& props,
                     const std::map<uint64_t, std::set<uint64_t> >& params,
                     std::map<uint64_t, std::set<uint64_t> >& querys);

    std::string toString();

    static uint64_t StrHash(const std::string& str);

    std::string getStr(uint64_t id);
private:
    sylar::ds::Bitmap::ptr query(const std::map<uint64_t, std::set<uint64_t> >& params);
    uint64_t hash(const std::string& str, bool save);

    void buildWordIdx(const std::string& str, uint32_t idx);
private:
    uint64_t m_createTime;
    uint64_t m_endTime;
    std::vector<uint64_t> m_docs;
    std::map<uint64_t, std::map<uint64_t, sylar::ds::Bitmap::ptr> > m_indexs;
    std::unordered_map<uint64_t, std::string> m_strings;
};

class IndexManager {
public:
    Index::ptr get();
    void build();
private:
    sylar::RWMutex m_mutex;
    Index::ptr m_index;
    Index::ptr m_next;
};

typedef sylar::Singleton<IndexManager> IndexMgr;

}

#endif
