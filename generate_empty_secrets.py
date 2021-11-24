from os.path import exists

if exists("include/secrets.h"):
    exit()
with open("include/secrets.h", "w") as secrets:
    secrets.write("""
#define WIFI_SSID "..."
#define WIFI_PASSWORD "..."
    """.strip())