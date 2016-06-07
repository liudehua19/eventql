/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *   - Laura Schlimmer <laura@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include <eventql/config/process_config.h>
#include <eventql/util/io/fileutil.h>
#include <inih/ini.h>

namespace eventql {

struct IniParserState {
  IniParserState(
      ProcessConfigBuilder* _cfg_builder) :
      cfg_builder(_cfg_builder),
      status(Status::success()) {}
  ProcessConfigBuilder* cfg_builder;
  Status status;
};

ProcessConfig::ProcessConfig(
      HashMap<String, String> properties) :
      properties_(properties) {}

Option<String> ProcessConfig::getString(
    const String& section,
    const String& key) const {
  return getString(StringUtil::format("$0.$1", section, key));
}

Option<String> ProcessConfig::getString(const String& key) const {
  auto p = properties_.find(key);
  if (p != properties_.end()) {
    return Some(p->second);
  }

  return None<String>();
}

Option<int64_t> ProcessConfig::getInt(
    const String& section,
    const String& key) const {
  return getInt(StringUtil::format("$0.$1", section, key));
}

Option<int64_t> ProcessConfig::getInt(const String& key) const {
  auto p = properties_.find(key);
  if (p != properties_.end() && StringUtil::isNumber(p->second)) {
    try {
      auto value = std::stoi(p->second);
      return Some<int64_t>(value);

    } catch (std::exception e) {
      /* fallthrough */
    }
  }

  return None<int64_t>();
}

static int ini_parse_handler(
    void* user,
    const char* section,
    const char* name,
    const char* value) {
  auto parser_state = (IniParserState*) user;
  parser_state->cfg_builder->setProperty(section, name, value);
  return 1;
}

Status ProcessConfigBuilder::loadFile(const String& file) {
  IniParserState parser_state(this);
  if (ini_parse(file.c_str(), &ini_parse_handler, &parser_state) < 0) {
    parser_state.status = Status(eParseError, "invalid config file");
  }

  return parser_state.status;
}

Status ProcessConfigBuilder::loadDefaultConfigFile() {
  char* homedir = getenv("HOME");
  if (!homedir) {
    return Status::success();
  }

  String confg_file_path = FileUtil::joinPaths(homedir, ".evqlrc");
  if (!FileUtil::exists(confg_file_path)) {
    return Status::success();
  }

  return loadFile(confg_file_path);
}

void ProcessConfigBuilder::setProperty(const String& key, const String& value) {
  properties_[key] = value;
}

void ProcessConfigBuilder::setProperty(
    const String& section,
    const String& key,
    const String& value) {
  setProperty(StringUtil::format("$0.$1", section, key), value);
}

RefPtr<ProcessConfig> ProcessConfigBuilder::getConfig() {
  return mkRef(new ProcessConfig(properties_));
}


} // namespace eventql



