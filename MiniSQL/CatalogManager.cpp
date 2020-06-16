#include "CatalogManager.h"

CatalogManager* CatalogManager::getInstance()
{
    static CatalogManager ctm;
    return &ctm;
}
