#ifndef __BLOG_MANAGER_COMMENT_MANAGER_H__
#define __BLOG_MANAGER_COMMENT_MANAGER_H__

#include "blog/data/comment_info.h"
#include "sylar/singleton.h"
#include "sylar/mutex.h"
#include <unordered_map>

namespace blog {

class CommentManager {
public:
    bool loadAll();
    void add(blog::data::CommentInfo::ptr info);
    blog::data::CommentInfo::ptr get(int64_t id);
    blog::data::CommentInfo::ptr getByArticleId(int64_t id);
private:
    sylar::RWMutex m_mutex;
    std::unordered_map<int64_t, blog::data::CommentInfo::ptr> m_datas;
    std::unordered_map<int64_t, blog::data::CommentInfo::ptr> m_articles;
};

typedef sylar::Singleton<CommentManager> CommentMgr;

}

#endif
