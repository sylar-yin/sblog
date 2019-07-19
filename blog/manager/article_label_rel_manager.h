#ifndef __BLOG_MANAGER_ARTICLE_LABEL_REL_MANAGER_H__
#define __BLOG_MANAGER_ARTICLE_LABEL_REL_MANAGER_H__

#include "blog/data/article_label_rel_info.h"
#include "sylar/singleton.h"
#include "sylar/mutex.h"
#include <unordered_map>

namespace blog {

class ArticleLabelRelManager {
public:
    bool loadAll();
    void add(blog::data::ArticleLabelRelInfo::ptr info);
    blog::data::ArticleLabelRelInfo::ptr get(int64_t id);
    blog::data::ArticleLabelRelInfo::ptr getByArticleId(int64_t id);
    blog::data::ArticleLabelRelInfo::ptr getByLabelId(int64_t id);
private:
    sylar::RWMutex m_mutex;
    std::unordered_map<int64_t, blog::data::ArticleLabelRelInfo::ptr> m_datas;
    std::unordered_map<int64_t, blog::data::ArticleLabelRelInfo::ptr> m_articles;
    std::unordered_map<int64_t, blog::data::ArticleLabelRelInfo::ptr> m_labels;
};

typedef sylar::Singleton<ArticleLabelRelManager> ArticleLabelRelMgr;

}

#endif
