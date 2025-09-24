function coruvs_global_setup()
  -- General Workspace Settings
  set_project(workspace_name)
  set_version(workspace_version, {build = engine_buildid_pattern})

  set_allowedplats(build_platforms)
  set_defaultplat(build_platforms[1])

  set_allowedarchs(build_architectures)
  set_defaultarchs(build_architectures[1])

  set_allowedmodes(build_configurations)
  set_defaultmode(build_configurations[1])

  set_languages(build_cxx_dialect)

  -- Global Configuration
  set_targetdir('$(projectdir)/Binaries/Engine/$(plat)-$(arch)/$(mode)')
  set_objectdir('$(projectdir)/Intermediates/Engine/$(plat)-$(arch)/$(mode)')

  set_warnings('error')
  set_exceptions('none')

  -- Mode Configuration
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

  -- Platform Configuration
  if is_plat('windows') then
    add_cxflags('/EHsc', '/Zc:preprocessor', '/wd5103', {force = true})
    add_defines('WIN32_LEAN_AND_MEAN', 'NOMINMAX', 'WIN32_DEFAULT_LIBS')
  end
end