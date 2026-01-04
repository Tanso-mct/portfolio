#include "windows_base/src/pch.h"
#include "windows_base/include/id_factory.h"

size_t wb::IDFactory::CreateComponentID()
{
    static size_t idCounter = 0;

    size_t createdId = idCounter;
    idCounter++;

    return createdId;
}

size_t wb::IDFactory::CreateSystemID()
{
    static size_t idCounter = 0;

    size_t createdId = idCounter;
    idCounter++;

    return createdId;
}

size_t wb::IDFactory::CreateFileLoaderID()
{
    static size_t idCounter = 0;

    size_t createdId = idCounter;
    idCounter++;

    return createdId;
}

size_t wb::IDFactory::CreateAssetID()
{
    static size_t idCounter = 0;

    size_t createdId = idCounter;
    idCounter++;

    return createdId;
}

size_t wb::IDFactory::CreateAssetFactoryID()
{
    static size_t idCounter = 0;

    size_t createdId = idCounter;
    idCounter++;

    return createdId;
}

size_t wb::IDFactory::CreateSceneFacadeID()
{
    static size_t idCounter = 0;

    size_t createdId = idCounter;
    idCounter++;

    return createdId;
}

size_t wb::IDFactory::CreateMonitorID()
{
    static size_t idCounter = 0;

    size_t createdId = idCounter;
    idCounter++;

    return createdId;
}

size_t wb::IDFactory::CreateMonitorFactoryID()
{
    static size_t idCounter = 0;

    size_t createdId = idCounter;
    idCounter++;

    return createdId;
}

size_t wb::IDFactory::CreateWindowID()
{
    static size_t idCounter = 0;

    size_t createdId = idCounter;
    idCounter++;

    return createdId;
}

size_t wb::IDFactory::CreateSharedFacadeID()
{
    static size_t idCounter = 0;

    size_t createdId = idCounter;
    idCounter++;

    return createdId;
}