#ifndef __BLOG_DATAARTICLE_CATEGORY_REL_INFO_H__
#define __BLOG_DATAARTICLE_CATEGORY_REL_INFO_H__

#include <json/json.h>
#include <vector>
#include "sylar/db/db.h"
#include "sylar/util.h"


namespace blog {
namespace data {

class ArticleCategoryRelInfoDao;
class ArticleCategoryRelInfo {
friend class ArticleCategoryRelInfoDao;
public:
    typedef std::shared_ptr<ArticleCategoryRelInfo> ptr;

    ArticleCategoryRelInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getArticleId() { return m_articleId; }
    void setArticleId(const int64_t& v);

    const int64_t& getCategoryId() { return m_categoryId; }
    void setCategoryId(const int64_t& v);

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
    int64_t m_categoryId;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class ArticleCategoryRelInfoDao {
public:
    typedef std::shared_ptr<ArticleCategoryRelInfoDao> ptr;
    static int Update(ArticleCategoryRelInfo::ptr info, sylar::IDB::ptr conn);
    static int Insert(ArticleCategoryRelInfo::ptr info, sylar::IDB::ptr conn);
    static int InsertOrUpdate(ArticleCategoryRelInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(ArticleCategoryRelInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteById( const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteByArticleId( const int64_t& article_id, sylar::IDB::ptr conn);
    static int DeleteByArticleIdCategoryId( const int64_t& article_id,  const int64_t& category_id, sylar::IDB::ptr conn);
    static int QueryAll(std::vector<ArticleCategoryRelInfo::ptr>& results, sylar::IDB::ptr conn);
    static ArticleCategoryRelInfo::ptr Query( const int64_t& id, sylar::IDB::ptr conn);
    static int QueryByArticleId(std::vector<ArticleCategoryRelInfo::ptr>& results,  const int64_t& article_id, sylar::IDB::ptr conn);
    static ArticleCategoryRelInfo::ptr QueryByArticleIdCategoryId( const int64_t& article_id,  const int64_t& category_id, sylar::IDB::ptr conn);
    static int CreateTableSQLite3(sylar::IDB::ptr info);
    static int CreateTableMySQL(sylar::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //__BLOG_DATAARTICLE_CATEGORY_REL_INFO_H__
