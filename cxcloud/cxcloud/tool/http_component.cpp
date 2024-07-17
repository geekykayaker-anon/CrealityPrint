#include "cxcloud/tool/http_component.h"

#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QSettings>
#include <QtCore/QUuid>
#include <QtNetwork/QNetworkInterface>

#include <cpr/cpr.h>

#include <qtusercore/module/systemutil.h>
#include <qtusercore/util/settings.h>

namespace cxcloud {

  namespace {

    enum class OsType : int {
      UNKNOWN,
      WINDOWS,
      LINUX,
      MACOS,
    };

    constexpr inline auto GetOsType() -> OsType {
#ifdef Q_OS_WINDOWS
      return OsType::WINDOWS;
#endif

#ifdef Q_OS_LINUX
      return OsType::LINUX;
#endif

#ifdef Q_OS_MACOS
      return OsType::MACOS;
#endif

      return OsType::UNKNOWN;
    };

    inline auto GetMacAddress() -> QString {
      for (const auto& network_interface : QNetworkInterface::allInterfaces()) {
        const auto flags = network_interface.flags();
        if (flags.testFlag(QNetworkInterface::InterfaceFlag::IsUp) &&
            flags.testFlag(QNetworkInterface::InterfaceFlag::IsRunning) &&
            !flags.testFlag(QNetworkInterface::InterfaceFlag::IsLoopBack)) {
            if(network_interface.hardwareAddress()!="")
                return network_interface.hardwareAddress();
        }
      }

      return {};
    }

    inline auto CreateUuid() -> QString {
      return QUuid::createUuid().toString();
    }

    inline auto I18nToLangaugeIndex(const QString& i18n) -> QString {
      static const auto I18N_INDEX_MAP = std::map<QString, QString>{
        { QStringLiteral("en")   , QStringLiteral("0")  },
        { QStringLiteral("en_us"), QStringLiteral("0")  },
        { QStringLiteral("en_uk"), QStringLiteral("0")  },

        { QStringLiteral("zh")   , QStringLiteral("1")  },
        { QStringLiteral("zh_cn"), QStringLiteral("1")  },
        { QStringLiteral("zh_tw"), QStringLiteral("2")  },
        { QStringLiteral("zh_hk"), QStringLiteral("3")  },

        { QStringLiteral("ko")   , QStringLiteral("5")  },
        { QStringLiteral("ko_kr"), QStringLiteral("5")  },

        { QStringLiteral("ja")   , QStringLiteral("10") },
        { QStringLiteral("ja_jp"), QStringLiteral("10") },

        { QStringLiteral("pt")   , QStringLiteral("11") },
        { QStringLiteral("pt_pt"), QStringLiteral("11") },
        { QStringLiteral("pt_br"), QStringLiteral("15") },

        { QStringLiteral("ru")   , QStringLiteral("4")  },
        { QStringLiteral("xa")   , QStringLiteral("6")  },
        { QStringLiteral("es")   , QStringLiteral("7")  },
        { QStringLiteral("de")   , QStringLiteral("8")  },
        { QStringLiteral("fr")   , QStringLiteral("9")  },
        { QStringLiteral("th")   , QStringLiteral("12") },
        { QStringLiteral("nl")   , QStringLiteral("13") },
        { QStringLiteral("it")   , QStringLiteral("14") },
        { QStringLiteral("tr")   , QStringLiteral("16") },
        { QStringLiteral("ro")   , QStringLiteral("17") },
        { QStringLiteral("he")   , QStringLiteral("18") },
        { QStringLiteral("po")   , QStringLiteral("19") },
        { QStringLiteral("in")   , QStringLiteral("20") },
        { QStringLiteral("hu")   , QStringLiteral("21") },
      };

      auto iter = I18N_INDEX_MAP.find(i18n);
      if (iter != I18N_INDEX_MAP.cend()) {
        return iter->second;
      }

      for (const auto& [key, value] : I18N_INDEX_MAP) {
        if (key.contains(i18n)) {
          return value;
        }
      }

      for (const auto& [key, value] : I18N_INDEX_MAP) {
        if (i18n.contains(key)) {
          return value;
        }
      }

      return QStringLiteral("0");
    }

    inline auto GetLanguageIndex() -> QString {
      auto setting = qtuser_core::VersionSettings{};
      setting.beginGroup(QStringLiteral("language_perfer_config"));
      const auto i18n = setting.value(QStringLiteral("language_type"), QStringLiteral("en"))
                               .toString()
                               .toLower()
                               .remove(QStringLiteral(".ts"));
      setting.endGroup();
      return I18nToLangaugeIndex(i18n);
    }

    inline auto ErrorCodeHttpToCpr(HttpErrorCode error_code) -> cpr::ErrorCode {
      return static_cast<cpr::ErrorCode>(static_cast<int>(error_code));
    }

    inline auto ErrorCodeCprToHttp(cpr::ErrorCode error_code) -> HttpErrorCode {
      return static_cast<HttpErrorCode>(static_cast<int>(error_code));
    }

    inline auto ParametersHttpToCpr(const HttpParameters& parameters) -> cpr::Parameters {
      auto cpr_parameters = cpr::Parameters{};
      for (const auto& [key, value] : parameters) {
        cpr_parameters.Add({ key.toStdString(), value.toStdString() });
      }
      return cpr_parameters;
    }

    inline auto HeaderHttpToCpr(const HttpHeader& header) -> cpr::Header {
      auto cpr_header = cpr::Header{};
      for (const auto& [key, value] : header) {
        cpr_header.emplace(std::make_pair(key.toStdString(), value.toStdString()));
      }
      return cpr_header;
    }

    inline auto UrlEncode(const QString& url) -> QString {
      // return cpr::util::urlEncode(url.toStdString());
      // return url.toUtf8().toPercentEncoding(QByteArrayLiteral(":/?="));
      return QString{ url }.replace(QStringLiteral(" "), QStringLiteral("%20"));
    }

    inline auto UrlDecode(const QString& url) -> QString {
      // return cpr::util::urlEncode(url.toStdString());
      // return QString::fromUtf8(QByteArray::fromPercentEncoding(url.toUtf8()));
      return QString{ url }.replace(QStringLiteral("%20"), QStringLiteral(" "));
    };

  }  // namespace





  HttpRequest::HttpRequest(QObject* parent) : QObject{ parent } {}

  auto HttpRequest::isInitlized() -> bool {
    return !getUrl().isEmpty();
  }

  auto HttpRequest::getRequestData() const -> QByteArray {
    return QByteArrayLiteral("{}");
  }

  auto HttpRequest::getDownloadSrcUrl() const -> QString {
    return QStringLiteral("");
  }

  auto HttpRequest::getDownloadDstPath() const -> QString {
    return QStringLiteral("");
  }

  auto HttpRequest::getApplicationVersion() const -> Version {
    return application_version_;
  }

  auto HttpRequest::getApplicationType() const -> ApplicationType {
    return application_type_;
  }

  auto HttpRequest::getAccountUid() const -> QString {
    return account_uid_;
  }

  auto HttpRequest::getAccountToken() const -> QString {
    return account_token_;
  }

  auto HttpRequest::getHeader() const -> HttpHeader {
    if (!header_.empty()) {
      return header_;
    }

    header_ = {
      { QStringLiteral("Content-Type"), QStringLiteral("application/json") },
      { QStringLiteral("__CXY_BRAND_"), QStringLiteral("creality") },
      { QStringLiteral("__CXY_PLATFORM_"),
          QString::number(static_cast<int>(getApplicationType())) },
      { QStringLiteral("__CXY_DUID_"), GetMacAddress() },
      { QStringLiteral("__CXY_OS_VER_"), []() {
          if constexpr (GetOsType() == OsType::WINDOWS) {
            return QStringLiteral("win");
          } else if constexpr (GetOsType() == OsType::LINUX) {
            return QStringLiteral("linux");
          } else if constexpr (GetOsType() == OsType::MACOS) {
            return QStringLiteral("macos");
          } else {
            return QStringLiteral("unknown");
          }
        }() },
      { QStringLiteral("__CXY_OS_LANG_"), GetLanguageIndex() },
      { QStringLiteral("__CXY_APP_ID_"), QStringLiteral("creality_model") },
      { QStringLiteral("__CXY_APP_CH_"), QStringLiteral("creality") },
      { QStringLiteral("__CXY_APP_VER_"), getApplicationVersion().toString() },
      { QStringLiteral("__CXY_TIMEZONE_"),
          QString::number(QDateTime::currentDateTime().offsetFromUtc()) },
      { QStringLiteral("__CXY_REQUESTID_"), CreateUuid() },
    };

    if (auto token = getAccountToken(); !token.isEmpty()) {
      header_.emplace(std::make_pair(QStringLiteral("__CXY_TOKEN_"), std::move(token)));
    }

    if (auto uid = getAccountUid(); !uid.isEmpty()) {
      header_.emplace(std::make_pair(QStringLiteral("__CXY_UID_"), std::move(uid)));
    }

    return header_;
  }

  auto HttpRequest::getParameters() const -> HttpParameters {
    return {};
  }

  auto HttpRequest::getUrlHead() const -> QString {
    return url_head_;
  }

  auto HttpRequest::getUrlTail() const -> QString {
    return {};
  }

  auto HttpRequest::getUrl() const -> QString {
    return QStringLiteral("%1%2").arg(getUrlHead(), getUrlTail());
  }

  auto HttpRequest::getTryTimes() const -> size_t {
    return try_times_;
  }

  auto HttpRequest::setTryTimes(size_t times) -> void {
    try_times_ = times;
  }

  auto HttpRequest::callWhenSuccessed() -> void {}

  auto HttpRequest::callWhenFailed() -> void {}

  auto HttpRequest::callWhenErred() -> void {}

  auto HttpRequest::callWhenProgressUpdated() -> void {}

  auto HttpRequest::syncGet() -> void {
    if (!isInitlized()) {
      return;
    }

    auto session = cpr::Session{};
    session.SetUrl(UrlEncode(getUrl()).toStdString());
    session.UpdateHeader(HeaderHttpToCpr(getHeader()));
    session.SetParameters(ParametersHttpToCpr(getParameters()));

    const auto response = [this, &session] {
      for (auto try_times = 0; try_times < getTryTimes(); ++try_times) {
        const auto response = session.Get();
        if (response.error.code == cpr::ErrorCode::OK) {
          return response;
        }
      }

      return cpr::Response{};
    }();

    setResponseState(ErrorCodeCprToHttp(response.error.code));
    setResponseMessage(QString::fromStdString(response.error.message));
    setResponseData(response.text.c_str());
    callWhenResponsed();
    callAfterResponsed();
  }

  auto HttpRequest::asyncGet() -> void {
    if (!isInitlized()) {
      return;
    }

    future_ = std::async(std::launch::async, [this]() {
      syncGet();
    });
  }

  auto HttpRequest::asyncGet(ThreadPool& pool) -> void {
    if (!isInitlized()) {
      return;
    }

    future_ = pool.execute([this]() {
      syncGet();
    });
  }

  auto HttpRequest::syncPost() -> void {
    if (!isInitlized()) {
      return;
    }

    auto session = cpr::Session{};
    session.SetUrl(UrlEncode(getUrl()).toStdString());
    session.UpdateHeader(HeaderHttpToCpr(getHeader()));
    session.SetBody(getRequestData().toStdString());

    const auto response = [this, &session] {
      for (auto try_times = 0; try_times < getTryTimes(); ++try_times) {
        const auto response = session.Post();
        if (response.error.code == cpr::ErrorCode::OK) {
          return response;
        }
      }

      return cpr::Response{};
    }();

    setResponseState(ErrorCodeCprToHttp(response.error.code));
    setResponseMessage(QString::fromStdString(response.error.message));
    setResponseData(response.text.c_str());
    callWhenResponsed();
    callAfterResponsed();
  }

  auto HttpRequest::asyncPost() -> void {
    if (!isInitlized()) {
      return;
    }

    future_ = std::async(std::launch::async, [this]() {
      syncPost();
    });
  }

  auto HttpRequest::asyncPost(ThreadPool& pool) -> void {
    if (!isInitlized()) {
      return;
    }

    future_ = pool.execute([this]() {
      syncPost();
    });
  }

  auto HttpRequest::syncDownload() -> void {
    if (!isInitlized()) {
      return;
    }

    const auto download_url = getDownloadSrcUrl();
    const auto download_path = getDownloadDstPath();
    if (download_url.isEmpty() || download_path.isEmpty()) {
      return;
    }

    mkMutiDir(QFileInfo{ download_path }.absolutePath());

    qDebug().noquote()
      << QStringLiteral("---------- cxcloud::HttpRequest::syncDownload ----------\n")
      << QStringLiteral("url: %1\n").arg(download_url)
      << QStringLiteral("file: %1").arg(download_path);

    auto session = cpr::Session{};
    session.SetVerifySsl(false);
    session.SetUrl(UrlEncode(download_url).toStdString());
    session.SetProgressCallback({ [self = QPointer<HttpRequest>{ this }]
                                  (cpr::cpr_off_t download_total,
                                   cpr::cpr_off_t download_current,
                                   cpr::cpr_off_t upload_total,
                                   cpr::cpr_off_t upload_current,
                                   intptr_t userdata) {
      // @see https://docs.libcpr.org/advanced-usage.html#download-file
      // Return `true` on success, or `false` to **cancel** the transfer.
      if (!self || self->isCancelDownloadLater()) {
        return false;
      }

      self->setTotalProgress(download_total);
      self->setCurrentProgress(download_current);
      self->callWhenProgressUpdated();
      self->callAfterProgressUpdated();
      return true;
    } });

    const auto response = [this, &session, &download_path] {
      for (size_t try_times = 0; try_times < getTryTimes(); ++try_times) {
        const auto response = session.Download(download_path.toStdString());
        if (response.error.code == cpr::ErrorCode::OK) {
          return response;
        }
      }

      return cpr::Response{};
    }();

    setResponseState(ErrorCodeCprToHttp(response.error.code));
    setResponseMessage(QString::fromStdString(response.error.message));
    setResponseData(QJsonDocument{ QJsonObject{
      { QStringLiteral("code"), QString::number(response.error ? 1 : 0) }
    } }.toJson(QJsonDocument::JsonFormat::Compact));
    callWhenResponsed();
    callAfterResponsed();
  }

  auto HttpRequest::asyncDownload() -> void {
    if (!isInitlized()) {
      return;
    }

    future_ = std::async(std::launch::async, [this]() {
      syncDownload();
    });
  }

  auto HttpRequest::asyncDownload(ThreadPool& pool) -> void {
    if (!isInitlized()) {
      return;
    }

    future_ = pool.execute([this]() {
      syncDownload();
    });
  }

  auto HttpRequest::isAsyncTasking() const -> bool {
    using namespace std::chrono;
    return future_.valid() && future_.wait_for(0s) != std::future_status::ready;
  }

  auto HttpRequest::isAutoDelete() const -> bool {
    return true;
  }

  auto HttpRequest::handleResponseData() -> bool {
    auto successed = true;
    auto message = QString{};
    auto json = QJsonObject{};

    auto error = QJsonParseError{};
    const auto document = QJsonDocument::fromJson(getResponseData(), &error);
    successed = error.error == QJsonParseError::ParseError::NoError;

    if (!successed) {
      message = error.errorString();

    } else {
      json = document.object();
      successed = json[QStringLiteral("code")].toInt() == 0;

      if (!successed) {
        message = json[QStringLiteral("msg")].toString();
      }
    }

    setResponseJson(json);
    setResponseMessage(message);
    return successed;
  }

  auto HttpRequest::callWhenResponsed() -> void {
    const auto handle_with_state = [this](bool successed) {
      QMetaObject::invokeMethod(this, [this, successed]() {
        successed ? callWhenSuccessed() : callWhenFailed();
        successed ? callAfterSuccessed() : callAfterFailed();
        if (isAutoDelete()) { deleteLater(); }
      }, Qt::ConnectionType::AutoConnection);
    };

    if (response_state_ != HttpErrorCode::OK) {
      handle_with_state(false);
      return;
    }

    if (!handleResponseData()) {
      handle_with_state(false);
      return;
    }

    handle_with_state(true);
  }

  auto HttpRequest::getTotalProgress() const -> size_t {
    return total_progress_;
  }

  auto HttpRequest::getCurrentProgress() const -> size_t {
    return current_progress_;
  }

  auto HttpRequest::isCancelDownloadLater() const -> bool {
    return cancel_download_later_.load();
  }

  auto HttpRequest::setCancelDownloadLater(bool cancel) -> void {
    cancel_download_later_.store(cancel);
  }

  auto HttpRequest::getResponseData() const -> const QByteArray& {
    return response_data_;
  }

  auto HttpRequest::getResponseJson() const -> const QJsonObject& {
    return response_json_;
  }

  auto HttpRequest::getResponseState() const -> HttpErrorCode {
    return response_state_;
  }

  auto HttpRequest::getResponseMessage() const -> const QString& {
    return response_message_;
  }

  auto HttpRequest::setTotalProgress(size_t progress) -> void {
    total_progress_ = progress;
  }

  auto HttpRequest::setCurrentProgress(size_t progress) -> void {
    current_progress_ = progress;
  }

  auto HttpRequest::setResponseData(const QByteArray& data) -> void {
    response_data_ = data;
  }

  auto HttpRequest::setResponseJson(const QJsonObject& json) -> void {
    response_json_ = json;
  }

  auto HttpRequest::setResponseState(const HttpErrorCode& state) -> void {
    response_state_ = state;
  }

  auto HttpRequest::setResponseMessage(const QString& message) -> void {
    response_message_ = message;
  }

  auto HttpRequest::setResponsedCallback(std::function<void(void)> callback) -> void {
    after_responsed_callback_ = callback;
  }

  auto HttpRequest::setSuccessedCallback(std::function<void(void)> callback) -> void {
    after_successed_callback_ = callback;
  }

  auto HttpRequest::setFailedCallback(std::function<void(void)> callback) -> void {
    after_failed_callback_ = callback;
  }

  auto HttpRequest::setErredCallback(std::function<void(void)> callback) -> void {
    after_erred_callback_ = callback;
  }

  auto HttpRequest::setProgressUpdatedCallback(std::function<void(void)> callback) -> void {
    after_progress_updated_callback_ = callback;
  }

  auto HttpRequest::callAfterResponsed() -> void {
    if (after_responsed_callback_) {
      after_responsed_callback_();
    }
  }

  auto HttpRequest::callAfterSuccessed() -> void {
    if (after_successed_callback_) {
      after_successed_callback_();
    }
  }

  auto HttpRequest::callAfterFailed() -> void {
    if (after_failed_callback_) {
      after_failed_callback_();
    }
  }

  auto HttpRequest::callAfterErred() -> void {
    if (after_erred_callback_) {
      after_erred_callback_();
    }
  }

  auto HttpRequest::callAfterProgressUpdated() -> void {
    if (after_progress_updated_callback_) {
      after_progress_updated_callback_();
    }
  }





  HttpSession::HttpSession(Version application_version,
                           std::function<QString(void)> url_head_getter,
                           std::function<QString(void)> account_uid_getter,
                           std::function<QString(void)> account_token_getter,
                           QObject* parent)
      : QObject{ parent }
      , application_version_{ std::move(application_version) }
      , url_head_getter_{ url_head_getter }
      , account_uid_getter_{ account_uid_getter }
      , account_token_getter_{ account_token_getter } {}

  auto HttpSession::getApplicationVersion() const -> Version {
    return application_version_;
  }

  auto HttpSession::setApplicationVersion(const Version& version) -> void {
    application_version_ = version;
  }

  auto HttpSession::getApplicationType() const -> ApplicationType {
    return application_type_;
  }

  auto HttpSession::setApplicationType(ApplicationType type) -> void {
    application_type_ = type;
  }

  auto HttpSession::getUrlHeadGetter() const -> std::function<QString(void)> {
    return url_head_getter_;
  }

  auto HttpSession::setUrlHeadGetter(std::function<QString(void)> getter) -> void {
    url_head_getter_ = getter;
  }

  auto HttpSession::getAccountUidGetter() const -> std::function<QString(void)> {
    return account_uid_getter_;
  }

  auto HttpSession::setAccountUidGetter(std::function<QString(void)> getter) -> void {
    account_uid_getter_ = getter;
  }

  auto HttpSession::getAccountTokenGetter() const -> std::function<QString(void)> {
    return account_token_getter_;
  }

  auto HttpSession::setAccountTokenGetter(std::function<QString(void)> getter) -> void {
    account_token_getter_ = getter;
  }

  auto HttpSession::addRequest(QPointer<HttpRequest> request) -> void {
    if (!request) {
      return;
    }

    request->application_version_ = application_version_;
    request->application_type_ = application_type_;
    request->url_head_ = url_head_getter_ ? url_head_getter_() : QString{};
    request->account_uid_ = account_uid_getter_ ? account_uid_getter_() : QString{};
    request->account_token_ = account_token_getter_ ? account_token_getter_() : QString{};
  }

}  // namespace cxcloud
