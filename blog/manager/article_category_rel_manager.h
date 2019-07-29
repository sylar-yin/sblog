#ifndef __BLOG_MANAGER_ARTICLE_CATEGORY_REL_MANAGER_H__
#define __BLOG_MANAGER_ARTICLE_CATEGORY_REL_MANAGER_H__

#include "blog/data/article_category_rel_info.h"
#include "sylar/singleton.h"
#include "sylar/mutex.h"
#include <map>
#include <unordered_map>

namespace blog {

class ArticleCategoryRelManager {
public:
    bool loadAll();
    void add(blog::data::ArticleCategoryRelInfo::ptr info);
    blog::data::ArticleCategoryRelInfo::ptr get(int64_t id);
    bool listByArticleId(std::vector<data::ArticleCategoryRelInfo::ptr>& infos, int64_t id, bool valid);
    blog::data::ArticleCategoryRelInfo::ptr getByArticleIdCategoryId(int64_t article_id, int64_t category_id);

    std::string statusString();
private:
    sylar::RWMutex m_mutex;
    std::unordered_map<int64_t, blog::data::ArticleCategoryRelInfo::ptr> m_datas;
    std::unordered_map<int64_t, std::map<int64_t, blog::data::ArticleCategoryRelInfo::ptr> > m_articles;
};

typedef sylar::Singleton<ArticleCategoryRelManager> ArticleCategoryRelMgr;

}

#endif
