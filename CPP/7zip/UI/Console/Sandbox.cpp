// Sandbox.cpp

#include "Sandbox.h"
#include "../../../Common/C_FileIO.h"
#include "../../../Common/StringConvert.h"

#ifndef __linux__

void Sandbox::Enforce(void) {}

#else

// TODO: don't error out if landlock.h is missing
#include <linux/landlock.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>

#ifndef landlock_create_ruleset
static inline int landlock_create_ruleset(
  const struct landlock_ruleset_attr *const attr,
  const size_t size, const __u32 flags)
{
  return syscall(__NR_landlock_create_ruleset, attr, size, flags);
}
#endif

#ifndef landlock_add_rule
static inline int landlock_add_rule(
  const int ruleset_fd,
  const enum landlock_rule_type rule_type,
  const void *const rule_attr,
  const __u32 flags)
{
  return syscall(__NR_landlock_add_rule, ruleset_fd, rule_type, rule_attr, flags);
}
#endif

#ifndef landlock_restrict_self
static inline int landlock_restrict_self(
  const int ruleset_fd,
  const __u32 flags)
{
  return syscall(__NR_landlock_restrict_self, ruleset_fd, flags);
}
#endif

class Ruleset
{
public:
  int fd;
  Ruleset(): fd(-1) {};
  ~Ruleset()
  {
    close(fd);
  }
  bool Create(const struct landlock_ruleset_attr *ruleset_attr)
  {
    fd = landlock_create_ruleset(ruleset_attr, sizeof(*ruleset_attr), 0);
    return fd != -1;
  }
};

void Sandbox::Enforce(void)
{
  struct landlock_path_beneath_attr path_beneath = {
    .allowed_access = \
      LANDLOCK_ACCESS_FS_REMOVE_DIR |
      LANDLOCK_ACCESS_FS_REMOVE_FILE |
      LANDLOCK_ACCESS_FS_WRITE_FILE |
      LANDLOCK_ACCESS_FS_MAKE_CHAR |
      LANDLOCK_ACCESS_FS_MAKE_DIR |
      LANDLOCK_ACCESS_FS_MAKE_REG |
      LANDLOCK_ACCESS_FS_MAKE_SOCK |
      LANDLOCK_ACCESS_FS_MAKE_FIFO |
      LANDLOCK_ACCESS_FS_MAKE_BLOCK |
      LANDLOCK_ACCESS_FS_MAKE_SYM,
    .parent_fd = -1,
  };
  const struct landlock_ruleset_attr ruleset_attr = {
    .handled_access_fs = \
      path_beneath.allowed_access |
      LANDLOCK_ACCESS_FS_EXECUTE |
      LANDLOCK_ACCESS_FS_READ_FILE |
      LANDLOCK_ACCESS_FS_READ_DIR,
  };
  Ruleset ruleset = Ruleset();

  if (!ruleset.Create(&ruleset_attr)) {
    // TODO: remove status message
    if (ErrorStream)
      *ErrorStream << "landlock: Failed to create a ruleset: " << strerror(errno) << endl;
    // Silently ignores sandboxing if Landlock is not available (i.e. best-effort security).
    return;
  }

  if (!WritableDir->IsEmpty()) {
#ifdef USE_UNICODE_FSTRING
    AString astr = UnicodeStringToMultiByte(*WritableDir);
#else
    AString astr = *WritableDir;
#endif

    NC::NFile::NIO::CPathFile path;
    if (!path.Open(astr)) {
      if (ErrorStream)
        *ErrorStream << "landlock: Failed to open path " << astr << endl;
      return;
    }

    path_beneath.parent_fd = path.GetHandle();
    if (landlock_add_rule(ruleset.fd, LANDLOCK_RULE_PATH_BENEATH, &path_beneath, 0)) {
      if (ErrorStream)
        *ErrorStream << "landlock: Failed to update the ruleset: " << strerror(errno) << endl;
      return;
    }

    // TODO: remove this status message
    if (ErrorStream)
        *ErrorStream << "landock: Writable directory: " << astr << endl;
  }

  if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) {
    if (ErrorStream)
      *ErrorStream << "landlock: Failed to restrict privileges: " << strerror(errno) << endl;
    return;
  }
  if (landlock_restrict_self(ruleset.fd, 0)) {
    if (ErrorStream)
      *ErrorStream << "landlock: Failed to enforce the ruleset: " << strerror(errno) << endl;
    return;
  }
  // TODO: remove this status message
  if (ErrorStream)
    *ErrorStream << "landock: Successfully sandboxed!" << endl;
}

#endif
