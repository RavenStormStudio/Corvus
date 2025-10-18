local module_name = 'Launch'

corvus_engine_target(module_name)
  add_deps('Core', { public = true })
  add_deps('Platform', { public = true })
corvus_target_end()
