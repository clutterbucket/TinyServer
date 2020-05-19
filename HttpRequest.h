//
// Created by WTF on 2020/5/4.
//

#ifndef WEBSERVER_HTTPREQUEST_H
#define WEBSERVER_HTTPREQUEST_H

#include <unordered_map>
#include <memory>
using std::string;

class HttpRequest;
typedef std::shared_ptr<HttpRequest> HttpRequestPtr;

enum Method{
    GET,
    HEAD,
    POST
};

enum Version{
    VERSION_1_0,
    VERSION_1_1
};

enum ParsingState{
    PARSE_HEADLINE,
    PARSE_HEADERS,
    PARSE_BODY,
};

class HttpRequest{
    Method method_;
    string path_;
    Version version_;
    std::unordered_map<string, string> headers_;
    string body_;
    int receivedBytes_;
public:
    HttpRequest(): receivedBytes_(0) {}
    void addHeader(string key, string value){
        headers_[key] = value;
    }

    string getHeader(string key){
        auto it = headers_.find(key);
        if(it == headers_.end()){
            return string();
        }
        return it->second;
    }

    Method getMethod(){ return method_; }
    string &getPath(){ return path_; }
    Version getVersion(){ return version_; }
    void setMethod(Method method) { method_ = method; }
    void setPath(const string &path) { path_ = path; }
    void setVersion(Version version) { version_ = version; }
    void setBody(const string &body){ body_ = body; }
    int getReceivedLen() { return receivedBytes_; }
    int receiveBytes(int bytesNum) { receivedBytes_ += bytesNum; }
};

#endif //WEBSERVER_HTTPREQUEST_H
