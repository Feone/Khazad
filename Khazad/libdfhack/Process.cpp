#include "DFCommon.h"
#ifdef LINUX_BUILD
#include <sys/wait.h>
#endif

ProcessManager::Process::Process(DataModel * dm, memory_info* mi, ProcessHandle ph)
{
    my_datamodel = dm;
    my_descriptor = mi;
    my_handle = ph;
    attached = false;
}

// destroy data model. this is assigned by processmanager
ProcessManager::Process::~Process()
{
    if(attached)
        detach();
    delete my_datamodel;
    freeResources();
}


DataModel *ProcessManager::Process::getDataModel()
{
    return my_datamodel;
}
memory_info * ProcessManager::Process::getDescriptor()
{
    return my_descriptor;
}

bool ProcessManager::Process::isAttached()
{
    ///TODO: check for weird states here - like crashed DF and similar crap
    return attached; // valid when attached
}

#ifdef LINUX_BUILD
/**
 *     LINUX PART
 */

bool ProcessManager::Process::attach()
{
    /// TODO: check for errors!
    if(g_pProcess != NULL)
        return false;
    ptrace(PTRACE_ATTACH , my_handle, NULL, NULL);
    wait(NULL); // wait for DF to be stopped.
    attached = true;
    /// !!!HACK!!!
    /// Set the global process variables
    g_pProcess = this;
    g_ProcessHandle = my_handle;
    return true; // we are attached
}
bool ProcessManager::Process::detach()
{
    /// TODO: check for errors.
    ptrace(PTRACE_DETACH, my_handle, NULL, NULL);
    attached = false;
    g_pProcess = NULL;
    g_ProcessHandle = 0;
    return true;
}

void ProcessManager::Process::freeResources()
{
    // nil
};

#else
/**
 *     WINDOWS PART
 */
bool ProcessManager::Process::attach()
{
    attached = true;
    g_pProcess = this;
    g_ProcessHandle = my_handle;
    return true;
}
bool ProcessManager::Process::detach()
{
    attached = false;
    g_pProcess = NULL;
    g_ProcessHandle = 0;
    // nothing to do here, we are not a debbuger on Windows
    return true;
}
void ProcessManager::Process::freeResources()
{
    // opened by process manager
    CloseHandle(my_handle);
};
#endif
