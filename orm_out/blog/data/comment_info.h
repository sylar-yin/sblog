#ifndef __BLOG_DATACOMMENT_INFO_H__
#define __BLOG_DATACOMMENT_INFO_H__

#include <json/json.h>
#include <vector>
#include "sylar/db/db.h"
#include "sylar/util.h"


namespace blog {
namespace data {

class CommentInfoDao;
class CommentInfo {
friend class CommentInfoDao;
public:
    typedef std::shared_ptr<CommentInfo> ptr;

    CommentInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getUserId() { return m_userId; }
    void setUserId(const int64_t& v);

    const int64_t& getArticleId() { return m_articleId; }
    void setArticleId(const int64_t& v);

    const std::string& getContent() { return m_content; }
    void setContent(const std::string& v);

    const int64_t& getParentId() { return m_parentId; }
    void setParentId(const int64_t& v);

    const int32_t& getState() { return m_state; }
    void setState(const int32_t& v);

    const int32_t& getIsDeleted() { return m_isDeleted; }
    void setIsDeleted(const int32_t& v);

    const int64_t& getCreateTime() { return m_createTime; }
    void setCreateTime(const int64_t& v);

    const int64_t& getUpdateTime() { return m_updateTime; }
    void setUpdateTime(const int64_t& v);

    std::string toJsonString() const;

private:
    int32_t m_state;
    int32_t m_isDeleted;
    int64_t m_id;
    int64_t m_userId;
    int64_t m_articleId;
    int64_t m_parentId;
    std::string m_content;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class CommentInfoDao {
public:
    typedef std::shared_ptr<CommentInfoDao> ptr;
    static int Update(CommentInfo::ptr info, sylar::IDB::ptr conn);
    static int Insert(CommentInfo::ptr info, sylar::IDB::ptr conn);
    static int InsertOrUpdate(CommentInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(CommentInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteById( const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteByUserId( const int64_t& user_id, sylar::IDB::ptr conn);
    static int DeleteByArticleId( const int64_t& article_id, sylar::IDB::ptr conn);
    static int QueryAll(std::vector<CommentInfo::ptr>& results, sylar::IDB::ptr conn);
    static CommentInfo::ptr Query( const int64_t& id, sylar::IDB::ptr conn);
    static int QueryByUserId(std::vector<CommentInfo::ptr>& results,  const int64_t& user_id, sylar::IDB::ptr conn);
    static int QueryByArticleId(std::vector<CommentInfo::ptr>& results,  const int64_t& article_id, sylar::IDB::ptr conn);
    static int CreateTableSQLite3(sylar::IDB::ptr info);
    static int CreateTableMySQL(sylar::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //__BLOG_DATACOMMENT_INFO_H__
