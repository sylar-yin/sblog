#ifndef __BLOG_DATAARTICLE_INFO_H__
#define __BLOG_DATAARTICLE_INFO_H__

#include <json/json.h>
#include <vector>
#include "sylar/db/db.h"
#include "sylar/util.h"


namespace blog {
namespace data {

class ArticleInfoDao;
class ArticleInfo {
friend class ArticleInfoDao;
public:
    typedef std::shared_ptr<ArticleInfo> ptr;

    ArticleInfo();

    const int64_t& getId() { return m_id; }
    void setId(const int64_t& v);

    const int64_t& getUserId() { return m_userId; }
    void setUserId(const int64_t& v);

    const std::string& getTitle() { return m_title; }
    void setTitle(const std::string& v);

    const std::string& getContent() { return m_content; }
    void setContent(const std::string& v);

    const int32_t& getType() { return m_type; }
    void setType(const int32_t& v);

    const int32_t& getState() { return m_state; }
    void setState(const int32_t& v);

    const int64_t& getChannel() { return m_channel; }
    void setChannel(const int64_t& v);

    const int32_t& getIsDeleted() { return m_isDeleted; }
    void setIsDeleted(const int32_t& v);

    const int64_t& getPublishTime() { return m_publishTime; }
    void setPublishTime(const int64_t& v);

    const int64_t& getWeight() { return m_weight; }
    void setWeight(const int64_t& v);

    const int64_t& getViews() { return m_views; }
    void setViews(const int64_t& v);

    const int64_t& getPraise() { return m_praise; }
    void setPraise(const int64_t& v);

    const int64_t& getFavorites() { return m_favorites; }
    void setFavorites(const int64_t& v);

    const int64_t& getCreateTime() { return m_createTime; }
    void setCreateTime(const int64_t& v);

    const int64_t& getUpdateTime() { return m_updateTime; }
    void setUpdateTime(const int64_t& v);

    std::string toJsonString() const;

private:
    int32_t m_type;
    int32_t m_state;
    int32_t m_isDeleted;
    int64_t m_id;
    int64_t m_userId;
    int64_t m_channel;
    int64_t m_weight;
    int64_t m_views;
    int64_t m_praise;
    int64_t m_favorites;
    std::string m_title;
    std::string m_content;
    int64_t m_publishTime;
    int64_t m_createTime;
    int64_t m_updateTime;
};


class ArticleInfoDao {
public:
    typedef std::shared_ptr<ArticleInfoDao> ptr;
    static int Update(ArticleInfo::ptr info, sylar::IDB::ptr conn);
    static int Insert(ArticleInfo::ptr info, sylar::IDB::ptr conn);
    static int InsertOrUpdate(ArticleInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(ArticleInfo::ptr info, sylar::IDB::ptr conn);
    static int Delete(const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteById( const int64_t& id, sylar::IDB::ptr conn);
    static int DeleteByUserId( const int64_t& user_id, sylar::IDB::ptr conn);
    static int QueryAll(std::vector<ArticleInfo::ptr>& results, sylar::IDB::ptr conn);
    static ArticleInfo::ptr Query( const int64_t& id, sylar::IDB::ptr conn);
    static int QueryByUserId(std::vector<ArticleInfo::ptr>& results,  const int64_t& user_id, sylar::IDB::ptr conn);
    static int CreateTableSQLite3(sylar::IDB::ptr info);
    static int CreateTableMySQL(sylar::IDB::ptr info);
};

} //namespace data
} //namespace blog
#endif //__BLOG_DATAARTICLE_INFO_H__
