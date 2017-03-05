--
-- Copyright (c) 2017 Jani Niemelä.
-- License: https://github.com/siquel/irkki/blob/master/LICENSE
--

newoption {
    trigger = "enable-ssl",
    description = "Enable OpenSSL"
}

solution "irkki"
    configurations {
        "Debug",
        "Release"
    }
    
    platforms {
        -- "x32", for now only x64 
        "x64"
    }
    
    language "C++"
    
IRKKI_DIR = path.getabsolute("..")
local IRKKI_BUILD_DIR = path.join(IRKKI_DIR, ".build")
local IRKKI_THIRD_PARTY_DIR = path.join(IRKKI_DIR, "3rdparty")

dofile("toolchain.lua")
if not toolchain(IRKKI_BUILD_DIR, IRKKI_THIRD_PARTY_DIR) then
    return
end

if _OPTIONS["enable-ssl"] then
    if os.is("windows") then
        if not os.getenv("OPENSSL_DIR") then
            print("Set OPENSSL_DIR env var")
        end
    end
end

if os.is("windows") then
    project "ircclient"
        language "C"
        kind "StaticLib"
        
        files {
            path.join(IRKKI_THIRD_PARTY_DIR, "libircclient/src/libircclient.c"),
            path.join(IRKKI_THIRD_PARTY_DIR, "libircclient/src/**.h")
        }
        
        includedirs {
            path.join(IRKKI_THIRD_PARTY_DIR, "libircclient/include"),
            "$(OPENSSL_DIR)/include"
        }
        
        defines {
            "_WINSOCK_DEPRECATED_NO_WARNINGS",
            "ENABLE_THREADS"
        }
        
        if _OPTIONS["enable-ssl"] then
            defines { "ENABLE_SSL" }
        end
end

project "irkki"
    
    kind "ConsoleApp"
    
    files {
        path.join(IRKKI_DIR, "src/**.cpp"),
        path.join(IRKKI_DIR, "src/**.h"),
    }
    
    configuration "vs*" 
        includedirs {
            path.join(IRKKI_THIRD_PARTY_DIR, "libircclient/include")
        }
        
        libdirs {
            "$(OPENSSL_DIR)/lib"
        }
        
        if _OPTIONS["enable-ssl"] then
            links {
                "ssleay32",
                "libeay32"
            }
        end
        
        links {
            "ws2_32"
        }
    
    
    configuration {}
    
    links {
        "ircclient"
    }
   