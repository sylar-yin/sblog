#ifndef __BLOG_SERVLETS_COMMENT_VERIFY_LIST_SERVLET_H__
#define __BLOG_SERVLETS_COMMENT_VERIFY_LIST_SERVLET_H__

#include "sylar/http/servlet.h"
#include "blog/struct.h"

namespace blog {
namespace servlet {

class CommentVerifyListServlet : public BlogServlet {
public:
    typedef std::shared_ptr<CommentVerifyListServlet> ptr;
    CommentVerifyListServlet();
    virtual int32_t handle(sylar::http::HttpRequest::ptr request
                   ,sylar::http::HttpResponse::ptr response
                   ,sylar::http::HttpSession::ptr session
                   ,Result::ptr result) override;
};

}
}

#endif
