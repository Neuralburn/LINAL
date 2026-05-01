project(
  'linal',
  'c',
  version: '0.2.0',
  default_options: [
    'c_std=c11',
    'warning_level=3',
    'b_lto=true',
  ],
  license: 'MIT',
)

cc = meson.get_compiler('c')

# Optional OpenMP support — parallelization is an optimization, not a requirement.
# Adding -fopenmp enables OpenMP pragmas (ignored gracefully when absent).
if cc.has_argument('-fopenmp')
  openmp_dep = declare_dependency(
    compile_args: ['-fopenmp'],
    link_args: ['-fopenmp'],
    dependencies: [cc.find_library('gomp', required : false)],
  )
else
  message('OpenMP not available — serial fallback for all functions')
  # Empty dependency so downstream code doesn't fail on macOS/clang
  openmp_dep = declare_dependency()
endif

project_description = 'A small linear algebra Meson-based C11 library'

ver = meson.project_version().split('.')
major = ver[0].to_int()
minor = ver[1].to_int()
patch = ver[2].to_int()

conf = configuration_data()
conf.set_quoted('VERSION', meson.project_version())
conf.set('MAJOR', major)
conf.set('MINOR', minor)
conf.set('PATCH', patch)

generated_version_header = configure_file(
  input: 'config' / 'linal_version.h.in',
  output: 'linal_version.h',
  configuration: conf,
  install: true,
  install_dir: get_option('includedir') / 'linal',
)

public_headers = include_directories('include')
generated_headers = include_directories('.')

library_sources = files('src/linal.c')

linal_lib = static_library(
  'linal',
  library_sources,
  include_directories: [public_headers, generated_headers],
  install: true,
  dependencies: openmp_dep,
)

math_lib = cc.find_library('m')

linal_dep = declare_dependency(
  include_directories: [public_headers, generated_headers],
  link_with: linal_lib,
  sources: generated_version_header,
  dependencies: [math_lib, openmp_dep],
)

meson.override_dependency('linal', linal_dep)

pkg = import('pkgconfig')
pkg.generate(
  linal_lib,
  description: project_description,
  filebase: 'linal',
  name: 'LINAL',
  subdirs: 'linal',
)

if get_option('build_tests')
  subdir('tests')
endif
