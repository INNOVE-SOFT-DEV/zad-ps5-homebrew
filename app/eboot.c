/* Lanceur ZAD (eboot.bin) : programme minimal, sans libc.
 * Au demarrage de l'application, il lit /data/homebrew/zad/zad.elf et l'envoie
 * au chargeur d'ELF local (127.0.0.1:9021), puis reste actif pendant que ZAD tourne.
 * Il ecrit /data/zad-eboot.log pour le diagnostic. Base sur l'exemple du SDK (GPL-3.0). */

#define ELF_PATH "/data/homebrew/zad/zad.elf"
#define LOG_PATH "/data/zad-eboot.log"
#define ATTENTE_SECONDES 120

long __syscall(long n, ...)
{
    long a1 = 0, a2 = 0, a3 = 0, a4 = 0, a5 = 0, a6 = 0;
    __builtin_va_list ap;
    unsigned long ret;
    char iserror;

    __builtin_va_start(ap, n);
    a1 = __builtin_va_arg(ap, long);
    a2 = __builtin_va_arg(ap, long);
    a3 = __builtin_va_arg(ap, long);
    a4 = __builtin_va_arg(ap, long);
    a5 = __builtin_va_arg(ap, long);
    a6 = __builtin_va_arg(ap, long);
    __builtin_va_end(ap);

    register long r10 __asm__("r10") = a4;
    register long r8 __asm__("r8") = a5;
    register long r9 __asm__("r9") = a6;

    __asm__ __volatile__("syscall"
                         : "=a"(ret), "=@ccc"(iserror), "+r"(r10), "+r"(r8), "+r"(r9)
                         : "a"(n), "D"(a1), "S"(a2), "d"(a3)
                         : "rcx", "r11", "memory");
    return iserror ? -ret : ret;
}

#define SYS_exit 1
#define SYS_read 3
#define SYS_write 4
#define SYS_open 5
#define SYS_close 6
#define SYS_socket 97
#define SYS_connect 98
#define SYS_nanosleep 240

#define O_RDONLY 0
#define O_WRONLY 1
#define O_CREAT 0x200
#define O_APPEND 8

static int klog(const char *s) { return __syscall(0x259, 7, s, 0); }

static unsigned long longueur(const char *s)
{
    unsigned long n = 0;
    while (s[n])
        n++;
    return n;
}

static void journal(const char *s)
{
    long fd = __syscall(SYS_open, LOG_PATH, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd >= 0) {
        __syscall(SYS_write, fd, s, longueur(s));
        __syscall(SYS_write, fd, "\n", 1);
        __syscall(SYS_close, fd);
    }
    klog(s);
    klog("\n");
}

static void dormir(long secondes)
{
    long ts[2] = {secondes, 0};
    __syscall(SYS_nanosleep, ts, 0);
}

struct adresse {
    unsigned char len, famille;
    unsigned short port;
    unsigned int ip;
    char zero[8];
};

static unsigned char tampon[65536];

int main(void)
{
    journal("zad-eboot: demarrage");

    long f = __syscall(SYS_open, ELF_PATH, O_RDONLY, 0);
    if (f < 0) {
        journal("zad-eboot: zad.elf introuvable dans /data/homebrew/zad/");
        dormir(10);
        return 1;
    }
    long s = __syscall(SYS_socket, 2 /* AF_INET */, 1 /* SOCK_STREAM */, 0);
    if (s < 0) {
        journal("zad-eboot: socket impossible");
        return 1;
    }
    struct adresse a = {16, 2, (unsigned short)((9021 >> 8) | ((9021 & 0xff) << 8)), 0x0100007F, {0}};
    if (__syscall(SYS_connect, s, &a, sizeof a) < 0) {
        journal("zad-eboot: chargeur d'ELF injoignable sur 127.0.0.1:9021");
        dormir(10);
        return 1;
    }

    long total = 0, n;
    while ((n = __syscall(SYS_read, f, tampon, sizeof tampon)) > 0) {
        long envoye = 0;
        while (envoye < n) {
            long w = __syscall(SYS_write, s, tampon + envoye, n - envoye);
            if (w <= 0) {
                journal("zad-eboot: envoi interrompu");
                return 1;
            }
            envoye += w;
        }
        total += n;
    }
    __syscall(SYS_close, f);
    __syscall(SYS_close, s);
    journal(total > 0 ? "zad-eboot: zad.elf envoye au chargeur" : "zad-eboot: fichier vide");

    /* rester ouvert pendant que ZAD tourne, pour garder l'application au premier plan */
    for (int i = 0; i < ATTENTE_SECONDES; i++)
        dormir(1);
    return 0;
}

void _start(void)
{
    __syscall(SYS_exit, main());
    __builtin_trap();
}
