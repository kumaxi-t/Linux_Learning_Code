#pragma once
#include "Comm.hpp"
#include "Business.hpp"


class Http {
private:
  std::string _webroot;


public:
  Http(const std::string& webroot = "wwwroot");

  std::string HttpHandler(const std::string& req);

  ~Http();



};