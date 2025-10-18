local module_name = 'Platform'

corvus_engine_target(module_name)
  add_deps('Core')
corvus_target_end()

-- Tests
-- corvus_test_target(module_name)
--   add_deps('Platform')
-- corvus_target_end()

