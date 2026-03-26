test_exe = executable(
  'test_linal',
  files('test_linal.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test('linal module', test_exe)
