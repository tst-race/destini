#
# Dummy for building flickcurl:


cmake_minimum_required(VERSION 3.20)
add_custom_command(OUTPUT "/usr/local/lib/libflickcurl.so",
		   COMMAND "make install")
