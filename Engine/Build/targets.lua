function corvus_base_target(name)
  target(name)
end

function corvus_engine_target(name)
  corvus_base_target(name)
    set_group('Engine')

    if is_mode('debug') then
      set_kind('shared')
    elseif is_mode('development') then
      set_kind('shared')
    elseif is_mode('shipping') then
      set_kind('static')
    end

    if is_plat('windows') then
      add_rules('corvus.windows.default')
    end

    -- Source Files
    add_files('./Private/**.cpp')
    add_headerfiles('./Public/**.hpp', './Public/**.hpp.in')

    -- Includes
    add_includedirs('./Private/')
    add_includedirs('./Public/', { public = true })

    -- Definitions
    add_defines('CORVUS_BUILD_' .. string.upper(name))

    -- Misc
    add_forceincludes(name .. 'API.hpp', { public = true })
end

function corvus_application_target(name)
  corvus_base_target(name)
    set_group('Applications')
    set_kind('binary')
    set_default(true)

    if is_plat('windows') then
      if is_mode('debug') then
        add_ldflags('/SUBSYSTEM:CONSOLE', { force = true })
      elseif is_mode('development') then
        add_ldflags('/SUBSYSTEM:CONSOLE', { force = true })
      elseif is_mode('shipping') then
        add_ldflags('/SUBSYSTEM:WINDOWS', { force = true })
      end

      add_rules('corvus.windows.default')
    end

    -- Source Files
    add_files('./Private/**.cpp')
    add_headerfiles('./Public/**.hpp', './Public/**.hpp.in')

    -- Includes
    add_includedirs('./Private/')
    add_includedirs('./Public/', { public = true })
end

function corvus_program_target(name)
  corvus_base_target(name)
    set_group('Programs')
    set_kind('binary')
    set_default(true)

    if is_plat('windows') then
      if is_mode('debug') then
        add_ldflags('/SUBSYSTEM:CONSOLE', { force = true })
      elseif is_mode('development') then
        add_ldflags('/SUBSYSTEM:CONSOLE', { force = true })
      elseif is_mode('shipping') then
        add_ldflags('/SUBSYSTEM:WINDOWS', { force = true })
      end

      add_rules('corvus.windows.default')
    end

    -- Source Files
    add_files('./Private/**.cpp')
    add_headerfiles('./Public/**.hpp', './Public/**.hpp.in')

    -- Includes
    add_includedirs('./Private/')
    add_includedirs('./Public/')
end

function corvus_test_target(name)
  corvus_base_target(name .. 'Tests')
    set_group('Tests')
    set_kind('binary')
    
    if is_plat('windows') then
      add_ldflags('/SUBSYSTEM:CONSOLE', { force = true })
      add_rules('corvus.windows.default')
    end

    add_files('./Tests/**.cpp')
    add_packages('catch2')
end

function corvus_target_end()
  target_end()
end