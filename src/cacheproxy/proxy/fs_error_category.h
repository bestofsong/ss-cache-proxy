//
// Created by wansong on 16/03/2018.
//

#ifndef CACHE_PROXY_FS_ERROR_CATEGORY_H
#define CACHE_PROXY_FS_ERROR_CATEGORY_H


#include <system_error>

namespace smartstudy {

enum class fs_error {
  ok = 0,
  noent,
};


struct fs_error_category : std::error_category {
  const char* name() const noexcept override;
  std::string message(int ev) const override;
};


std::error_code make_error_code(fs_error e);

}


#endif //CACHE_PROXY_FS_ERROR_CATEGORY_H
