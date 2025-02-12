/*************************************************************************
	> File Name: src/zisync/kernel/utils/plain_config.h
	> Author: wansong 
	> Mail: betterofsong@gmail.com 
	> Created Time: Wed 15 Apr 2015 04:09:36 PM CST
 ************************************************************************/
#ifndef ZISYNC_KERNEL_UTILS_PLAIN_CONFIG_H
#define ZISYNC_KERNEL_UTILS_PLAIN_CONFIG_H
#include <string>
#include <vector>
#include <map>
#include "zisync_kernel.h"
#include "zisync/kernel/platform/platform.h"
using std::string;
using std::map;
using std::vector;

namespace zs {
  class PlainConfig {
    public:
      static err_t Initialize(const string &database_dir, const vector<string> &token);
      static bool HasInit();
      static void Clear();

      static std::string plain_db_file();
      static void set_plain_db_file(const string &file);
      static std::string ciphered_db_file();
      static void set_ciphered_db_file(const string &file);

      static err_t GetSqlcipherPassword(string *passphrase);
      static err_t SetSqlcipherToken(const string &mactoken);
      static err_t SetValueForKey(const string &key, const string &val);
      static err_t GetValueForKey(const string &key, string *passphrase);

    private:
      PlainConfig():has_init(false){};
      PlainConfig(const PlainConfig&);
      void operator=(const PlainConfig&);

      map<string, string> dictionary;
      string database_dir_;
      Mutex mutex;
      bool has_init;

      static PlainConfig singleton;
  };
}

#endif
