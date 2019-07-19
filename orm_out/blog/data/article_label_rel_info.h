#ifndef __BLOG_DATAARTICLE_LABEL_REL_INFO_H__
#define __BLOG_DATAARTICLE_LABEL_REL_INFO_H__

#include <json/json.h>
#include <vector>
#include "sylar/db/db.h"
#include "sylar/util.h"


namespace blog {
namespace data {

class ArticleLabelRelInfoDao;
class ArticleLabelRelInfo {
friend class ArticleLabelRelInfoDao;
public:
    typedef std::shared_ptr<ArticleLabelRelInfo> ptr;

    ArticleLabelRelInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getArticleId() { return m_articleId; }
    void setArticleId(const int64_t& v);

    const int64_t& getLabelId() { return m_labelId; }
    void setLabelId(const int64_t& v);

    const int32_t& getIsDeleted() { return m_isDeleted; }
    void setIsDeleted(const int32_t& v);

    const int64_t& getCreateTime() { return m_createTime; }
    void setCreateTime(const int64_t& v);

    const int64_t& getUpdateTime() { return m_updateTime; }
    void setUpdateTime(const int64_t& v);

    std::string toJsonString() const;

private:
    int32_t m_isDeleted;
    int64_t m_id;
    int64_t m_articleId;
    int64_t m_labelId;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class ArticleLabelRelInfoDao {
public:
    typedef std::shared_ptr<ArticleLabelRelInfoDao> ptr;
    static int Update(ArticleLabelRelInfo::ptr info, sylar::IDB::ptr conn);
    static int Insert(ArticleLabelRelInfo::ptr info, sylar::IDB::ptr conn);
    static int InsertOrUpdate(ArticleLabelRelInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(ArticleLabelRelInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteById( const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteByArticleId( const int64_t& article_id, sylar::IDB::ptr conn);
    static int QueryAll(std::vector<ArticleLabelRelInfo::ptr>& results, sylar::IDB::ptr conn);
    static ArticleLabelRelInfo::ptr Query( const int64_t& id, sylar::IDB::ptr conn);
    static int QueryByArticleId(std::vector<ArticleLabelRelInfo::ptr>& results,  const int64_t& article_id, sylar::IDB::ptr conn);
    static int CreateTableSQLite3(sylar::IDB::ptr info);
    static int CreateTableMySQL(sylar::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //__BLOG_DATAARTICLE_LABEL_REL_INFO_H__
