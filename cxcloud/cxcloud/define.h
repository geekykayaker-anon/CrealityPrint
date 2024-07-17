#pragma once

#ifndef CXCLOUD_DEFINE_HPP_
#define CXCLOUD_DEFINE_HPP_

#include <QString>

#include "cxcloud/interface.hpp"

namespace cxcloud {

  /// @brief Some http request behaviors will have differences at defferent application.
  ///        Such as recommend model list in model library or newest application version at website.
  enum class ApplicationType : int {
    CREALITY_PRINT = 11,
    HALOT_BOX      = 12,
  };

  enum class InterfaceType : int {
    PRE_RELEASE = 0,
    RELEASE     = 1,
  };

  enum class ServerType : int {
    MAINLAND_CHINA = 0,
    ASIA_PASCIFIC  = 1,
    EUROPE         = 2,
    NORTH_AMERICA  = 3,
    SOUTH_AMERICA  = 4,
    OTHERS         = 5,
  };

  CXCLOUD_API extern const ServerType DEFAULT_SERVER_TYPE;

  enum class RealServerType : int {
    MAINLAND_CHINA = 0,
    INTERNATIONAL  = 1,
  };

  CXCLOUD_API extern const RealServerType DEFAULT_REAL_SERVER_TYPE;

  struct CXCLOUD_API UserBaseInfo {
    QString token{};
    QString user_id{};
  };

  struct CXCLOUD_API UserDetailInfo {
    QString avatar{};
    QString nick_name{};
    double max_storage_space{ 0.0 };
    double used_storage_space{ 0.0 };
  };

}  // namespace cxcloud

#endif  // CXCLOUD_DEFINE_HPP_