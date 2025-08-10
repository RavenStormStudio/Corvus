set_xmakever('3.0.1')

set_project('Corvus')
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
  set_symbols('hidden')
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
add_cxflags('/EHsc', '/MP', {force = true})

function corvus_engine_target(name)
  target(name)
    if is_mode('debug') then
      set_kind('shared')
    elseif is_mode('development') then
      set_kind('shared')
    elseif is_mode('shipping') then
      set_kind('static')
    end

    add_files('./Private/**.cpp')
    add_headerfiles('./Public/**.hpp')
    add_includedirs('./Private')
    add_includedirs('./Public', { public = true })
    add_forceincludes(name .. '/' .. name .. 'API.hpp', { public = true })

    add_defines('CORVUS_BUILD_' .. string.upper(name))
end

function corvus_launch_target(name)
  target(name)
    set_kind('binary')
    set_default(true)

    add_files('./Private/**.cpp')
    add_headerfiles('./Public/**.hpp')
    add_includedirs('./Private', './Public')
end

function corvus_target_end()
  target_end()
end

includes('**/xmake.lua')