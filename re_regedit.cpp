#include "re_regedit.h"

HKEY RegEdit::extractKeyHandle(Key key) {
  switch (key) {
  case Key::KEY_CLASSES_ROOT:
    return HKEY_CLASSES_ROOT;
  case Key::KEY_CURRENT_CONFIG:
    return HKEY_CURRENT_CONFIG;
  case Key::KEY_CURRENT_USER:
    return HKEY_CURRENT_USER;
  case Key::KEY_LOCAL_MACHINE:
    return HKEY_LOCAL_MACHINE;
  case Key::KEY_USERS:
    return HKEY_USERS;
  default:
    throw std::system_error{ERROR_INVALID_HANDLE, std::system_category(),
                            "The specified key handle was not found"};
  }
}

RegEdit::RegEdit(Key key, const std::string &catalog) {
  this->createCatalog(key, catalog);
}

RegEdit::~RegEdit() { this->closeCatalog(); }

void RegEdit::createCatalog(Key key, const std::string &catalog) {
  if (isOpened_)
    return;

  HKEY keyHandle{this->extractKeyHandle(key)};

  auto result{RegCreateKeyExA(keyHandle, catalog.c_str(), 0, nullptr,
                              REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr,
                              &keyHandle_, nullptr)};
  if (result != ERROR_SUCCESS) {
    throw std::system_error{result, std::system_category()};
  }

  isOpened_ = true;
}

const std::string RegEdit::getString(const std::string &key) {
  std::string value{};

  this->queryValue<std::string>(
      key, value, [&](const QueriedValue &queriedValue) {
        if (queriedValue.type != REG_SZ) {
          throw std::system_error{ERROR_DATATYPE_MISMATCH,
                                  std::system_category(),
                                  "Readed data type mismatch"};
        }

        value.resize(queriedValue.size);
        return false;
      });

  return value;
}

const unsigned long RegEdit::getUlong(const std::string &key) {
  unsigned long value{};

  this->queryValue<unsigned long>(
      key, value, [&](const QueriedValue &queriedValue) {
        if (queriedValue.type != REG_DWORD) {
          throw std::system_error{ERROR_DATATYPE_MISMATCH,
                                  std::system_category(),
                                  "Readed data type mismatch"};
        }
        return false;
      });

  return value;
}

void RegEdit::setString(const std::string &key, const std::string &value) {
  const LPBYTE valueConv{(const LPBYTE)value.c_str()};
  const DWORD size{static_cast<DWORD>(value.length())};

  auto result{
      RegSetValueExA(keyHandle_, key.c_str(), 0, REG_SZ, valueConv, size)};
  if (result != ERROR_SUCCESS) {
    throw std::system_error{result, std::system_category()};
  }
}

void RegEdit::setUlong(const std::string &key, const unsigned long value) {
  const LPBYTE valueConv{(const LPBYTE)&value};
  const DWORD size{sizeof(unsigned long)};

  auto result{
      RegSetValueExA(keyHandle_, key.c_str(), 0, REG_DWORD, valueConv, size)};
  if (result != ERROR_SUCCESS) {
    throw std::system_error{result, std::system_category()};
  }
}

void RegEdit::removeValue(const std::string &key) {
  auto result{RegDeleteValueA(keyHandle_, key.c_str())};
  if (result != ERROR_SUCCESS) {
    throw std::system_error{result, std::system_category()};
  }
}

void RegEdit::removeCatalog(Key key, const std::string &catalog) {
  HKEY keyHandle{this->extractKeyHandle(key)};

  auto result{RegDeleteKeyExA(keyHandle, catalog.c_str(), KEY_WOW64_32KEY, 0)};
  if (result != ERROR_SUCCESS) {
    throw std::system_error{result, std::system_category()};
  }
}

void RegEdit::closeCatalog() {
  if (!isOpened_)
    return;

  auto result{RegCloseKey(keyHandle_)};
  if (result != ERROR_SUCCESS) {
    throw std::system_error{result, std::system_category()};
  }

  isOpened_ = false;
}