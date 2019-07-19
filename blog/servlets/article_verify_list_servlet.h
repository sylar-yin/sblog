#ifndef __BLOG_SERVLETS_ARTICLE_VERIFY_LIST_SERVLET_H__
#define __BLOG_SERVLETS_ARTICLE_VERIFY_LIST_SERVLET_H__

#include "sylar/http/servlet.h"
#include "blog/struct.h"

namespace blog {
namespace servlet {

class ArticleVerifyListServlet : public BlogServlet {
public:
    typedef std::shared_ptr<ArticleVerifyListServlet> ptr;
    ArticleVerifyListServlet();
    virtual int32_t handle(sylar::http::HttpRequest::ptr request
                   ,sylar::http::HttpResponse::ptr response
                   ,sylar::http::HttpSession::ptr session
                   ,Result::ptr result) override;
};

}
}

#endif
