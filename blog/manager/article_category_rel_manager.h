#ifndef __BLOG_MANAGER_ARTICLE_CATEGORY_REL_MANAGER_H__
#define __BLOG_MANAGER_ARTICLE_CATEGORY_REL_MANAGER_H__

#include "blog/data/article_category_rel_info.h"
#include "sylar/singleton.h"
#include "sylar/mutex.h"
#include <unordered_map>

namespace blog {

class ArticleCategoryRelManager {
public:
    bool loadAll();
    void add(blog::data::ArticleCategoryRelInfo::ptr info);
    blog::data::ArticleCategoryRelInfo::ptr get(int64_t id);
    blog::data::ArticleCategoryRelInfo::ptr getByArticleId(int64_t id);
    blog::data::ArticleCategoryRelInfo::ptr getByCategoryId(int64_t id);
private:
    sylar::RWMutex m_mutex;
    std::unordered_map<int64_t, blog::data::ArticleCategoryRelInfo::ptr> m_datas;
    std::unordered_map<int64_t, blog::data::ArticleCategoryRelInfo::ptr> m_articles;
    std::unordered_map<int64_t, blog::data::ArticleCategoryRelInfo::ptr> m_categorys;
};

typedef sylar::Singleton<ArticleCategoryRelManager> ArticleCategoryRelMgr;

}

#endif
