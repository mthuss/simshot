project('simshot', 'cpp', default_options: ['cpp_std=c++17'])
gtkdeps = [dependency('gtkmm-4.0')]
src_files = ['main.cc', 'screenshot.cc']
executable('simshot', src_files, dependencies: gtkdeps, install: true)
