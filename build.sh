x86_64-w64-mingw32-gcc -o not_load_window.exe not_load_window.c -lws2_32
gcc plugin_convert.c -o plugin_convert -lz -lcrypto -w
gcc online_array_out_of_bound.c -o online_array_out_of_bound -lz
gcc fake_online.c -o fake_online -lssl -lcrypto -lz -w
