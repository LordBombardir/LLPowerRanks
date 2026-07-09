add_rules("mode.debug", "mode.release")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")
add_repositories("lordbombardir-repo https://github.com/LordBombardir/xmake-repo.git")

if is_config("target_type", "server") then
    add_requires("levilamina 26.20.0", {configs = {target_type = "server"}})
    add_requires("placeholderapi 26.20.1", {configs = {target_type = "server"}})
    add_requires("playerdb 26.20.1", {configs = {target_type = "server"}})
else
    add_requires("levilamina 26.20.0", {configs = {target_type = "client"}})
    add_requires("placeholderapi 26.20.1", {configs = {target_type = "client"}})
    add_requires("playerdb 26.20.1", {configs = {target_type = "client"}})
end

add_requires("nlohmann_json")
add_requires("levibuildscript")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

option("target_type")
    set_default("server")
    set_showmenu(true)
    set_values("server", "client")
option_end()

target("PowerRanks") -- Change this to your mod name.
    add_rules("@levibuildscript/linkrule")
    add_rules("@levibuildscript/modpacker")

    if is_plat("windows") then
        add_defines("NOMINMAX", "UNICODE")
        set_exceptions("none") -- To avoid conflicts with /EHa.
        add_cxflags( "/EHa", "/utf-8", "/W4", "/w44265", "/w44289", "/w44296", "/w45263", "/w44738", "/w45204")
        add_cxflags(
            "/EHs",
            "-Wno-microsoft-cast",
            "-Wno-invalid-offsetof",
            "-Wno-c++2b-extensions",
            "-Wno-microsoft-include",
            "-Wno-overloaded-virtual",
            "-Wno-ignored-qualifiers",
            "-Wno-missing-field-initializers",
            "-Wno-potentially-evaluated-expression",
            "-Wno-pragma-system-header-outside-header",
            {tools = {"clang_cl"}}
        )
        set_toolchains("clang-cl")
    end

    add_packages("levilamina")
    add_packages("placeholderapi")
    add_packages("playerdb")
    add_packages("nlohmann_json")

    set_languages("c++20")
    if is_windows then
        add_defines("_HAS_CXX23=1")
    else
        add_defines("_LIBCPP_STD_VER=23")
        add_cxxflags("clang::-stdlib=libc++")
    end
    
    add_defines("POWERRANKS_EXPORT")
    set_symbols("debug")
    set_kind("shared")

    add_headerfiles("src/**.h")
    add_files("src/**.cpp")
    add_includedirs("src")

    if is_config("target_type", "server") then
        add_defines("LL_PLAT_S")
    else
        add_defines("LL_PLAT_C")
    end

    after_build(function (target)
        local binDirectory = path.join(os.projectdir(), "bin")
        
        local libDirectory = path.join(binDirectory, "lib")
        local includeDirectory = path.join(path.join(binDirectory, "include"), "power_ranks")
        local typesDirectory = path.join(includeDirectory, "types")

        os.mkdir(libDirectory)
        os.mkdir(includeDirectory)
        os.mkdir(typesDirectory)

        os.cp(path.join(target:targetdir(), "PowerRanks.lib"), libDirectory)
        os.cp(path.join(os.projectdir(), "src", "mod", "core", "Api.h"), includeDirectory)
        os.cp(path.join(os.projectdir(), "src", "mod", "types", "Rank.h"), typesDirectory)
        os.cp(path.join(os.projectdir(), "src", "mod", "types", "HiddenCommandOverloads.h"), typesDirectory)
        os.cp(path.join(os.projectdir(), "assets", "data"), path.join(path.join(binDirectory, target:name()), "data"))
    end)
