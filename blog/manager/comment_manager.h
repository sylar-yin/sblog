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
    int64_t listByArticleId(std::vector<blog::data::CommentInfo::ptr>& infos,
            int64_t article_id, int64_t offset, int64_t size, bool valid);

    void delVerify(int64_t id);
    void addVerify(data::CommentInfo::ptr info);

    int64_t listVerifyPages(std::vector<data::CommentInfo::ptr>& infos
                            ,int32_t offset, int32_t size);

    std::string statusString();
private:
    sylar::RWMutex m_mutex;
    std::unordered_map<int64_t, blog::data::CommentInfo::ptr> m_datas;
    std::unordered_map<int64_t, std::map<int64_t, blog::data::CommentInfo::ptr> > m_articles;
    std::map<int64_t, blog::data::CommentInfo::ptr> m_verifys;
};

typedef sylar::Singleton<CommentManager> CommentMgr;

}

#endif
