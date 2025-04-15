
void InstallService(PSTR pszServiceName,
    PSTR pszDisplayName,
                    DWORD dwStartType,
    PSTR pszDependencies,
    PSTR pszAccount,
    PSTR pszPassword);



void UninstallService(PSTR pszServiceName);

void yang_start_service(PSTR pszServiceName);