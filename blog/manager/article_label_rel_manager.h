#ifndef __BLOG_MANAGER_ARTICLE_LABEL_REL_MANAGER_H__
#define __BLOG_MANAGER_ARTICLE_LABEL_REL_MANAGER_H__

#include "blog/data/article_label_rel_info.h"
#include "sylar/singleton.h"
#include "sylar/mutex.h"
#include <map>
#include <unordered_map>

namespace blog {

class ArticleLabelRelManager {
public:
    bool loadAll();
    void add(blog::data::ArticleLabelRelInfo::ptr info);
    blog::data::ArticleLabelRelInfo::ptr get(int64_t id);
    blog::data::ArticleLabelRelInfo::ptr getByArticleIdLabelId(int64_t article_id, int64_t label_id);
    bool listByArticleId(std::vector<data::ArticleLabelRelInfo::ptr>& infos, int64_t article_id, bool valid);
private:
    sylar::RWMutex m_mutex;
    std::unordered_map<int64_t, blog::data::ArticleLabelRelInfo::ptr> m_datas;
    std::unordered_map<int64_t, std::map<int64_t, blog::data::ArticleLabelRelInfo::ptr> > m_articles;
};

typedef sylar::Singleton<ArticleLabelRelManager> ArticleLabelRelMgr;

}

#endif
