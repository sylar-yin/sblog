#ifndef __BLOG_MANAGER_ARTICLE_MANAGER_H__
#define __BLOG_MANAGER_ARTICLE_MANAGER_H__

#include "blog/data/article_info.h"
#include "sylar/singleton.h"
#include "sylar/mutex.h"
#include <map>
#include <unordered_map>

namespace blog {

class ArticleManager {
public:
    bool loadAll();
    void add(blog::data::ArticleInfo::ptr info);
    blog::data::ArticleInfo::ptr get(int64_t id);
    bool listByUserId(std::vector<data::ArticleInfo::ptr>& infos, int64_t id, bool valid);
    int64_t listByUserIdPages(std::vector<data::ArticleInfo::ptr>& infos, int64_t id
                              ,int32_t offset, int32_t size, bool valid);

    void delVerify(int64_t id);
    void addVerify(data::ArticleInfo::ptr info);

    int64_t listVerifyPages(std::vector<data::ArticleInfo::ptr>& infos
                            ,int32_t offset, int32_t size);


private:
    sylar::RWMutex m_mutex;
    std::map<int64_t, blog::data::ArticleInfo::ptr> m_datas;
    std::unordered_map<int64_t, std::map<int64_t, blog::data::ArticleInfo::ptr> > m_users;
    std::map<int64_t, blog::data::ArticleInfo::ptr> m_verifys;
};

typedef sylar::Singleton<ArticleManager> ArticleMgr;

}

#endif
