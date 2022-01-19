#pragma once

#include <iostream>
#include <stdint.h>
#include "memory/Map.hpp"

namespace trafi
{


class Object {

public:
    Object() {
      std::cout << "Object's ..ctor invoked" << std::endl;
    }

    ~Object() {
      std::cout << "Object's ..dector invoked" << std::endl;
    }

    int n = 54;

}; // class Object


class config_parser : memory::Map<const char*, Object>
{
public:
  config_parser() = default;

  config_parser(const char* file_name) :
        file_name(const_cast<char*>(file_name))
        {  };

  inline
  void set_path(const char* file_name)
  { this->file_name = const_cast<char*>(file_name); }



public:
  void parse()
  {
    auto _this = *this;

    _this["server.bind.ip"];
  }

private:

  char* file_name;



}; // class config_parser


void test()
{
  config_parser pars("config.trf");
  pars.parse();
  // std::cout << pars["server.bind.ip"] << " " << pars["server.port"];
}

}; // namespace trafi
