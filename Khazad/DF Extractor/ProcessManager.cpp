#include "DFCommon.h"

#include <DataModel.h>
#include <MemInfo.h>
///FIXME: what to do with these?
#include <Paths.h>
#include <StringMagic.h>

/// HACK: global variables (only one process can be attached at the same time.)
Process * g_pProcess; ///< current process. non-NULL when picked
ProcessHandle g_ProcessHandle; ///< cache of handle to current process. used for speed reasons

/// TODO: Add something that can destroy the vector of processes and their stuff. Add destructor that calls it.
/// TODO: release ProcessHandles of Windows processes. Scanning for processes over and over could lead to crashed Windows

#ifdef LINUX_BUILD
/// LINUX version of the process finder. lots of headers.
#include <md5.h>
#include <sys/param.h>
#include <sys/user.h>
#include <sys/sysctl.h>
#include <sys/ptrace.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>

Process* ProcessManager::addWineProcess(string & exe,ProcessHandle PH)
{
    string hash = MD5Sum((char *)exe.c_str());// get hash of the running DF process
    // iterate over the list of memory locations
    vector<memory_info>::iterator it;
    for ( it=meminfo.begin() ; it < meminfo.end(); it++ )
        if("windows" == (*it).getString("system")) // is this a linux entry?
            if(hash == (*it).getString("md5")) // are the md5 hashes the same?
            {
                memory_info * m = &*it;
                printf("Found wine process %d. It's DF version %s.\n",PH, m->getString("version").c_str());
                Process *ret= new Process(new DMWindows40d(),m,PH);
                processes.push_back(ret);
                return ret;
            }
    return NULL;
}
Process* ProcessManager::addLinuxProcess(string & exe,ProcessHandle PH)
{
    string hash = MD5Sum((char *)exe.c_str());// get hash of the running DF process
    // iterate over the list of memory locations
    vector<memory_info>::iterator it;
    for ( it=meminfo.begin() ; it < meminfo.end(); it++ )
        if("linux" == (*it).getString("system")) // is this a linux entry?
            if(hash == (*it).getString("md5")) // are the md5 hashes the same?
            {
                memory_info * m = &*it;
                printf("Found linux process %d. It's DF version %s.\n",PH, m->getString("version").c_str());
                Process *ret= new Process(new DMLinux40d(),m,PH);
                processes.push_back(ret);
                return ret;
            }
    return NULL;
}

bool ProcessManager::findProcessess()
{
    DIR *dir_p;
	struct dirent *dir_entry_p;
	string dir_name;
    string exe_link;
    string cwd_link;
    string cmdline_path;
    string cmdline;
	char target_name[1024];									// ??? buffer overrun potential
	int target_result;
	int errorcount;
	int result;

	errorcount=0;
	result=0;
	// Open /proc/ directory
	dir_p = opendir("/proc/");
	// Reading /proc/ entries
	while(NULL != (dir_entry_p = readdir(dir_p)))
	{
	    // Checking for numbered directories
		if (strspn(dir_entry_p->d_name, "0123456789") == strlen(dir_entry_p->d_name))
		{
		    dir_name = "/proc/";
		    dir_name +=  + dir_entry_p->d_name;
		    dir_name += "/";
		    exe_link = dir_name + "exe";
			// Getting the target of the exe ie to which binary it points to
			target_result = readlink(exe_link.c_str(), target_name, sizeof(target_name)-1);
			if (target_result > 0)
			{
			    // make sure we have a null terminated string...
				target_name[target_result] = 0;
				// is this the regular linux DF?
				if (strstr(target_name, "dwarfort.exe") != NULL)
				{
				    exe_link = target_name;
				    // get PID
					result = atoi(dir_entry_p->d_name);
					/// create linux process, add it to the vector
					addLinuxProcess(exe_link,result);
				}
				// a wine process, we need to do more checking and this may break is the future
				else if(strstr(target_name, "wine-preloader")!= NULL)
				{
				    cwd_link = dir_name + "cwd";
				    // get working directory
				    target_result = readlink(cwd_link.c_str(), target_name, sizeof(target_name)-1);
				    target_name[target_result] = 0;
				    // got path to executable, do the same for its name
				    cmdline_path = dir_name + "cmdline";
				    ifstream ifs ( cmdline_path.c_str() , ifstream::in );
				    getline(ifs,cmdline);
				    if (cmdline.find("dwarfort.exe") != string::npos || cmdline.find("Dwarf Fortress.exe") != string::npos)
				    {
				        // put executable name and path together
				        exe_link = target_name;
				        exe_link += "/";
				        exe_link += cmdline;
				        // get PID
				        result = atoi(dir_entry_p->d_name);
				        /// create wine process, add it to the vector
				        addWineProcess(exe_link,result);
				    }
				}
			}
		}
	}
	closedir(dir_p);
	// we have some precesses stored. nice.
	if(processes.size())
        return true;
    return false;
}

#else

bool EnableDebugPriv()
{
   BOOL               bRET = FALSE;
   TOKEN_PRIVILEGES   tp;
   HANDLE             hToken;

   if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))
   {
      if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
      {
         if (hToken != INVALID_HANDLE_VALUE)
         {
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            tp.PrivilegeCount = 1;
            if (AdjustTokenPrivileges(hToken, FALSE, &tp, 0, 0, 0))
               bRET = TRUE;
            CloseHandle(hToken);
         }
      }
   }
   return bRET;
}



/// WINDOWS version of the process finder
bool ProcessManager::findProcessess()
{
    /*
    HWND DFWindow;
    HANDLE DFHandle;
    DWORD DFProcessID;
    // should use "SDL_app" instead of NULL here?
    DFWindow = FindWindow(NULL,"Dwarf Fortress");
    if(!DFWindow)
    {
        return false;
    }
    // And Create Handle
    GetWindowThreadProcessId(DFWindow, &DFProcessID);
    printf("Window Thread Process ID [%u] \n", (unsigned int)DFProcessID);
    ///FIXME: Add corresponding CloseHandle call
    DFHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, DFProcessID);

    if(!DFHandle)
    {
        printf("Could not get application handle to hook. \n");
        return false;
    }
    ///TODO: plug in hexsearch here.
    ///TODO: rewrite for multiple processes. maybe use a library as the APIs have changed between versions of Windows
    ///destroy the process vector after we're done, free resources

    // we need to use memory reading here. assign the global
    g_ProcessHandle = DFHandle;
    int Base = 0x400000;
    // New Method
    int TempOffset = MreadDWord(Base + 60);
    int TimeStamp = MreadDWord(Base + TempOffset + 8);
    vector<memory_info>::iterator it;
    for ( it=meminfo.begin() ; it < meminfo.end(); it++ )
    {
        if("windows" == (*it).getString("system")) // is this a windows entry?
        {
            Uint32 pe_timestamp = (*it).getOffset("pe_timestamp");
            // Old Method
            int OldTimeStamp = MreadDWord((*it).getOffset("pe_offset"));
            if (pe_timestamp == TimeStamp || pe_timestamp == OldTimeStamp)
            {
                printf("Match found! Using version %s.\n", (*it).getString("version").c_str());
                memory_info * m = &*it;
                Process *ret= new Process(new DMWindows40d(),m,DFHandle);
                processes.push_back(ret);
                return true;
            }
        }
    }*/
    /*
    if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
        {
            TCHAR szModName[MAX_PATH];

            // Get the full path to the module's file.

            if ( GetModuleFileNameEx( hProcess, hMods[i], szModName,
                                      sizeof(szModName) / sizeof(TCHAR)))
            {
                // Print the module name and handle value.

                _tprintf( TEXT("\t%s (0x%08X)\n"), szModName, hMods[i] );
            }
        }
    }

    */
    // Get the list of process identifiers.
    ///TODO: make this dynamic. (call first to get the array size and second to really get process handles)
    DWORD ProcArray[512], memoryNeeded, numProccesses;
    HMODULE hmod = NULL;
    DWORD junk;
    HANDLE hProcess;
    bool found = false;

    IMAGE_NT_HEADERS32 pe_header;
	IMAGE_SECTION_HEADER sections[16];

    EnableDebugPriv();
    if ( !EnumProcesses( ProcArray, sizeof(ProcArray), &memoryNeeded ) )
        return false;

    // Calculate how many process identifiers were returned.
    numProccesses = memoryNeeded / sizeof(DWORD);

    // iterate through processes
    for ( int i = 0; i < numProccesses; i++ )
    {
        found = false;
        // open process
        hProcess = OpenProcess( /*PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_VM_WRITE*/ PROCESS_ALL_ACCESS, FALSE, ProcArray[i] );
        if (NULL == hProcess)
            continue;
        // we've got some process, look at its first module
        if(EnumProcessModules(hProcess, &hmod, 1 * sizeof(HMODULE), &junk))
        {
            /// check module filename to verify that it's DF!
            // got base ;)
            uint32_t base = (uint32_t)hmod;
            // read from this process
            g_ProcessHandle = hProcess;
            uint32_t pe_offset = MreadDWord(base+0x3C);
            Mread(base + pe_offset                   , sizeof(pe_header), (uint8_t *)&pe_header);
            Mread(base + pe_offset+ sizeof(pe_header), sizeof(sections) , (uint8_t *)&sections );
            //uint32_t TempOffset = MreadDWord(base + 60);
            //uint32_t TimeStamp = MreadDWord(base + TempOffset + 8);
            vector<memory_info>::iterator it;
            for ( it=meminfo.begin() ; it < meminfo.end(); it++ )
            {
                if("windows" == (*it).getString("system")) // is this a windows entry?
                {
                    Uint32 pe_timestamp = (*it).getOffset("pe_timestamp");
                    if (pe_timestamp == pe_header.FileHeader.TimeDateStamp)
                    {
                        printf("Match found! Using version %s.\n", (*it).getString("version").c_str());
                        // give the process a data model and memory layout fixed to the base of first module
                        memory_info *m = new memory_info(*it);
                        m->setRebase(0x400000,base);
                        Process *ret= new Process(new DMWindows40d(),m,hProcess);
                        processes.push_back(ret);
                        found = true;
                        break; // break the iterator loop
                    }
                }
            }
            // close handle of processes that aren't DF
            if(!found)
            {
                CloseHandle(hProcess);
            }

        }
    }
	if(processes.size())
        return true;
    return false;
}
#endif

/// OS independent part
bool ProcessManager::loadDescriptors()
{
    meminfo.clear();
    Path file = "Assets\\Maps\\memory.ini";
    string line;
    ifstream inFile;
    inFile.open(file);
    if (!inFile)
    {
        cerr << "Unable to open file datafile.txt";
        return false;
    }
    memory_info mem;
    while(std::getline(inFile, line))
    {
        vector<string> tokens; // tokens go here
        // trim crap
        string t1 = trim(line);
        // ignore comments
        if(t1[0] == ';') continue;
        // we got data
        tokenize(t1,tokens,"=");
        if(tokens.size() != 2)
        {
            cerr << "malformed line: " << line << endl;
            continue;
        }
        // assume two tokens
        string ta = trim(tokens[0]);
        string tb = trim(tokens[1]);
        if(ta == "version")// we got a version line, this means a new entry
        {
            if(mem.hasToken("version"))// we have one already
            {
                // push the sucker on the vector
                meminfo.push_back(mem);
            }
            //make a clean new mem
            mem.flush();
            mem.setToken(ta,tb);// set its version
        }
        else
        {
            mem.setToken(ta,tb);// ordinary line with a '=', add
        }
    }
    if(mem.hasToken("version")) // one last entry remaining
        meminfo.push_back(mem); // add it to the vector
    return true;
}

uint32_t ProcessManager::size()
{
    return processes.size();
};
Process * ProcessManager::operator[](uint32_t index)
{
    assert(index < processes.size());
    return processes[index];
};
Process* ProcessManager::getCurrentProcess()
{
    return currentProcess;
};
ProcessHandle ProcessManager::getCurrentProcessHandle()
{
    return currentProcessHandle;
};

ProcessManager::ProcessManager()
{
    currentProcess = NULL;
    currentProcessHandle = 0;
    loadDescriptors();
}
