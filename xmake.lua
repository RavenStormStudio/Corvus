set_xmakever('3.0.1')

-- Dependencies
add_requires('catch2 3.9.0')
add_requires('spdlog 1.15.3', { configs = { std_format = true } })

-- Project Setup

project_name = 'Corvus'
set_project(project_name)
set_version('0.0.1', {build = '%Y%m%d%H%M'})

set_allowedplats('windows')
set_defaultplat('windows')

set_allowedarchs('x64')
set_defaultarchs('x64')

set_allowedmodes('debug', 'development', 'shipping')

set_languages('cxx20')

if is_mode('debug') then
  set_symbols('debug')
  set_optimize('none')
  set_runtimes('MDd')
  add_defines('CORVUS_BUILD_MODULAR')
  add_defines('CORVUS_MODE_DEBUG', 'CORVUS_MODE="Debug"')
elseif is_mode('development') then
  set_symbols('debug')
  set_optimize('fastest')
  set_strip('all')
  set_runtimes('MDd')
  add_defines('CORVUS_BUILD_MODULAR')
  add_defines('CORVUS_MODE_DEVELOPMENT', 'CORVUS_MODE="Development"')
elseif is_mode('shipping') then
  set_symbols('debug')
  set_optimize('aggressive')
  set_strip('all')
  set_runtimes('MD')
  add_defines('CORVUS_BUILD_MONOLITHIC')
  add_defines('CORVUS_MODE_SHIPPING', 'CORVUS_MODE="Shipping"')
end

set_targetdir('./Binaries/Engine/$(plat)-$(arch)/$(mode)')
set_objectdir('./Intermediates/Engine/$(plat)-$(arch)/$(mode)')

set_warnings('error')
set_exceptions('none')
add_cxflags('/EHsc', '/Zc:preprocessor', '/wd5103', {force = true})
add_defines('WIN32_LEAN_AND_MEAN', 'NOMINMAX', 'WIN32_DEFAULT_LIBS')

-- Custom Rules

rule('windows.default')
  after_load(function (target)
    target:add('syslinks', 'kernel32', 'user32', 'gdi32', 'winspool', 'comdlg32', 'advapi32')
    target:add('syslinks', 'shell32', 'ole32', 'oleaut32', 'uuid', 'odbc32', 'odbccp32', 'comctl32')
    target:add('syslinks', 'comdlg32', 'setupapi', 'shlwapi')
    if not target:is_plat('mingw') then
      target:add('syslinks', 'strsafe')
    end
  end)

function corvus_engine_target(name)
  local config_dir = '$(builddir)/Config/' .. name;

  target(name)
    set_group('Engine')
    set_configdir(config_dir)

    if is_mode('debug') then
      set_kind('shared')
    elseif is_mode('development') then
      set_kind('shared')
    elseif is_mode('shipping') then
      set_kind('static')
    end

    add_rules('windows.default')
    add_files('./Private/**.cpp')
    add_headerfiles('./Public/**.hpp', './Public/**.hpp.in')
    add_includedirs('./Private')
    add_includedirs('./Public', { public = true })
    add_includedirs(config_dir, { public = true })
    add_forceincludes(name .. '/' .. name .. 'API.hpp', { public = true })

    add_defines('CORVUS_BUILD_' .. string.upper(name))
end

function corvus_launch_target(name)
  target(name)
    set_group('Engine')
    set_kind('binary')
    set_default(true)

    if is_mode('debug') then
      add_ldflags('/SUBSYSTEM:CONSOLE', { force = true })
    elseif is_mode('development') then
      add_ldflags('/SUBSYSTEM:CONSOLE', { force = true })
    elseif is_mode('shipping') then
      add_ldflags('/SUBSYSTEM:WINDOWS', { force = true })
    end

    add_rules('windows.default')
    add_files('./Private/**.cpp')
    add_headerfiles('./Public/**.hpp')
    add_includedirs('./Private', './Public')
end

function corvus_test_target(target_name) 
  target(target_name .. 'Tests')
    set_group('Tests')
    set_kind('binary')

    add_ldflags('/SUBSYSTEM:CONSOLE', { force = true })

    add_rules('windows.default')
    add_files('./Tests/**.cpp')
    add_deps(target_name, 'CorvusTestSuit')
    add_packages('catch2')
end

function corvus_target_end()
  target_end()
end

target('BuildFiles')
  set_group('Tools')
  set_kind('phony')
  add_extrafiles('xmake.lua')
  add_extrafiles('**/xmake.lua')
target_end()

includes('**/xmake.lua')