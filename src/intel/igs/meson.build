sanitize_data = configuration_data()

igs_defines = []

if get_option('intel-service')
  igs_defines += [ '-DHAVE_IGS' ]

  sanitize_data.set(
    'install_libexecdir',
    join_paths(get_option('prefix'), get_option('libexecdir'))
  )

  install_data(
    'com.intel.GPU.conf',
    install_dir : join_paths(get_option('sysconfdir'), 'dbus-1', 'system.d')
  )

  install_data(
    'com.intel.GPU.policy',
    install_dir : join_paths(get_option('datadir'), 'polkit-1', 'actions')
  )

  install_data(
    'com.intel.GPU.rules',
    install_dir : join_paths(get_option('datadir'), 'polkit-1', 'rules.d')
  )

  install_data(
    'com.intel.GPU.xml',
    install_dir : join_paths(get_option('datadir'), 'dbus-1', 'interfaces')
  )


  configure_file(
    input : 'com.intel.GPU.service.in',
    output : '@BASENAME@',
    install_dir : join_paths(get_option('datadir'), 'dbus-1', 'system-services'),
    configuration : sanitize_data,
    install : true
  )

  configure_file(
    input : 'intel-gpu-service.service.in',
    output : '@BASENAME@',
    install_dir : systemd_unit_dir,
    configuration : sanitize_data,
    install : true
  )

  dep_polkit = dependency('polkit-gobject-1')
  dep_gio_unix = dependency('gio-unix-2.0', version : '>= 2.30')
  dep_libdbus = dependency('dbus-1')

  gdbus_codegen = find_program('gdbus-codegen')

  intel_service_sources = files('main.c',
                                'igs_server.c',
                                'igs_frequency_override.c',
                                'igs_util.c')

  intel_service_sources += custom_target(
    'intel-service-dbus-source',
    input : 'com.intel.GPU.xml',
    output : 'com_intel_gpu.c',
    command : [
      gdbus_codegen,
      '--c-namespace', 'Igs',
      '--c-generate-object-manager',
      '--body', '--output', '@OUTPUT@',
      '@INPUT@' ]
  )

  intel_service_sources += custom_target(
    'intel-service-dbus-header',
    input : 'com.intel.GPU.xml',
    output : 'com_intel_gpu.h',
    command : [
      gdbus_codegen,
      '--c-namespace', 'Igs',
      '--c-generate-object-manager',
      '--header', '--output', '@OUTPUT@',
      '@INPUT@' ]
  )

  intel_service = executable(
    'intel-gpu-service',
    intel_service_sources,
    dependencies : [dep_libdrm, dep_polkit, dep_gio_unix],
    include_directories : [inc_common, inc_drm_uapi, inc_intel],
  c_args : [c_vis_args, no_override_init_args],
  build_by_default : true,
  install : true,
  install_dir : get_option('libexecdir')
  )

  libigs = static_library(
    'igs', files('igs_dbus_client.c'),
    include_directories : [inc_common, inc_intel],
    dependencies : dep_libdbus
  )
else
  libigs = static_library(
    'igs', files('igs_ioctl_client.c'),
    include_directories : [inc_common, inc_intel],
    dependencies : dep_libdrm
  )
endif
