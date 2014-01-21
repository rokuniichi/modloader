/*
 * Copyright (C) 2013  LINK/2012 <dma_2012@hotmail.com>
 * Licensed under GNU GPL v3, see LICENSE at top level directory.
 * 
 *  std-text -- Standard GTA Text Loader Plugin for San Andreas Mod Loader
 *      This plugin overrides gxt files
 *
 */
#include <modloader.hpp>
#include <modloader_util.hpp>
#include <modloader_util_path.hpp>
#include <modloader_util_injector.hpp>
using namespace modloader;

/*
 *  The plugin object
 */
class CThePlugin *textPlugin;
class CThePlugin : public modloader::CPlugin
{
    public:
        const char* GetName();
        const char* GetAuthor();
        const char* GetVersion();
        bool CheckFile(modloader::ModLoaderFile& file);
        bool ProcessFile(const modloader::ModLoaderFile& file);
        bool PosProcess();
        
        const char** GetExtensionTable();
        
        // Overrider object
        struct GxtOverrider
        {
            const char* original;   // The original filename (e.g. "american.gxt")
            std::string replace;    // The replacement file (fullpath)
            
            // Setup the structure
            void Setup(const char* original)
            {
                this->original = original;
            }
            
            // Register a replacement
            bool Replace(const char* withfile)
            {
                return RegisterReplacementFile(*textPlugin, original, replace, withfile);
            }
            
            // Gets the replacement file for original (based on game root directory)
            // If no replacement has been registered, returns "text\@original"
            const char* GetReplacement()
            {
                if(replace.empty())
                {
                    replace = std::string("text\\") + original;
                }
                return replace.c_str();
            }
            
        };
        
        //
        GxtOverrider gxt[5];
        
        CThePlugin()
        {
            // Setup gxt overriders
            gxt[0].Setup("american.gxt");
            gxt[1].Setup("spanish.gxt");
            gxt[2].Setup("german.gxt");
            gxt[3].Setup("italian.gxt");
            gxt[4].Setup("french.gxt");
        }
        
        /* Finds a gxt overrider based on the original filename */
        GxtOverrider* GetOverrider(const char* filename)
        {
            for(int i = 0; i < 5; ++i)
            {
                auto& x = gxt[i];
                if(!strcmp(x.original, filename, false))
                    return &x;
            }
            return nullptr;
        }

} plugin;



/*
 *  Export plugin object data
 */
extern "C" __declspec(dllexport)
void GetPluginData(modloader_plugin_t* data)
{
    textPlugin = &plugin;
    modloader::RegisterPluginData(plugin, data);
}



/*
 *  Basic plugin informations
 */
const char* CThePlugin::GetName()
{
    return "std-text";
}

const char* CThePlugin::GetAuthor()
{
    return "LINK/2012";
}

const char* CThePlugin::GetVersion()
{
    return "RC1";
}

const char** CThePlugin::GetExtensionTable()
{
    static const char* table[] = { "gxt", 0 };
    return table;
}

/*
 *  Check if the file is the one we're looking for
 */
bool CThePlugin::CheckFile(modloader::ModLoaderFile& file)
{
    if(!file.is_dir && IsFileExtension(file.filext, "gxt"))
        return true;
    return false;
}

/*
 * Process the replacement
 */
bool CThePlugin::ProcessFile(const modloader::ModLoaderFile& file)
{
    if(auto* gxt = GetOverrider(file.filename))
    {
        if(gxt->Replace(GetFilePath(file).c_str()))
            return true;
    }
    return false;
}


/*
 * Called after all files have been processed
 * Hook things up
 */
bool CThePlugin::PosProcess()
{
    typedef int(*fopen_type)(const char*, const char*);
    typedef int(*hook_type)(fopen_type fopen, const char*& file, const char*& mode);
    
    // chdir into the root directory instead of "text" folder
    WriteMemory<const char*>(0x69FCE1 + 1, "", true);
    WriteMemory<const char*>(0x6A01BE + 1, "", true);
    
    // Hook that replaces the fopen that opens GXT files (there's two calls)
    // Remember, the filename received is based on "text" folder but we are on the root folder!
    hook_type hook = [](fopen_type fopen, const char*& filename, const char*& mode)
    {
        if(auto* gxt = textPlugin->GetOverrider(filename))
        {
            // Found overrider!
            return fopen(gxt->GetReplacement(), mode);
        }
        else
        {
            // Nope, open filename on text folder
            std::string buf = std::string("text\\") + filename;
            return fopen(buf.data(), mode);
        }
    };
    
    // Hook the gxt fopen's
    make_function_hook<0x6A0228, int(const char*, const char*)>(hook);
    make_function_hook<0x69FD5A, int(const char*, const char*)>(hook);

    return true;
}