local module_name = 'Core'

corvus_engine_target(module_name)
  add_forceincludes(module_name .. '/CoreTypes.hpp', { public = true })

  add_defines('SPDLOG_WCHAR_FILENAMES')
  add_defines('SPDLOG_WCHAR_TO_UTF8_SUPPORT')
  add_defines('SPDLOG_ACTIVE_LEVEL=0')

  add_packages('mimalloc')
  add_packages('spdlog')
corvus_target_end()

-- Tests
corvus_test_target(module_name)
  add_deps('Core')
corvus_target_end()

