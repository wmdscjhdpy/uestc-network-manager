// Copyright (c) 2020 The UESTC-Networkmanager Authors. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Authors: ehds(ds.he@foxmail.com)
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <iostream>
#include <regex>
#include <unordered_map>

#include "httplib.h"
#include "util/config.h"
#include "util/crypto.h"
namespace uestc {
using InfoList = std::vector<std::pair<std::string, std::string> >;
std::string kDomain;
constexpr int N = 200;
constexpr int TYPE = 1;

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

struct UserInfo {
  std::string username;
  std::string password;
  std::string ip;
  std::string acid;
  std::string enc_ver;
  UserInfo(const std::string& username, const std::string& password,
           const std::string& ip, const std::string& acid,
           const std::string& enc_ver)
      : username(username + kDomain),
        password(password),
        ip(ip),
        acid(acid),
        enc_ver(enc_ver){};

  std::string dump() const {
    InfoList user = {{"username", username},
                     {"password", password},
                     {"ip", ip},
                     {"acid", acid},
                     {"enc_ver", enc_ver}};
    return stringfiy(user);
  }
};

class Client {
 public:
  Client(const std::string& host) : host_(host) {
    client_ =
        std::unique_ptr<httplib::Client>(new httplib::Client(host_.c_str()));
  };
  Client(const Client& client) = delete;
  Client(Client&& client) = delete;
  int64_t GetTimeStamp() {
    auto tp = std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now());
    return tp.time_since_epoch().count();
  }

  bool GetChallenge(UserInfo& user_info, std::string& result) {
    std::string path = "/cgi-bin/get_challenge";
    std::unordered_map<std::string, std::string> params;
    auto timestamp = GetTimeStamp();
    params["callback"] =
        "jQuery112409591102916896419_" + std::to_string(timestamp);
    params["username"] = user_info.username;
    params["_"] = std::to_string(timestamp);
    params["ip"] = user_info.ip;

    std::stringstream ss;
    for (auto p : params) {
      ss << p.first << "=" << p.second << "&";
    }
    path += ("?" + ss.str());
    std::cout<<"[DEBUG] Now will try "<<path<<std::endl;
    if (auto res = client_->Get(path.c_str())) {
      if (res->status == 200) {
        std::smatch results;
        if (std::regex_search(res->body, results,
                              std::regex("\"challenge\":\"(.*?)\""))) {
          result = std::move(results[1]);  // the value of challenge
          std::cout<<"[DEBUG] res status------------------------"<<std::endl<<res->body<<std::endl<<"[DEBUG] res status------------------------"<<std::endl;
          return true;
        }
      }
    } else {
      auto err = res.error();
      std::cout<<"[ERROR] Get failed. Error code is "<<err<<std::endl;
    }
    return false;
  }

  bool IsAccessInternet() {
      httplib::Client client("www.baidu.com");
      client.set_follow_location(true);
      client.set_connection_timeout(0, 300000);
      if (auto res = client.Get("/")) {
        if (res->status == 200 || res->status == 301) {
          // we can access one of the hosts indicated that we connect to the internet
          return true;
        }
      }
    std::cout << "[INFO] can not access network" << std::endl;
    return false;
  }

  std::string CheckSum(const std::string& data) { return uestc::Sha1(data); }
  std::string Info(const UserInfo& u, const std::string& token) {
    return "{SRBX1}" + uestc::Base64(uestc::XEncode(u.dump(), token));
  }
  bool AuthNetwork(UserInfo& u) {
    if (IsAccessInternet()) {
      std::cout << "[INFO] You have authorized this network, not need to login"<< std::endl;
      //If you want to debug you can comment next line
      return true;
    }
    std::string challenge;
    auto ret = GetChallenge(u, challenge);
    if (!ret) {
      std::cout << "[ERROR] get challenge failed" << std::endl;
      return false;
    }

    auto i = Info(u, challenge);
    std::string hmd5 = uestc::HmacMD5(u.password, challenge);
    std::string check_str = challenge + u.username;
    check_str += challenge + hmd5;
    check_str += challenge + u.acid;
    check_str += challenge + u.ip;
    check_str += challenge + std::to_string(N);
    check_str += challenge + std::to_string(TYPE);
    check_str += challenge + i;

    // mock os
    InfoList os = {{"device", "Linux"}, {"platform", "Linux"}};

    httplib::Params params{{"action", "login"},
                           {"username", u.username},
                           {"password", "{MD5}" + hmd5},
                           {"ac_id", u.acid},
                           {"ip", u.ip},
                           {"chksum", CheckSum(check_str)},
                           {"info", i},
                           {"n", std::to_string(N)},
                           {"type", std::to_string(TYPE)},
                           {"os", os[0].second},
                           {"name", os[1].second},
                           {"double_stack", "0"},
                           {"_", std::to_string(GetTimeStamp())}};
    for (auto item : params) {
      std::cout << item.first << ":" << item.second << std::endl;
    }
    if (auto res = client_->Post("/cgi-bin/srun_portal", params)) {
      if (res->status == 200 && IsAccessInternet()) {
        std::cout << "Login status:" << res->body << std::endl;
        return true;
      } else {
        std::cout << res->status << res->body << std::endl;
      }
    } else {
      std::cout << (char*)res.error() << std::endl;
    }
    return false;
  }

 private:
  std::string host_;
  std::unique_ptr<httplib::Client> client_;

};  // Class Client
}  // namespace uestc

int main() {
  uestc::Config config{};
  uestc::kDomain=config.get("kDomain");
  std::cout<<"[DEBUG] Will use web "<<config.get("web")<<" to login"<<std::endl;
  uestc::Client client(config.get("web"));
  uestc::UserInfo user(config.get("username"), config.get("password"), "", config.get("ac_id"),
                       config.get("kEncVer"));
  int interval = std::atoi(config.get("interval").c_str());
  if (interval <= 0) {
    interval = 5;
  };
  while (true) {
    client.AuthNetwork(user);
    usleep(interval * 1000000);
  }
  return 0;
}
