/*
 * Copyright (C) 2013  LINK/2012 <dma_2012@hotmail.com>
 * Licensed under GNU GPL v3, see LICENSE at top level directory.
 * 
 *
 */
#ifndef DATA_H
#define	DATA_H

#include "traits.hpp"
#include <map>
#include <list>
#include <modloader.hpp>
#include <modloader_util_path.hpp>
using namespace modloader;
using namespace data;           /* from traits.hpp */



/*
 *  The plugin object
 */
extern class CThePlugin* dataPlugin;
class CThePlugin : public modloader::CPlugin
{
    public:
        unsigned int BuiltDataFilesCount;
        std::string cachePath;
        
        static const int default_priority = 50;
        
        CThePlugin() : BuiltDataFilesCount(0)
        {}
        
        const char* GetName();
        const char* GetAuthor();
        const char* GetVersion();
        int OnStartup();
        int OnShutdown();
        int CheckFile(const modloader::ModLoaderFile& file);
        int ProcessFile(const modloader::ModLoaderFile& file);
        int PosProcess();
        
        const char** GetExtensionTable();
};





/*
 *  This [filesystem + stack + something] object stores data about a fake filesystem
 *  This is a very simple object, not something complex as you'd expect from a filesystem
 *  Let's call it a container...
 */
template<class DataTraitsType>  /* DataTraitsType must be of DataTraitsBase */
class CDataFS
{
    public:
        typedef std::list<DataTraitsType> TraitsList;
        
        /* TODO hash NormalizeFSPath */
        
        /* map< FSPath, ListOfPhysicalPaths > */
        std::map< std::string, TraitsList > files;
        
        
    public:
        /* Returns an pointer to the list of files assigned with @path or nullptr if no file is assigned to it */
        TraitsList* GetList(const char* path)
        {
            auto it = files.find(NormalizePath(path));
            if(it == files.end() || it->second.empty())
                return nullptr;
            return &it->second;
        }
        
        /* Returns an pointer to the list of files assigned with @path,
         * if no file is assigned to @path returns the empty list */
        TraitsList* GetListAlways(const char* path)
        {
            return &files[NormalizePath(path)];
        }
        
        /* Adds a file to the container, it's assigned path will be @fsPath and the physical path on the computer is @physicalPath 
         * It will be pushed in the front of the list and marked as default if @isDefault is true
         */
        DataTraitsType& AddFile(const char* fsPath, const char* physicalPath, bool isDefault)
        {
            DataTraitsType* p;
            auto& l = *GetListAlways(fsPath);
            
            /* Get space in the list */
            if(isDefault)
            {
                l.push_front( DataTraitsType() );
                p = &l.front();
            }
            else
            {
                p = &AddNewItemToContainer(l);
            }
            
            /* Assign information and return */
            p->path = physicalPath;
            p->isDefault = isDefault;
            return *p;
        }
        
        /* Adds a default file (that's, original, from the base game folder) to the container */
        DataTraitsType& AddFile(const char* filepath)
        {
            return AddFile(filepath, filepath, true);
        }
        
        /* Adds a custom file to the container */
        DataTraitsType& AddFile(const ModLoaderFile& file)
        {
            return AddFile(file.filepath, GetFilePath(file).c_str(), false);
        }

        /* Returns the cache file (used to finally send data to the game) from the assigned @path name
         * (e.g. "DATA/MAPS/VEGAS/VEGASS.IPL" will return something like "$CACHE_PATH/data_10/VEGASS.IPL" */
        std::string GetCacheFileFor(const char* path)
        {
            char dataPath[MAX_PATH];
            
            /* Create path to store the cache file,
             * it's like that because the original filename (last @path component) must be preserved */
            sprintf(dataPath, "%sdata_%d\\", dataPlugin->cachePath.c_str(), ++dataPlugin->BuiltDataFilesCount);
            MakeSureDirectoryExistA(dataPath);
            
            /* Append the cache path with the file name */
            return std::string(dataPath) + &path[GetLastPathComponent(path)];
        }
        
};


/* All data traits we're going to use are inside this structure */
struct CAllTraits
{
    CDataFS<TraitsIDE>      ide;
    CDataFS<TraitsIPL>      ipl;
    CDataFS<TraitsGTA>      gta;
};
extern CAllTraits traits;


#endif	/* DATA_H */