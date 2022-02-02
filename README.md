# mlwx486
reversed mini rootkit\
the launcher:\
a mini launcher that copies the driver image to system32\
from it's resource section, and installs the driver as a service, then starts it\
the driver:\
hooks calls to NtQueryDirectoryFile through ssdt, adjusting the returned data to skip specific files\
apparently this function is used under the hood by Find...FileA functions, which are the functions explorer uses\
therefore - the driver can effectively make files invisible thus it's a mini rootkit\
repo contains src with IDA files as well as a C translation\
