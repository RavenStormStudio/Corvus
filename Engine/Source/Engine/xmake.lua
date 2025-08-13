local target_name = 'Engine'

corvus_engine_target(target_name)

  add_configfiles('Public/Engine/Config.hpp.in', {
    variables = {
      PROJECT_NAME = project_name
    }
  })

  add_packages('spdlog', { public = true })
corvus_target_end()

-- Tests
corvus_test_target(target_name)

