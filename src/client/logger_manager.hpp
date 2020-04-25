#pragma once
#include <cstring>
#include <ctime>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

class LoggerManager {
public:
  static void setup() {
#ifdef DEBUG
#define LOG_FILEPATH_PREFIX "debug/client_log_"
    spdlog::set_level(spdlog::level::debug);
#else
#define LOG_FILEPATH_PREFIX "client_log_"
    spdlog::set_level(spdlog::level::info);
#endif

    try {
      char filename[100];
      strcpy(filename, LOG_FILEPATH_PREFIX);
      int prefix_len = strlen(filename);
      std::time_t t = std::time(nullptr);
      std::strftime(filename + prefix_len, sizeof(filename) - prefix_len,
                    "%H:%M:%S", std::localtime(&t));
      strcat(filename, ".txt");
      auto file_logger = spdlog::basic_logger_mt("flog", filename);
      spdlog::register_logger(file_logger);
    } catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log init failed: " << ex.what() << std::endl;
    }
  }
};
