// gcc -Wall -Wextra -O3 -D_FORTIFY_SOURCE=2 -fPIE -pie -o fastcheck main.c
// mkdir build; cd build; cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..; ninja -v

#include <arpa/inet.h>
#include <dirent.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <unistd.h>

void print_memory_info(void) {
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        perror("sysinfo failed");
        return;
    }

    unsigned long total_memory = info.totalram * info.mem_unit;
    unsigned long free_memory = info.freeram * info.mem_unit;

    printf("Total Memory: %lu bytes (%.2f GB)\n", total_memory,
           (double) total_memory / 1024.0 / 1024.0 / 1024.0);
    printf("Free Memory: %lu bytes (%.2f GB)\n", free_memory,
           (double) free_memory / 1024.0 / 1024.0 / 1024.0);
}

void print_cpu_info(void) {
    int num_procs = get_nprocs();
    printf("Number of CPU cores: %d\n", num_procs);
}

void print_hard_drive_info(void) {
    struct statvfs buf;
    if (statvfs("/", &buf) != 0) {
        perror("statvfs error");
        return;
    }

    unsigned long total_size = buf.f_blocks * buf.f_frsize;
    unsigned long free_size = buf.f_bfree * buf.f_frsize;

    printf("Total Hard Drive Size: %lu bytes (%.2f GB)\n", total_size,
           (double) total_size / 1024.0 / 1024.0 / 1024.0);
    printf("Free Hard Drive Space: %lu bytes (%.2f GB)\n", free_size,
           (double) free_size / 1024.0 / 1024.0 / 1024.0);
}

void print_network_interfaces(void) {
    struct ifaddrs *ifaddr;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return;
    }

    printf("Network Interfaces:\n");
    for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;

        void *addr;
        if (ifa->ifa_addr->sa_family == AF_INET) {
            addr = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            addr = &((struct sockaddr_in6 *) ifa->ifa_addr)->sin6_addr;
        } else {
            continue;
        }

        char addr_str[INET6_ADDRSTRLEN];
        inet_ntop(ifa->ifa_addr->sa_family, addr, addr_str, sizeof(addr_str));
        printf("%s: %s\n", ifa->ifa_name, addr_str);
    }

    freeifaddrs(ifaddr);
}

void list_cpu_details(void) {
    char buffer[1024];

    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
        perror("Failed to open /proc/cpuinfo");
        return;
    }

    printf("Detailed CPU Information:\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }

    fclose(fp);
}

void list_gpus(void) {
    const char *path = "/sys/class/drm";
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Failed to open /sys/class/drm");
        return;
    }

    struct dirent *entry;
    printf("GPUs found:\n");
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "card", 4) == 0) {
            char linkpath[512];
            snprintf(linkpath, sizeof(linkpath), "%s/%s/device", path,
                     entry->d_name);
            char device[256];
            ssize_t len = readlink(linkpath, device, sizeof(device) - 1);
            if (len != -1) {
                device[len] = '\0';
                printf("%s -> %s\n", entry->d_name, device);
            } else {
                printf("%s -> Unknown device\n", entry->d_name);
            }
        }
    }

    closedir(dir);
}

void list_usb_devices(void) {
    const char *path = "/sys/bus/usb/devices/";
    DIR *dir = opendir(path);
    if (!(long) dir) {
        perror("Failed to open USB devices directory");
        return;
    }

    struct dirent *entry;
    printf("USB Devices:\n");
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] ==
            '.') // Skip the current and parent directory entries
            continue;

        char devicePath[1024];
        snprintf(devicePath, sizeof(devicePath), "%s%s/product", path,
                 entry->d_name);

        FILE *file = fopen(devicePath, "r");
        if (file) {
            char productName[256];
            if (fgets(productName, sizeof(productName), file)) {
                printf("%s: %s", entry->d_name, productName);
            }
            fclose(file);
        }
    }
    closedir(dir);
}

void list_cd_dvd_devices(void) {
    const char *path = "/sys/class/block";
    DIR *dir = opendir(path);
    if (!(long) dir) {
        perror("Failed to open block devices directory");
        return;
    }

    struct dirent *entry;
    printf("CD/DVD Devices:\n");
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "sr", 2) == 0) {
            printf("%s\n", entry->d_name);
        }
    }
    closedir(dir);
}

void list_sd_card_readers(void) {
    const char *path = "/sys/class/block";
    DIR *dir = opendir(path);
    if (!(long) dir) {
        perror("Failed to open block devices directory");
        return;
    }

    struct dirent *entry;
    printf("SD Card Readers:\n");
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "mmcblk", 6) == 0) {
            printf("%s\n", entry->d_name);
        }
    }
    closedir(dir);
}

void list_webcams(void) {
    const char *path = "/dev";
    DIR *dir = opendir(path);
    if (!(long) dir) {
        perror("Failed to open /dev directory");
        return;
    }

    struct dirent *entry;
    printf("Webcams:\n");
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "video", 5) == 0) {
            printf("%s\n", entry->d_name);
        }
    }
    closedir(dir);
}

void list_sound_output_devices(void) {
    char line[256];
    FILE *file = fopen("/proc/asound/cards", "r");
    if (!(long) file) {
        perror("Failed to read /proc/asound/cards");
        return;
    }

    printf("Sound Output Devices:\n");
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }

    fclose(file);
}

void list_temperatures(void) {
    struct dirent *entry;

    printf("Temperatures:\n");
    DIR *dir = opendir("/sys/class/thermal/");
    if (!(long) dir) {
        perror("Failed to open /sys/class/thermal");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "thermal_zone", 12) == 0) {
            char path[512];
            snprintf(path, sizeof(path), "/sys/class/thermal/%s/temp",
                     entry->d_name);
            FILE *file = fopen(path, "r");
            if (file) {
                char temp[256];
                if (fgets(temp, sizeof(temp), file) != NULL) {
                    char *ptr;
                    long temperature = strtol(temp, &ptr, 10) / 1000;
                    printf("%s: %ld°C\n", entry->d_name, temperature);
                }
                fclose(file);
            }
        }
    }
    closedir(dir);
}

int main(void) {
    print_memory_info();
    print_cpu_info();
    list_cpu_details();
    print_hard_drive_info();
    print_network_interfaces();
    list_gpus();
    list_usb_devices();
    list_cd_dvd_devices();
    list_sd_card_readers();
    list_webcams();
    list_sound_output_devices();
    list_temperatures();
    return 0;
}
