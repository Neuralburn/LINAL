# Test executables for each module
test_mat_create = executable(
  'test_mat_create',
  files('test_mat_create.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_mat_memory = executable(
  'test_mat_memory',
  files('test_mat_memory.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_mat_arithmetic = executable(
  'test_mat_arithmetic',
  files('test_mat_arithmetic.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_mat_mul = executable(
  'test_mat_mul',
  files('test_mat_mul.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_mat_scale = executable(
  'test_mat_scale',
  files('test_mat_scale.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_mat_transpose = executable(
  'test_mat_transpose',
  files('test_mat_transpose.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_mat_getset = executable(
  'test_mat_getset',
  files('test_mat_getset.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_mat_print = executable(
  'test_mat_print',
  files('test_mat_print.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_mat_norm = executable(
  'test_mat_norm',
  files('test_mat_norm.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_mat_trace = executable(
  'test_mat_trace',
  files('test_mat_trace.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_mat_dot = executable(
  'test_mat_dot',
  files('test_mat_dot.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_mat_det = executable(
  'test_mat_det',
  files('test_mat_det.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_mat_inv = executable(
  'test_mat_inv',
  files('test_mat_inv.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_mat_copy = executable(
  'test_mat_copy',
  files('test_mat_copy.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_stress = executable(
  'test_stress',
  files('test_stress.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_property = executable(
  'test_property',
  files('test_property.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_workloads = executable(
  'test_workloads',
  files('test_workloads.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test_numerics = executable(
  'test_numerics',
  files('test_numerics.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

# Benchmarks
benchmark_mat_transpose = executable(
  'benchmark_mat_transpose',
  files('benchmark_mat_transpose.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

benchmark_vec_dot = executable(
  'benchmark_vec_dot',
  files('benchmark_vec_dot.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

benchmark_vec_norm_l2 = executable(
  'benchmark_vec_norm_l2',
  files('benchmark_vec_norm_l2.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

benchmark_vec_add = executable(
  'benchmark_vec_add',
  files('benchmark_vec_add.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

benchmark_vec_sub = executable(
  'benchmark_vec_sub',
  files('benchmark_vec_sub.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

# Run all tests
test('mat_create', test_mat_create)
test('mat_memory', test_mat_memory)
test('mat_arithmetic', test_mat_arithmetic)
test('mat_mul', test_mat_mul)
test('mat_scale', test_mat_scale)
test('mat_transpose', test_mat_transpose)
test('mat_getset', test_mat_getset)
test('mat_print', test_mat_print)
test('mat_norm', test_mat_norm)
test('mat_trace', test_mat_trace)
test('mat_dot', test_mat_dot)
test('mat_det', test_mat_det)
test('mat_inv', test_mat_inv)
test('mat_copy', test_mat_copy)
test('stress', test_stress)
test('property', test_property)
test('workloads', test_workloads, timeout: 120)
test('numerics', test_numerics)

test_vec_create = executable(
  'test_vec_create',
  files('test_vec_create.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)
test_vec_add = executable(
  'test_vec_add',
  files('test_vec_add.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)
test_vec_sub = executable(
  'test_vec_sub',
  files('test_vec_sub.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)
test_vec_dot = executable(
  'test_vec_dot',
  files('test_vec_dot.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)
test_vec_scale = executable(
  'test_vec_scale',
  files('test_vec_scale.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)
test_vec_norm = executable(
  'test_vec_norm',
  files('test_vec_norm.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)

test('vec_create', test_vec_create)
test('vec_add', test_vec_add)
test('vec_sub', test_vec_sub)
test('vec_dot', test_vec_dot)
test('vec_scale', test_vec_scale)
test('vec_norm', test_vec_norm)
test_vec_access = executable(
  'test_vec_access',
  files('test_vec_access.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)
test_vec_copy = executable(
  'test_vec_copy',
  files('test_vec_copy.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)
test_vec_print = executable(
  'test_vec_print',
  files('test_vec_print.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)
test('vec_access', test_vec_access)
test('vec_copy', test_vec_copy)
test('vec_print', test_vec_print)
test_vec_geom = executable(
  'test_vec_geom',
  files('test_vec_geom.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)
test('vec_geom', test_vec_geom)
test_vec_ml = executable(
  'test_vec_ml',
  files('test_vec_ml.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)
test('vec_ml', test_vec_ml)
test_vec_property = executable(
  'test_vec_property',
  files('test_vec_property.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)
test('vec_property', test_vec_property)
test_vec_stress = executable(
  'test_vec_stress',
  files('test_vec_stress.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)
test('vec_stress', test_vec_stress)
test_vec_workloads = executable(
  'test_vec_workloads',
  files('test_vec_workloads.c'),
  dependencies: [linal_dep],
  include_directories: [public_headers, generated_headers],
)
test('vec_workloads', test_vec_workloads, timeout: 120)
