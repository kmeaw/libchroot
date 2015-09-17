#define _GNU_SOURCE
#include <asm-generic/fcntl.h>
#include <dlfcn.h>
#include <errno.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int (*real_open) (const char *file, int flags, mode_t mode) = 0;
static int (*real_open64) (const char *file, int flags, mode_t mode) = 0;
static int fdnull, fdzero;

int
open (const char *file, int flags, mode_t mode)
{
  if (0 == strcmp ("/dev/null", file))
    return dup (fdnull);
  if (0 == strcmp ("/dev/zero", file))
    return dup (fdzero);

  return real_open (file, flags, mode);
}

int
open64 (const char *file, int flags, mode_t mode)
{
  if (0 == strcmp ("/dev/null", file))
    return dup (fdnull);
  if (0 == strcmp ("/dev/zero", file))
    return dup (fdzero);

  return real_open64 (file, flags, mode);
}

void __attribute__ ((constructor)) run ()
{
  char *dir = 0, *uid_str = 0, *gid_str = 0, *user_str = 0;
  int uid = 0, gid = 0;

  if (0 == real_open)
    {
      if (0 == (real_open = dlsym (RTLD_NEXT, "open")))
	{
	  fprintf (stderr, "libchroot: dlsym(open): %s.\n", dlerror ());
	  abort ();
	}
      if (0 == (real_open64 = dlsym (RTLD_NEXT, "open64")))
	{
	  fprintf (stderr, "libchroot: dlsym(open): %s.\n", dlerror ());
	  abort ();
	}
      if (-1 == (fdzero = real_open ("/dev/zero", O_RDWR, 0)))
	{
	  perror ("open /dev/zero failed");
	  abort ();
	}
      if (-1 == (fdnull = real_open ("/dev/null", O_RDWR, 0)))
	{
	  perror ("open /dev/null failed");
	  abort ();
	}
    }

  if (0 == (dir = getenv ("CHROOT")))
    {
      fputs ("libchroot: You forgot to specify $CHROOT.", stderr);
      abort ();
    }

  if ((gid_str = getenv ("SUDO_GID")))
    {
      gid = atoi (gid_str);
      if ((user_str = getenv ("SUDO_USER")))
	{
	  if (-1 == initgroups (user_str, gid))
	    {
	      perror ("initgroups");
	      abort ();
	    }
	}
    }

  if (chdir (dir) == -1)
    {
      perror ("chdir");
      abort ();
    }
  if (chroot (dir) == -1)
    {
      perror ("libchroot: chroot");
      abort ();
    }

  if (0 == (uid_str = getenv ("SUDO_UID")))
    {
      return;
    }

  uid = atoi (uid_str);

  if (uid != 0 && gid != 0)
    {
      if (-1 == setresgid (gid, gid, gid))
	{
	  perror ("setresgid failed");
	  abort ();
	}

      if (-1 == setresuid (uid, uid, uid))
	{
	  perror ("setresuid failed");
	  abort ();
	}
    }
}
