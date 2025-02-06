add_rules("mode.debug", "mode.release")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")
add_repositories("lordbombardir-repo https://github.com/LordBombardir/xmake-repo.git")

-- add_requires("levilamina x.x.x") for a specific version
-- add_requires("levilamina develop") to use develop version
-- please note that you should add bdslibrary yourself if using dev version
add_requires("levilamina 1.0.1")
add_requires("translatorapi 1.0.0")
add_requires("sqlitecpp")
add_requires("levibuildscript")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

target("PowerRanks") -- Change this to your mod name.
    add_rules("@levibuildscript/linkrule")
    add_rules("@levibuildscript/modpacker")
    
    add_cxflags(
        "/EHa",
        "/utf-8",
        "/W4",
        "/w44265",
        "/w44289",
        "/w44296",
        "/w45263",
        "/w44738",
        "/w45204"
    )
    add_defines("NOMINMAX", "UNICODE", "_HAS_CXX23=1", "POWERRANKS_EXPORT")
    add_files("src/**.cpp")
    add_includedirs("src")
    add_packages("levilamina")
    add_packages("translatorapi")
    add_packages("sqlitecpp")
    set_exceptions("none") -- To avoid conflicts with /EHa.
    set_kind("shared")
    set_languages("c++20")
    set_symbols("debug")

    after_build(function (target)
        local binDirectory = path.join(os.projectdir(), "bin")
        
        local libDirectory = path.join(binDirectory, "lib")
        local includeDirectory = path.join(binDirectory, "include")
        local objectDirectory = path.join(includeDirectory, "object")

        os.mkdir(libDirectory)
        os.mkdir(includeDirectory)
        os.mkdir(objectDirectory)

        os.cp(path.join(target:targetdir(), "PowerRanks.lib"), libDirectory)
        os.cp(path.join(os.projectdir(), "src", "mod", "LLPowerRanks.h"), includeDirectory)
        os.cp(path.join(os.projectdir(), "src", "mod", "object", "Rank.h"), objectDirectory)
        os.cp(path.join(os.projectdir(), "assets", "data"), path.join(path.join(binDirectory, target:name()), "data"))
    end)
