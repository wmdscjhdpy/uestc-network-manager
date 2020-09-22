#define CPPHTTPLIB_ZLIB_SUPPORT
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <time.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <regex>
#include <unordered_map>

#include "httplib.h"
#include "util/crypto.h"
using InfoList = std::vector<std::pair<std::string, std::string> >;

struct UserInfo {
  std::string username;
  std::string password;
  std::string ip;
  std::string acid;
  std::string enc;
  UserInfo(const std::string& username, const std::string& password,
           const std::string& ip, const std::string& acid,
           const std::string& enc)
      : username(username), password(password), ip(ip), acid(acid){};
};

class Client {
 public:
  Client(const std::string& host) : host_(host) {
    client_ =
        std::unique_ptr<httplib::Client>(new httplib::Client(host_.c_str()));
  };
  Client(const Client& client) = delete;
  Client(Client&& client) = delete;
  std::time_t GetTimeStamp() {
    auto tp = std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now());
    std::time_t timestamp = tp.time_since_epoch().count();
    return timestamp;
  }

  bool GetChallenge(UserInfo& user_info, std::string& result) {
    std::string path = "/cgi-bin/get_challenge";
    std::unordered_map<std::string, std::string> params;
    auto timestamp = GetTimeStamp();
    params["callback"] =
        "jQuery11240889396928485537_" + std::to_string(timestamp);
    params["username"] = user_info.username;
    params["_"] = std::to_string(timestamp);
    params["ip"] = user_info.ip;

    std::stringstream ss;
    for (auto p : params) {
      ss << p.first << "=" << p.second << "&";
    }
    path += ("?" + ss.str());
    if (auto res = client_->Get(path.c_str())) {
      if (res->status == 200) {
        std::smatch results;
        if (std::regex_search(res->body, results,
                              std::regex("\"challenge\":\"(.*?)\""))) {
          result = std::move(results[1]);  // the value of challenge
          return true;
        }
      }
    } else {
      auto err = res.error();
    }
    return false;
  }

 private:
  std::string host_;
  std::unique_ptr<httplib::Client> client_;
};

std::string stringfiy(const InfoList& s) {
  std::stringstream ss;
  ss << "{";
  for (int i = 0; i < s.size(); i++) {
    ss << "\"" << s[i].first << "\":\"" << s[i].second << "\"";
    if (i < s.size() - 1) ss << ",";
  }
  ss << "}";
  return ss.str();
}

int main() { return 0; }