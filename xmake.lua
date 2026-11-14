add_requires("raylib", "lua")

add_rules("mode.debug", "mode.release")

target("mykingdown")
    set_kind("binary")
    add_files("src/*.cpp")
    
    add_packages("raylib", "lua")