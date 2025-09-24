set_xmakever('3.0.3')

-- Global Properties
engine_name = 'Corvus'
engine_author = 'RavenStorm'
engine_version = '0.0.1'
engine_buildid_pattern = '%Y%m%d%H%M'

-- Build Properties
build_platforms = {
  'windows',
}

build_architectures = {
  'x64',
}

build_configurations = {
  'debug',
  'development',
  'shipping',
}

build_cxx_dialect = 'cxx20'

-- Dependencies
add_requires('mimalloc 3.1.5')

-- Includes
includes('Engine/Build/options.lua')
includes('Engine/Build/rules.lua')
includes('Engine/Build/targets.lua')
includes('Engine/Build/workspace.lua')

coruvs_global_setup()

-- Preconfigured Build Files Target
target('BuildFiles')
  set_group('Tools')
  set_kind('phony')
  
  -- Files
  add_extrafiles('$(projectdir)/xmake.lua')
  add_extrafiles('$(projectdir)/Engine/Build/*.lua')
  add_extrafiles('$(projectdir)/**/xmake.lua')
target_end()

-- Rest of targets
includes('**/xmake.lua')