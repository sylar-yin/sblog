#ifndef __BLOG_MANAGER_ARTICLE_MANAGER_H__
#define __BLOG_MANAGER_ARTICLE_MANAGER_H__

#include "blog/data/article_info.h"
#include "sylar/singleton.h"
#include "sylar/mutex.h"
#include <unordered_map>

namespace blog {

class ArticleManager {
public:
    bool loadAll();
    void add(blog::data::ArticleInfo::ptr info);
    blog::data::ArticleInfo::ptr get(int64_t id);
    blog::data::ArticleInfo::ptr getByUserId(int64_t id);
private:
    sylar::RWMutex m_mutex;
    std::unordered_map<int64_t, blog::data::ArticleInfo::ptr> m_datas;
    std::unordered_map<int64_t, blog::data::ArticleInfo::ptr> m_users;
};

typedef sylar::Singleton<ArticleManager> ArticleMgr;

}

#endif
