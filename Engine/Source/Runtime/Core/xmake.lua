local module_name = 'Core'

corvus_engine_target(module_name)
  add_forceincludes(module_name .. '/CoreTypes.hpp', { public = true })

  add_packages('mimalloc')
corvus_target_end()